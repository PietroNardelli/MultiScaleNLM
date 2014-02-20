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
#include "math.h"

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

        m_MinimumLevel = 1.0;
        m_MaximumLevel = 4.0;
        m_NumberOfLevelSteps = 4;

        m_scalesImage = NULL;
}

template< class TInputImage, class TOutputImage >
NLMFilter< TInputImage, TOutputImage >
::~NLMFilter()
{
	m_Features      = NULL;
        m_scalesImage = NULL;
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

		this->ComputeOptimalScale();
		this->ComputeLSWeights( this->GetRComp(), m_Order ); //add m_Order in XML

		L0Pointer l0 = L0Type::New();
		L1Pointer l1 = L1Type::New();
		L2Pointer l2 = L2Type::New();

		l0->SetRadius( this->GetRComp()[0] );
		l0->SetCoordinate( 0 );
		l0->SetScalesImage( m_scalesImage );
		l0->SetMinimumLevel( m_MinimumLevel );
		l0->SetMaximumLevel( m_MaximumLevel );
		l0->SetNumberOfLevels( m_NumberOfLevelSteps );

		l1->SetRadius( this->GetRComp()[1] );
		l1->SetCoordinate( 1 );
		l1->SetScalesImage( m_scalesImage );

		l2->SetRadius( this->GetRComp()[2] );
		l2->SetCoordinate( 2 );
		l2->SetScalesImage( m_scalesImage );

		l0->SetInput( this->GetInput() );
		l1->SetInput( l0->GetOutput() );
		l2->SetInput( l1->GetOutput() );
                std::cout<<"Before L0"<<std::endl;
                l0->Update();
                std::cout<<"After L0"<<std::endl;
                l1->Update();
		l2->Update();
		m_Features = l2->GetOutput();
		l0 = NULL;
		l1 = NULL;
		l2 = NULL;
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
	ImageRegionConstIteratorWithIndex<FeaturesMapType> mit;     // Iterator for the map of local featrues
	ImageRegionIterator<OutputImageType>               it;      // Iterator for the output image
	ImageRegionConstIterator<InputImageType>           search;  // Search iterator
	ImageRegionConstIterator<FeaturesMapType>          msit;    // Iterator for search in the map of local features
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
	float normNoise   = ( m_H * m_Sigma * m_Sigma ) * ComputeTraceMO1( this->GetRComp() );
	normNoise         = 1.0f/normNoise;
	float tho0        = m_PSTh*( m_H * m_Sigma * m_Sigma )*ComputeTraceMO0( this->GetRComp() );
	float tho1        = m_PSTh/normNoise;
	/*float lsnorm[TInputImage::ImageDimension];
	for( unsigned int k=0; k<TInputImage::ImageDimension; ++k ){
		lsnorm[k] = itk::NumericTraits<float>::Zero;
		//=====================================================================
		float* weight = new float[m_RComp[k]];
		float  wsum   = itk::NumericTraits<float>::Zero;
		for( int j=0; j<((int)m_RComp[k]); ++j ){
			weight[j]  = ::exp( -((float)(m_RComp[k]-j)*(m_RComp[k]-j))/2.0f );
			wsum      += 2.0f*weight[j];
		}
		wsum += weight[m_RComp[k]-1];
		wsum  = 1.0f/wsum;
		//=====================================================================
		for( int j=-((int)m_RComp[k]); j<0; ++j )
			lsnorm[k] += 2.0f * j*j * weight[j+m_RComp[k]] * wsum;
		//=====================================================================
		delete[] weight;
		lsnorm[k]  = 1.0f/lsnorm[k];
	}*/
	//==================================================================================================================================
	mit = ImageRegionConstIteratorWithIndex<FeaturesMapType>( m_Features, outputRegionForThread );
	it  = ImageRegionIterator<OutputImageType>(              output,     outputRegionForThread );

	ImageRegionConstIterator<ScalesImageType> scalesIt;
	scalesIt = ImageRegionConstIterator<ScalesImageType>( m_scalesImage, outputRegionForThread ); 

	InputImageIndexType originR;
	InputImageSizeType  radiusR;
	radiusR = m_RSearch;

	//==================================================================================================================================
	for( it.GoToBegin(),mit.GoToBegin(),scalesIt.GoToBegin(); !mit.IsAtEnd(); ++it,++mit,++scalesIt){
		//-------------------------------------------------------------------------------------------------------------
		// CREATE THE REGION TO SEARCH AND THE ITERATORS:
		searchSize = baseSearchSize;
		originR    = mit.GetIndex() - radiusR;
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
					midPosition += ( mit.GetIndex()[d] - originR[d] )*aux;
				}
			}
		}
		// ---------------------
		searchRegion.SetIndex( originR );
		searchRegion.SetSize( searchSize );
		search = ImageRegionConstIterator<InputImageType>(   input,      searchRegion  );
		msit   = ImageRegionConstIterator<FeaturesMapType>(  m_Features, searchRegion  );

		// Compute LS norm
		float lsnorm[TInputImage::ImageDimension];
		double level = scalesIt.Get();
		for( unsigned int k=0; k<TInputImage::ImageDimension; ++k ){
			lsnorm[k] = itk::NumericTraits<float>::Zero;
			//=====================================================================
			float* normWeight = new float[m_RComp[k]];
			float  wsum   = itk::NumericTraits<float>::Zero;
			for( int j=0; j<((int)m_RComp[k]); ++j ){
				normWeight[j]  = ::exp( -((float)(m_RComp[k]-j)*(m_RComp[k]-j))/(2.0f*level*level) );
				wsum      += 2.0f*normWeight[j];
			}
			wsum += normWeight[m_RComp[k]-1];
			wsum  = 1.0f/wsum;
			//=====================================================================
			for( int j=-((int)m_RComp[k]); j<0; ++j )
				lsnorm[k] += 2.0f * j*j * normWeight[j+m_RComp[k]] * wsum;
			//=====================================================================
			delete[] normWeight;
			lsnorm[k]  = 1.0f/lsnorm[k];
		}
	
		//-------------------------------------------------------------------------------------------------------------
		// FILTER THE PIXEL
		LSGradientsL2    center = mit.Get();
		float norm     = itk::NumericTraits<float>::Zero;    // To normalize the weights to sum to 1
		float filtered = itk::NumericTraits<float>::Zero;
		float weight;
		unsigned int pos; // Auxiliar variable
		for( pos=0,search.GoToBegin(),msit.GoToBegin(); !search.IsAtEnd(); ++search,++msit,++pos ){
			if( pos!=midPosition ){
				LSGradientsL2 value  = msit.Get();
				weight               = (center.LLL-value.LLL)*(value.LLL-center.LLL);
				if( weight > -tho0 ){
					
					weight              += (center.HLL-value.HLL)*(value.HLL-center.HLL)*lsnorm[0];
					weight              += (center.LHL-value.LHL)*(value.LHL-center.LHL)*lsnorm[1];
					weight              += (center.LLH-value.LLH)*(value.LLH-center.LLH)*lsnorm[2];
					if( weight > -tho1 ){
						weight          *= normNoise;
						//==========================================================================
						// Computing the exponential is painfully slow; instead, a rational approxima-
						// tion may be taken that very closely fits the exponential curve in the range
						// [0,1.6]. Far from this range, the error between the curves can reach 0.04,
						// but fortunately the exponential curve vanish to 0.1 from 2.3, so the overall
						// error is relatively small. As an example, the RMSD between the curves in the
						// range [0,2.7] is 0.0391. The RMSD between two exponential curves with h=1.0
						// and h=0.8 in this same range is 0.1021. Hence, this error is negligible.
						//
						//weight           = exp( weight );
						float temp       = 1.0f/(1.0f-weight);
						weight           = temp*(0.5f*(2.0f+weight)) - temp*temp*(0.5f*weight);
						//==========================================================================
						filtered        += ( (float)(search.Get()) ) * weight;
						norm            += weight;
					}
				}
			}
			else{
				weight   = 0.367879441171442f;
				filtered += ( (float)(search.Get()) ) * weight;
				norm     += weight;
			}
		}
		filtered = filtered/norm;
		//filtered = ( filtered>0.0f ? ::sqrt(filtered) : 0.0f );
		// Set the output pixel
		it.Set(   static_cast<OutputPixelType>( filtered )   );
	}
}


	
template< class TInputImage, class TOutputImage >
float NLMFilter< TInputImage, TOutputImage >
::ComputeTraceMO0( const InputImageSizeType& rcomp )
{
	unsigned int size = 1;
	for( unsigned int k=0; k<TInputImage::ImageDimension; ++k )
		size *= (2*rcomp[k]+1);
	typedef itk::ConstNeighborhoodIterator<InputImageType> IteratorType;
	IteratorType bit = IteratorType( rcomp, this->GetInput(), this->GetInput()->GetBufferedRegion()  );
	typename IteratorType::OffsetType idx;
	bit.GoToBegin();
	float norm  = itk::NumericTraits<float>::Zero;
	float trace = itk::NumericTraits<float>::Zero;
	for( unsigned int k=0; k<size/2; ++k ){
		idx    = bit.GetOffset(k);
		float aux = itk::NumericTraits<float>::Zero;
		for( unsigned int j=0; j<TInputImage::ImageDimension; ++j )
			aux += ((float)idx[j])*((float)idx[j]);
		norm  += ::exp(-aux/2);
		trace += ::exp(-aux);
	}
	norm  = 2.0f*norm  + ::exp(-0.5f);
	trace = 2.0f*trace + ::exp(-1.0f);
	return(trace/norm/norm);
}


template< class TInputImage, class TOutputImage >
float NLMFilter< TInputImage, TOutputImage >
::ComputeTraceMO1( const InputImageSizeType& rcomp )
{
	unsigned int size = 1;
	for( unsigned int k=0; k<TInputImage::ImageDimension; ++k )
		size *= (2*rcomp[k]+1);
	typedef itk::ConstNeighborhoodIterator<InputImageType> IteratorType;
	IteratorType bit = IteratorType( rcomp, this->GetInput(), this->GetInput()->GetBufferedRegion()  );
	typename IteratorType::OffsetType idx;
	bit.GoToBegin();
	float norm  = itk::NumericTraits<float>::Zero;
	float trace = itk::NumericTraits<float>::Zero;
	for( unsigned int k=0; k<size/2; ++k ){
		idx    = bit.GetOffset(k);
		float aux = itk::NumericTraits<float>::Zero;
		for( unsigned int j=0; j<TInputImage::ImageDimension; ++j )
			aux += ((float)idx[j])*((float)idx[j]);
		norm  += ::exp(-aux/2);
		trace += ::exp(-aux);
	}
	norm  = 2.0f*norm  + ::exp(-0.5f);
	trace = 2.0f*trace + ::exp(-1.0f);
	trace = trace/norm/norm;
	if( TInputImage::ImageDimension==2 )
		trace = 30.0f*trace*trace; 
	else if( TInputImage::ImageDimension==3 )
		trace = 126.6f*trace*trace; 
	else
		trace = 0.1f;
	return(trace);
}

template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::ComputeOptimalScale()
{
	MultiScaleFilterPointer multiScaleFilter = MultiScaleFilterType::New();

    	multiScaleFilter->SetSigmaMinimum( this->m_MinimumLevel );
    	multiScaleFilter->SetSigmaMaximum( this->m_MaximumLevel );
   	multiScaleFilter->SetNumberOfSigmaSteps( this->m_NumberOfLevelSteps );
   	multiScaleFilter->SetNonNegativeHessianBasedMeasure( true );
       	multiScaleFilter->SetSigmaStepMethod( 0 );
	InputImageConstPointer   input   =  this->GetInput();
    	multiScaleFilter->SetInput( input );

    	//Setup the functor and connect it to the filter. 
   
      	FunctorPointer functor = FunctorType::New();
      	multiScaleFilter->SetUnaryFunctor( functor );
	multiScaleFilter->SetGenerateScalesOutput( 1 );
	multiScaleFilter->Update();
   
        m_scalesImage = multiScaleFilter->GetScalesOutput();

        multiScaleFilter = NULL;

}

template< class TInputImage, class TOutputImage >
void NLMFilter< TInputImage, TOutputImage >
::ComputeLSWeights( const InputImageSizeType& rcomp, unsigned int order)
{
	unsigned int size = 1;
	for( unsigned int k=0; k<TInputImage::ImageDimension; ++k )
		size *= (2*rcomp[k]+1);
	typedef itk::ConstNeighborhoodIterator<InputImageType> IteratorType;
	IteratorType bit = IteratorType( rcomp, this->GetInput(), this->GetInput()->GetBufferedRegion()  );
	typename IteratorType::OffsetType idx;
	bit.GoToBegin();
	float norm  = itk::NumericTraits<float>::Zero;
	float trace = itk::NumericTraits<float>::Zero;
	for( unsigned int k=0; k<size/2; ++k ){
		idx    = bit.GetOffset(k);
		float aux = itk::NumericTraits<float>::Zero;
		for( unsigned int j=0; j<TInputImage::ImageDimension; ++j )
			aux += ((float)idx[j])*((float)idx[j]);
		norm  += ::exp(-aux/2);
		trace += ::exp(-aux);
	}
	norm  = 2.0f*norm  + ::exp(-0.5f);
	trace = 2.0f*trace + ::exp(-1.0f);
	trace = trace/norm/norm;
	if( TInputImage::ImageDimension==2 )
		trace = 30.0f*trace*trace; 
	else if( TInputImage::ImageDimension==3 )
		trace = 126.6f*trace*trace; 
	else
		trace = 0.1f;
	return(trace);
}


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
}

	
} // end namespace itk


#endif
