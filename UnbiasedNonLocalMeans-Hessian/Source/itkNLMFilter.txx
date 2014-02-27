/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNLMFilter.txx,v $
  Language:  C++
  Date:      $Date: 2006/01/11 19:43:31 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkNLMFilter_txx
#define _itkNLMFilter_txx
#include "itkNLMFilter.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "math.h"

#include "vnl/vnl_matrix.h"
#include "vnl/algo/vnl_matrix_inverse.h"
#include "vnl/vnl_trace.h"

namespace itk
{
	
template< class TInputImage, class TOutputImage >
NLMFilter< TInputImage, TOutputImage >
::NLMFilter()
{
	if( TInputImage::ImageDimension!=3 )
		itkExceptionMacro( << "This class is supported only for image dimension 3" );
	m_Features      = NULL;
	m_Sigma         = 25.0f;
	m_H             = 1.2f;
	m_PSTh          = 2.3f;
	m_RSearch.Fill(5);
	m_RComp.Fill(2);

        m_MinimumLevel 	     = 1.0;
        m_MaximumLevel 	     = 4.0;
        m_NumberOfLevelSteps = 5;
	m_Order 	     = 2;
	m_Alpha		     = 1.0;

        m_scalesImage 	     = NULL;
	m_strengthImage	     = NULL;
	
	this->m_GenerateScalesOutput = false;
	this->m_GenerateScalesOutput = false;
	this->m_UseEstimatedDistanceMean = false;
	this->m_NormalizedByFeatureStrength = false;
	this->m_UseDeltaFeatureStrength = false;

	this->ProcessObject::SetNumberOfRequiredOutputs( 4 );
	typename FeaturesMapType::Pointer featuresOutputImage = FeaturesMapType::New();
  	this->ProcessObject::SetNthOutput( 1,  featuresOutputImage.GetPointer() );
	typename ScalesImageType::Pointer scalesOutputImage = ScalesImageType::New();
  	this->ProcessObject::SetNthOutput( 2,  scalesOutputImage.GetPointer() );
	typename EigenValueImageType::Pointer strengthOutputImage = EigenValueImageType::New();
  	this->ProcessObject::SetNthOutput( 3,  strengthOutputImage.GetPointer() );

}

template< class TInputImage, class TOutputImage >
NLMFilter< TInputImage, TOutputImage >
::~NLMFilter()
{
	m_Features      = NULL;
        m_scalesImage   = NULL;
	m_strengthImage = NULL;
}
	
	
template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::GenerateInputRequestedRegion()
{
	// Call the superclass' implementation of this method
	Superclass::GenerateInputRequestedRegion();
		
	// Get pointers to the input and output
	InputImagePointer  inputPtr  = const_cast< TInputImage * >( this->GetInput() );
	OutputImagePointer outputPtr = this->GetOutput();
		
	if ( !inputPtr || !outputPtr )
		return;
		
	// Get a copy of the input requested region (should equal the output
	// requested region)
	InputImageRegionType inputRequestedRegion = inputPtr->GetRequestedRegion();
		
	// Pad the input requested region by the operator radius
	InputImageSizeType radius = m_RSearch;
	inputRequestedRegion.PadByRadius( radius );
		
	// Crop the input requested region at the input's largest possible region
	inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion());
	inputPtr->SetRequestedRegion( inputRequestedRegion );
	return;
}

template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::BeforeThreadedGenerateData( void )
{
	if( !m_Features ){
		this->ComputeFeatures();
		this->ComputeLSWeightsAndTrace( this->GetRComp(), m_Order );
	}
}
	
	
template< class TInputImage, class TOutputImage >
#if ITK_VERSION_MAJOR < 4
void NLMFilter< TInputImage, TOutputImage >
::ThreadedGenerateData( const OutputImageRegionType& outputRegionForThread, int itkNotUsed(threadId) )
#else
void NLMFilter< TInputImage, TOutputImage >
::ThreadedGenerateData( const OutputImageRegionType& outputRegionForThread, ThreadIdType itkNotUsed(threadId) )
#endif
{
	//==================================================================================================================================
	// Iterators:
	ImageRegionConstIteratorWithIndex<FeaturesMapType> featuresIt;     	// Iterator for the map of local features (mit)
	ImageRegionIterator<OutputImageType>               outputIt;      	// Iterator for the output image (it)
	ImageRegionConstIterator<InputImageType>           searchIt;  		// Search iterator (search)
	ImageRegionConstIterator<FeaturesMapType>          searchInFeaturesIt;  // Iterator for search in the map of local features (msit)
	ImageRegionConstIterator<ScalesImageType> 	   scalesIt;		// Iterator for the scales image
	ImageRegionConstIterator<ScalesImageType>          searchInScalesIt;    // Iterator for search in the scales image
	ImageRegionConstIterator<EigenValueImageType> 	   strengthIt;		// Iterator for the strength image
	ImageRegionConstIterator<EigenValueImageType> 	   searchInStrengthIt;	// Iterator for search in the strength image

	// Input and output
	InputImageConstPointer   input   =  this->GetInput();
	OutputImagePointer       output  =  this->GetOutput();
	//==================================================================================================================================
	unsigned int numNeighbours = 1;
	InputImageSizeType baseSearchSize, searchSize;
	for( unsigned int d=0; d<TInputImage::ImageDimension; ++d ){ // The number of voxels which are going to be accounted in the WA
		numNeighbours    *= ( 2*m_RSearch[d] + 1 );
		baseSearchSize[d] = ( 2*m_RSearch[d] + 1 );
	}
	InputImageRegionType searchRegion;

	//==================================================================================================================================
	featuresIt = ImageRegionConstIteratorWithIndex<FeaturesMapType>( m_Features,      outputRegionForThread );
	outputIt   = ImageRegionIterator<OutputImageType>(               output,          outputRegionForThread );
	scalesIt   = ImageRegionConstIterator<ScalesImageType>(          m_scalesImage,   outputRegionForThread );
	strengthIt = ImageRegionConstIterator<EigenValueImageType>(	 m_strengthImage, outputRegionForThread);

	InputImageIndexType originR;
	InputImageSizeType  radiusR;
	radiusR = m_RSearch;

	int dim = TInputImage::ImageDimension + 1;
	if( m_Order == 2 )
		dim += TInputImage::ImageDimension*(TInputImage::ImageDimension+1)/2;


	double weight;
        double *firstProduct = new double[dim];
	double distanceMean;
	double deltaStrengthMean;
        double *distance = new double[numNeighbours];
        double *deltaStrength = new double[numNeighbours];

	double *value = new double[dim];
	//==================================================================================================================================
	for( outputIt.GoToBegin(),featuresIt.GoToBegin(),scalesIt.GoToBegin(),strengthIt.GoToBegin(); !featuresIt.IsAtEnd(); ++outputIt,++featuresIt,++scalesIt, ++strengthIt)
	{
		//-------------------------------------------------------------------------------------------------------------
		// CREATE THE REGION TO SEARCH AND THE ITERATORS:
		searchSize = baseSearchSize;
		originR    = featuresIt.GetIndex() - radiusR;
		bool         needToComputeCenter = false;
		unsigned int midPosition         = numNeighbours/2;
		for( unsigned int d=0; d<TInputImage::ImageDimension; ++d ){
			if( originR[d]<0 ){
				searchSize[d] += originR[d];
				originR[d]     = 0;
				needToComputeCenter = true;
			}
			if( originR[d]+searchSize[d] > input->GetLargestPossibleRegion().GetSize()[d] ){
				searchSize[d]       = input->GetLargestPossibleRegion().GetSize()[d] - originR[d];
				needToComputeCenter = true;
			}
		}
		// ---------------------
		// Compute the index corresponding to the original center:
		if( needToComputeCenter ){
			unsigned int aux = 1;
			for( unsigned int d=0; d<TInputImage::ImageDimension; ++d )
				aux *= searchSize[d];
			midPosition = 0;
			if( aux>0 ){
				for( int d=(int)TInputImage::ImageDimension-1; d>=0; --d ){
					aux /= searchSize[d];
					midPosition += ( featuresIt.GetIndex()[d] - originR[d] )*aux;
				}
			}
		}
		// ---------------------
		searchRegion.SetIndex( originR );
		searchRegion.SetSize( searchSize );
		searchIt   = ImageRegionConstIterator<InputImageType>(              input,           searchRegion  );
		searchInFeaturesIt = ImageRegionConstIterator<FeaturesMapType>(     m_Features,      searchRegion  ); 
		searchInStrengthIt = ImageRegionConstIterator<EigenValueImageType>( m_strengthImage, searchRegion  );
		searchInScalesIt   = ImageRegionConstIterator<ScalesImageType>(     m_scalesImage,   searchRegion  ); 

		//-------------------------------------------------------------------------------------------------------------
		// FILTER THE PIXEL

		VariableVectorType  center         = featuresIt.Get();
   		double 		    centerScale    = scalesIt.Get();
		double 		    centerStrength = strengthIt.Get();

		double norm         = itk::NumericTraits<double>::Zero;    // To normalize the weights to sum to 1
		double meanDistance = itk::NumericTraits<double>::Zero;    // To use instead of normNoise
		double filtered     = itk::NumericTraits<double>::Zero;
		double normStrength = itk::NumericTraits<double>::Zero;

		//double weight;
		distanceMean = itk::NumericTraits<double>::Zero;
		deltaStrengthMean = itk::NumericTraits<double>::Zero;
		unsigned int pos; // Auxiliar variable
		
		vnl_matrix<double> BMatrix = m_BMatrixMap[centerScale];
		std::map< unsigned int, std::vector<unsigned int> > indexMap = m_IndexVectorMap[centerScale];
	
		double B_i_Ord0 = m_BiOrd0Map[centerScale];

		double trace0 = m_TraceOrder0Map[centerScale];
		double traceMaxOrder = m_TraceMaxOrderMap[centerScale];
		double normNoise  = traceMaxOrder *( m_H * m_Sigma * m_Sigma ); 
		normNoise         = 1.0/normNoise;
		double tho0       = m_PSTh * trace0 * ( m_H * m_Sigma * m_Sigma );
		double tho1       = m_PSTh/normNoise;
		/*if(tho1 < 300.0 )
		{
			tho1 *= (m_RComp[0]*2+1);
		}
		tho1 = 2000;*/

		for( pos=0,searchIt.GoToBegin(),searchInFeaturesIt.GoToBegin(), searchInStrengthIt.GoToBegin(), searchInScalesIt.GoToBegin();
		     !searchIt.IsAtEnd(); 
                     ++searchIt,++searchInFeaturesIt,++pos,++searchInStrengthIt, ++searchInScalesIt)
		{
                        distance[pos]=0.0;
			deltaStrength[pos]=0.0;
			if( pos!=midPosition )
			{
				double valueScale = searchInScalesIt.Get();
				std::vector<double> sigmasCouple;
				sigmasCouple.push_back(centerScale);	
				sigmasCouple.push_back(valueScale);					
		
				VariableVectorType  originalValue = searchInFeaturesIt.Get();

				double B_ij_Ord0 = m_BijOrd0Map[sigmasCouple];

				double value_ord0 = B_ij_Ord0 * originalValue[0];
				double tmp = (center[0] - value_ord0) * B_i_Ord0 * (value_ord0 - center[0]); // To get the proper distance for order 0		

				//If distance based on order 0 is small enough, we compute it using the selected order.
				// Otherwise, we use order zero as a good approximation.
        			if (tmp > -tho0)
				{
					vnl_matrix<double>  B_ij  = m_BijMatrixMap[sigmasCouple];
					std::map< unsigned int, std::vector<unsigned int> > BijIndMap = m_BijIndexMap[sigmasCouple];

					for( unsigned int row = 0; row < B_ij.rows(); row++ )
					{
						value[row] = 0.0;
						std::vector<unsigned int> columns = BijIndMap[row];
						for( unsigned int index = 0; index < columns.size(); index++ )
						{
							value[row] += B_ij(row,columns[index]) * ( originalValue[columns[index]] );
						}
					}

					for( unsigned int row = 0; row < BMatrix.rows(); row++ )
					{
						firstProduct[row] = 0.0;
						std::vector<unsigned int> columns = indexMap[row];
						for( unsigned int index = 0; index < columns.size(); index++ )
						{
							firstProduct[row] += BMatrix(row,columns[index])*( value[columns[index]] - center[columns[index]] );
						}
					}
					for( unsigned int row = 0; row < BMatrix.rows(); row++ )
					{
						distance[pos] += ( center[row] - value[row] ) * firstProduct[row];
					}
				}
				else
				{
					distance[pos] = tmp;
				}

				distanceMean +=distance[pos];

				double valueStrength = searchInStrengthIt.Get();
				deltaStrength[pos] = vnl_math_abs( centerStrength - valueStrength);
				deltaStrengthMean += deltaStrength[pos];
			}
			else
			{
				//distanceMean += 0;
				//deltaStrengthMean += 0;
			}
		}

		distanceMean = vnl_math_abs(distanceMean/numNeighbours);
		distanceMean = 1 / distanceMean;
		deltaStrengthMean = deltaStrengthMean/numNeighbours;
		deltaStrengthMean = 1.0/deltaStrengthMean;
		for( pos=0,searchIt.GoToBegin(), searchInStrengthIt.GoToBegin(); !searchIt.IsAtEnd(); ++searchIt,++pos,++searchInStrengthIt )
		{
		        weight = itk::NumericTraits<double>::Zero;
			if( pos!=midPosition )
			{
				double valueStrength = searchInStrengthIt.Get();
				normStrength = 1.0;
				if (m_NormalizedByFeatureStrength)
				{
					normStrength = 1/(centerStrength * valueStrength);
				}

				weight = distance[pos];
  
				if (weight > -tho0 ) {
				/*	
				if( (center[0] - value[0]) * (value[0] - center[0]) > -tho0 )
				{
					for( unsigned int row = 0; row < BMatrix.rows(); row++ )
					{
						firstProduct[row] = 0;
						std::vector<unsigned int> columns = indexMap[row];
						for( unsigned int index = 0; index < columns.size(); index++ )
						{
							firstProduct[row] += BMatrix(row,columns[index])*( value[columns[index]] - center[columns[index]] );
						}
					}
					for( unsigned int row = 0; row < BMatrix.rows(); row++ )
					{
						weight += ( center[row] - value[row] ) * firstProduct[row];
					}
				*/

					if( weight > -tho1 )
					{

						if (m_UseEstimatedDistanceMean) 
						{
							weight *= distanceMean;
						} 
						else
						{	
							weight *= normNoise;
						}
						if (m_NormalizedByFeatureStrength)
						{
							weight *= normStrength;
						}

						if (m_UseDeltaFeatureStrength)
						{
							weight -= m_Alpha * deltaStrength[pos] * deltaStrengthMean; // because negative weight!!
						}
						//==========================================================================
						// Computing the exponential is painfully slow; instead, a rational approxima-
						// tion may be taken that very closely fits the exponential curve in the range
						// [0,1.6]. Far from this range, the error between the curves can reach 0.04,
						// but fortunately the exponential curve vanish to 0.1 from 2.3, so the overall
						// error is relatively small. As an example, the RMSD between the curves in the
						// range [0,2.7] is 0.0391. The RMSD between two exponential curves with h=1.0
						// and h=0.8 in this same range is 0.1021. Hence, this error is negligible.
						//
						weight        = exp( weight );
						//double temp = 1.0/(1.0-weight);
						//weight      = temp*(0.5*(2.0+weight)) - temp*temp*(0.5*weight);
						//==========================================================================
						filtered    += ( (double)(searchIt.Get()) ) * weight;
						norm        += weight;
					} 

				}
			}
			else{
				weight   =  0.367879441171442f;
				filtered += ( (double)(searchIt.Get()) ) * weight;
				norm     += weight;
			}
		}
		
		filtered = filtered/norm;
		
		// Set the output pixel
		outputIt.Set(   static_cast<OutputPixelType>( filtered )   );
	}
  delete [] firstProduct;
  delete [] distance;
  delete [] deltaStrength;
  delete [] value;	
}

/**
 * ************************ ComputeFeatures *******************************
 */
template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::ComputeFeatures()
{
	unsigned int imageDimension = TInputImage::ImageDimension;
	InputSpacingType spacing = this->GetInput()->GetSpacing();

        OutputImageRegionType outputRegion = this->GetOutput()->GetBufferedRegion();
	
  	MultiScaleFilterPointer multiScaleFilter = MultiScaleFilterType::New();

    	multiScaleFilter->SetSigmaMinimum( this->m_MinimumLevel );
    	multiScaleFilter->SetSigmaMaximum( this->m_MaximumLevel );
   	multiScaleFilter->SetNumberOfSigmaSteps( this->m_NumberOfLevelSteps );
   	multiScaleFilter->SetNonNegativeHessianBasedMeasure( true );
       	multiScaleFilter->SetSigmaStepMethod( 0 );
	multiScaleFilter->SetRescale( 0 );

	InputImageConstPointer  input = this->GetInput();
    	multiScaleFilter->SetInput( input );

    	//Setup the functor and connect it to the filter.    
      	FunctorPointer functor = FunctorType::New();
  	const double pi = vnl_math::pi;
  	double sqrtProduct = vcl_sqrt( 2 * pi);
	double functorSpacing = 1;
	for( unsigned int i = 0; i < imageDimension; i++)
	{
		functorSpacing *= spacing[i]*spacing[i];
	}
  	double threshold = ( sqrtProduct * m_Sigma) / functorSpacing;
	functor->SetThreshold( threshold );

      	multiScaleFilter->SetUnaryFunctor( functor );
	multiScaleFilter->SetGenerateScalesOutput( 1 );
	multiScaleFilter->Update();

	// Get the general max Strenghts Image 
	m_strengthImage = multiScaleFilter->GetOutput();

	// Graft m_scalesImage to the second output to return it if requested
	if ( this->m_GenerateStrengthOutput )
  	{
		this->GraftNthOutput(3, const_cast<EigenValueImageType*>(multiScaleFilter->GetOutput()) );
	}
	// Get the Scales Image containing the scales at which each pixel gave the best response
        m_scalesImage = multiScaleFilter->GetScalesOutput();

	// Graft m_scalesImage to the second output to return it if requested
	if ( this->m_GenerateScalesOutput )
  	{
		this->GraftNthOutput(2, const_cast<ScalesImageType*>(multiScaleFilter->GetScalesOutput()) );
	}

	// Get the Mean Image and create the iterator
	MeanImageConstPointer meanImage = multiScaleFilter->GetGaussianOutput();
	ImageRegionConstIterator<MeanImageType> meanIter( meanImage, outputRegion );	
	// Get the Gradient Image and create the iterator
	GradientImageConstPointer  gradientImage = multiScaleFilter->GetGradientOutput();
	ImageRegionConstIterator<GradientImageType> gradientIter( gradientImage, outputRegion );

	// Get the Hessian Image and create the iterator (only if Order 2 is requested)
	ImageRegionConstIterator<HessianTensorImageType> hessianIter;
	if( m_Order == 2 )
	{
		HessianTensorConstPointer  hessianImage  = multiScaleFilter->GetHessianOutput();
		hessianIter = ImageRegionConstIterator<HessianTensorImageType>( hessianImage, outputRegion );
		hessianIter.GoToBegin();
	}	

	// Create the Features Image and the iterator
	//m_Features = FeaturesMapType::New();
	m_Features = dynamic_cast<FeaturesMapType*>( this->ProcessObject::GetOutput( 1 ) );
	m_Features->SetBufferedRegion( outputRegion );

    	unsigned int dim = imageDimension+1;
	
	if( m_Order == 2 )
	{
		dim += imageDimension*(imageDimension+1)/2;
	}
	m_Features->SetVectorLength( dim );
	m_Features->Allocate();
	ImageRegionIterator<FeaturesMapType> featuresIter( m_Features, outputRegion );

	meanIter.GoToBegin();
    	gradientIter.GoToBegin();
	featuresIter.GoToBegin();

	unsigned int gSize =  gradientIter.Value().Size();

	VariableVectorType features;
	features.SetSize( dim );

	while ( !meanIter.IsAtEnd() ) 
    	{
		// Set the Mean value
		features[0] = meanIter.Value(); 

		// Set the Gradient value
		for( unsigned int g = 0; g < gSize; g++ )
			features[g+1] = gradientIter.Value().GetElement(g);

		// Set the Hessian value (Only if Order 2 is requested)
		if( m_Order == 2 )
		{
			unsigned int hSize =  hessianIter.Value().GetNumberOfComponents();
			for( unsigned int h = 0; h < hSize; h++ )  
				features[gSize+1+h] = hessianIter.Value().GetNthComponent(h); 
			++hessianIter;
		}
		featuresIter.Set(features);

		++meanIter; ++gradientIter; ++featuresIter;		
	}
        multiScaleFilter = NULL;
}

/**
 * ********************* ComputeLSWeightsAndTrace ****************************
 */
template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::ComputeLSWeightsAndTrace( const InputImageSizeType& rcomp, unsigned int order)
{
	unsigned int imageDimension = TInputImage::ImageDimension;
	unsigned int size = 1;
        InputSpacingType spacing = this->GetInput()->GetSpacing(); // Taking the physical dimensions of the voxel into account

	for( unsigned int k=0; k<imageDimension; ++k )
		size *= (2*rcomp[k]+1);

	// Create the iterator to get the offset in the Comparison radius
	typedef itk::ConstNeighborhoodIterator<InputImageType> IteratorType;
	IteratorType bit = IteratorType( rcomp, this->GetInput(), this->GetInput()->GetBufferedRegion()  );
	typename IteratorType::OffsetType idx;
	bit.GoToBegin();

	unsigned int XColumns = imageDimension+1;
	
        if( order == 2 )
	{
		XColumns += imageDimension*(imageDimension+1)/2;
	}
	// Computing sigmaValue (all the matrixes depends on it)
	double sigmaValue = this->m_MinimumLevel;
	const double stepSize = vnl_math_max( 1e-10,( this->m_MaximumLevel - this->m_MinimumLevel ) / ( this->m_NumberOfLevelSteps - 1 ) );

	typedef std::pair< unsigned int, std::vector<unsigned int> > indexPairType;
	for( unsigned int level = 0; level < this->m_NumberOfLevelSteps; level++ )  
	{
		// Compute X Matrix (note that we are multiplying by spacing and dividing by sigmaValue to de-normalize the derivatives coming from the multiscale filtering) 
      		sigmaValue = this->m_MinimumLevel + stepSize * level;
		
		vnl_matrix<double>XMatrix(size,XColumns);
		XMatrix = itk::NumericTraits<double>::Zero;  
	
		for( unsigned int row = 0; row < size; row++ ) 
		{
			XMatrix(row,0) = 1;
			idx = bit.GetOffset( row );
			for( unsigned int column = 1; column < imageDimension+1; column++ )
			{
				XMatrix(row,column) = idx[column-1]*spacing[column-1]/sigmaValue;
			}
		}
		if( order == 2 ) 
		{
			for( unsigned int row = 0; row < size; row++ )
			{
				idx = bit.GetOffset( row );
				std::vector<double> values;
				unsigned int column = imageDimension+1;
				//while( column < XColumns )
				//{
					for( unsigned int k = 0; k < imageDimension; k++ )
					{
						for( unsigned int j = k; j < imageDimension; j++ )
						{

							if( j == k )
							{
								XMatrix(row,column) = 0.5*idx[k]*spacing[k]/sigmaValue*idx[j]*spacing[j]/sigmaValue;
								column++;
							}
							else
							{
								XMatrix(row,column) = idx[k]*spacing[k]/sigmaValue*idx[j]*spacing[j]/sigmaValue;
								column++;
							}
						}
					}
				//}
			}		
		}
		// Compute R and B Matrixes. They both depends on sigma value
		vnl_matrix<double>RMatrix(size,size);
		RMatrix.set_identity();
		
		vnl_matrix<double>BMatrix(XColumns,XColumns);
		BMatrix = itk::NumericTraits<double>::Zero;

		double norm = itk::NumericTraits<double>::Zero;

		for( unsigned int row = 0; row < size; row++ ) 
		{
			idx = bit.GetOffset( row );

			double sum  = itk::NumericTraits<double>::Zero;

			for( unsigned int k=0; k<imageDimension; ++k )
			{
				sum += idx[k]*spacing[k]*idx[k]*spacing[k]/(sigmaValue*sigmaValue);
			}
		
			double RValue = ::exp( -( (double)( sum ) ) / 2.0 ); 
			norm += RValue;
			RMatrix(row,row) = RValue;
		}
		norm  = 1.0/norm;
		RMatrix = RMatrix * norm;

    //std::cout<<"sigma: "<<sigmaValue<<" Ri trace:"<<vnl_trace(RMatrix)<<std::endl;
    
    vnl_matrix<double> RXMatrix = RMatrix * XMatrix;
		BMatrix = RXMatrix.transpose() * RXMatrix;
		
    //std::cout<<"sigma: "<<sigmaValue<<" Bi trace:"<<vnl_trace(BMatrix)<<std::endl;

		/** Compute proper coefficients for order (from Bi) */
		//vnl_vector<double> XOrd0   = XMatrix.get_column(0);
		//vnl_vector<double> firstVector = RMatrix * RMatrix * XOrd0;
		//vnl_matrix<double> RSquared = RMatrix * RMatrix;
		double BiOrd0 = 0.0;
		for(unsigned int p = 0; p < size ;p++)
		{
			//BiOrd0 +=  XOrd0[p] * firstVector[p];
			BiOrd0 += RMatrix(p,p)*RMatrix(p,p);
		}

		m_BiOrd0Map.insert( BiOrd0PairType(sigmaValue,BiOrd0) );

		/** Create a vector containing indeces of non-zero values in the BMatrix*/ 
		std::map< unsigned int,std::vector<unsigned int> >  indexMap;

		for( unsigned int row = 0; row < BMatrix.rows(); row++ )
		{
			std::vector< unsigned int > nonZeros;
			for( unsigned int column = 0; column < BMatrix.columns(); column++ )
			{
				if( vnl_math_abs(BMatrix(row,column)) > 0.0001 )
				{		
					nonZeros.push_back( column );							
				}
			}

			indexMap.insert( indexPairType(row, nonZeros) );
		}
		m_IndexVectorMap.insert( IndexVectorPairType(sigmaValue,indexMap) );
		m_BMatrixMap.insert( MatrixPairType(sigmaValue,BMatrix) );

 		m_RMatrixMap.insert( MatrixPairType(sigmaValue,RMatrix) );  
 		m_XMatrixMap.insert( MatrixPairType(sigmaValue,XMatrix) );
    

		// Compute trace for order 0
		double trace0 = vnl_trace<double>(RMatrix);
		m_TraceOrder0Map.insert( TracePairType(sigmaValue, trace0) );
	
		// Compute trace for max order
		/*vnl_matrix<double> TMatrix;
		vnl_matrix<double> XTrMatrix = XMatrix.transpose();
		vnl_matrix<double> pXMatrix  = XTrMatrix * XMatrix;
		vnl_matrix<double> invMatrix = vnl_matrix_inverse<double>(pXMatrix);
		vnl_matrix<double> pMatrix   = XMatrix * invMatrix * XTrMatrix;
		TMatrix = RMatrix * pMatrix;*/

		vnl_matrix<double> XTrMatrix = XMatrix.transpose();
		vnl_matrix<double> pXMatrix  = XTrMatrix * RXMatrix;
		vnl_matrix<double> invMatrix = vnl_matrix_inverse<double>(pXMatrix).inverse();
		//vnl_matrix<double> sMatrix   = XTrMatrix * RMatrix * RMatrix * XMatrix;
		vnl_matrix<double> TMatrix   = invMatrix * BMatrix;
		double trace2 = vnl_trace<double>(TMatrix);
		m_TraceMaxOrderMap.insert( TracePairType(sigmaValue, trace2) );
	}

	/* Compute Bij 	**/
	double sigmaValue_i = this->m_MinimumLevel;
      	double sigmaValue_j = this->m_MinimumLevel;
	

	for( unsigned int i = 0; i < this->m_NumberOfLevelSteps; i++ )  
	{
      		sigmaValue_i = this->m_MinimumLevel + stepSize * i;
		
		vnl_matrix<double> B_i = m_BMatrixMap[sigmaValue_i];
		vnl_matrix<double> R_i = m_RMatrixMap[sigmaValue_i];
		vnl_matrix<double> X_i = m_XMatrixMap[sigmaValue_i];
    vnl_matrix<double> X_it = X_i.transpose();
    vnl_matrix<double> inverseB_i = vnl_matrix_inverse<double>(B_i).inverse();
    //std::cout<<"inverseB_i trace:"<<vnl_trace(inverseB_i)<<std::endl;


		for( unsigned int j = 0; j < this->m_NumberOfLevelSteps; j++ )  //from 0?
		{
      			sigmaValue_j = this->m_MinimumLevel + stepSize * j;

			vnl_matrix<double> R_j = m_RMatrixMap[sigmaValue_j];
			vnl_matrix<double> X_j = m_XMatrixMap[sigmaValue_j];

			vnl_matrix<double> pmm = R_j * X_j;
			vnl_matrix<double> kmm = R_i * pmm;
			vnl_matrix<double> emm = X_it * kmm;
			vnl_matrix<double> B_ij = inverseB_i * emm;

			std::map< unsigned int,std::vector<unsigned int> >  indexMap;

			for( unsigned int row = 0; row < B_ij.rows(); row++ )
			{
				std::vector< unsigned int > nonZeros;
				for( unsigned int column = 0; column < B_ij.columns(); column++ )
				{
					if( vnl_math_abs(B_ij(row,column)) > 0.0001 )
					{		
						nonZeros.push_back( column );							
					}
				}

				indexMap.insert( indexPairType(row, nonZeros) );
			}


			std::vector<double> sigmaCouple;
			sigmaCouple.push_back(sigmaValue_i);
			sigmaCouple.push_back(sigmaValue_j);
			m_BijMatrixMap.insert( BijPairType(sigmaCouple , B_ij) );
			m_BijIndexMap.insert( IndexBijPairType(sigmaCouple,indexMap) );
      std::cout<<"(i,j)="<<sigmaValue_i<<","<<sigmaValue_j<<" Bij trace:"<<vnl_trace(B_ij)<<std::endl;

			
			/** Compute proper coefficients for order (from Bi) */
			//vnl_vector<double> X_iOrd0     = X_i.get_column(0);
			//vnl_vector<double> X_jOrd0     = X_j.get_column(0);
			//vnl_vector<double> firstVector = R_i * R_j * X_jOrd0;
			//vnl_matrix<double> R_ij	= R_i * R_j;
			double BijOrd0 = 0.0;
			for(unsigned int k=0; k< size; k++)
			{
				//BijOrd0 +=  X_iOrd0[k] * firstVector[k];
				BijOrd0 += R_i(k,k)*R_j(k,k);
			}
			double BiOrd0_inv = 1.0 / m_BiOrd0Map[sigmaValue_i];
			BijOrd0 = BiOrd0_inv * BijOrd0;

			m_BijOrd0Map.insert( BijOrd0PairType(sigmaCouple,BijOrd0) );
		}
        }
}

/**
 * ********************* GetFeaturesOutput **************************
 */
template< typename TInputImage, typename TOutputImage >
const typename NLMFilter<TInputImage, TOutputImage >::FeaturesMapType *
NLMFilter< TInputImage, TOutputImage >::GetFeaturesOutput( void ) const
{
  return static_cast<const FeaturesMapType*>(this->ProcessObject::GetOutput(1));
} 

/**
 * ********************* GetScalesOutput ****************************
 */
template< typename TInputImage, typename TOutputImage >
const typename NLMFilter<TInputImage, TOutputImage >::ScalesImageType *
NLMFilter< TInputImage, TOutputImage >::GetScalesOutput( void ) const
{
  return static_cast<const ScalesImageType*>(this->ProcessObject::GetOutput(2));
} 

/**
 * ********************* GetStrengthOutput ***************************
 */
template< typename TInputImage, typename TOutputImage >
const typename NLMFilter<TInputImage, TOutputImage >::EigenValueImageType *
NLMFilter< TInputImage, TOutputImage >::GetFunctorOutput( void ) const
{
  return static_cast<const EigenValueImageType*>(this->ProcessObject::GetOutput(3));
} 


/**
 * ************************ PrintSelf *******************************
 */

template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::PrintSelf(std::ostream &os, Indent indent) const
{
	os << indent << "Search Size: " << m_RSearch << std::endl;
	os << indent << "Search Comp: " << m_RComp << std::endl;
	os << indent << "Sigma: " << m_Sigma << std::endl;
	os << indent << "H: " << m_H << std::endl;
	os << indent << "PSTh: " << m_PSTh << std::endl;
	os << indent << "MinimumLevel: " << m_MinimumLevel << std::endl;
	os << indent << "MaximumLevel: " << m_MaximumLevel << std::endl;
	os << indent << "NumberOfSigmaSteps: " << m_NumberOfLevelSteps << std::endl;  
	os << indent << "Order: " << m_Order << std::endl;
}

	
} // end namespace itk


#endif
