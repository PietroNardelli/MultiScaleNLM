/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkLSDerivatives.txx,v $
  Language:  C++
  Date:      $Date: 2006/01/11 19:43:31 $
  Version:   $Revision: 1.21 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkLSDerivatives_txx
#define _itkLSDerivatives_txx
#include "itkLSDerivatives.h"

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"

#include <map>
#include <vector>
#include <math.h>
#include <vnl/vnl_math.h>

namespace itk
{

TFilterValuesMap lpfValuesMap;
TFilterValuesMap hpfValuesMap;


//=====================================================================================================
template< class TInputImage, class TScalesImage >
LSDerivativesL0< TInputImage, TScalesImage>
::LSDerivativesL0()
{
	m_Radius     = 2;
	m_Coordinate = 0;
  	m_scalesImage = NULL;
}

template< class TInputImage, class TScalesImage >
LSDerivativesL0< TInputImage, TScalesImage>
::~ LSDerivativesL0()
{
  	m_scalesImage = NULL;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::LSDerivativesL1()
{
	m_Radius     = 2;
	m_Coordinate = 1;
	m_scalesImage = NULL;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::~LSDerivativesL1()
{
	m_scalesImage = NULL;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::LSDerivativesL2()
{
	m_Radius     = 2;
	m_Coordinate = 2;
	m_scalesImage = NULL;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::~LSDerivativesL2()
{
	m_scalesImage = NULL;
}

//=====================================================================================================	


template< class TInputImage, class TScalesImage >
void LSDerivativesL0< TInputImage, TScalesImage >
::SetScalesImage( InputScalesImageConstPointer image)
{
	m_scalesImage = image;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
void LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::SetScalesImage( InputScalesImageConstPointer image)
{
	m_scalesImage = image;
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
void LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::SetScalesImage( InputScalesImageConstPointer image)
{
	m_scalesImage = image;
}

//=====================================================================================================	


template< class TInputImage, class TScalesImage >
void LSDerivativesL0< TInputImage, TScalesImage >
::ComputeFilters()
{
        const double stepSize = vnl_math_max( 1e-10,( this->m_MaximumLevel - this->m_MinimumLevel ) / ( this->m_NumberOfLevels - 1 ) );//double check this!!!
        double sigmaValue = this->m_MinimumLevel;
        std::cout<<"Creating filter coefficients"<<std::endl;
	for( unsigned int level = 0; level < this->m_NumberOfLevels; level++ )
	{
      		sigmaValue = this->m_MinimumLevel + stepSize * level;
		//--------------------------------------------------------------------------------------------------------------------
		float* lpf = new float[2*m_Radius+1];
		float* hpf = new float[2*m_Radius+1];
		for( int k=-((int)m_Radius); k<=((int)m_Radius); ++k ){
			lpf[k+m_Radius] = 1.0f;
			hpf[k+m_Radius] = (float)(k/(sigmaValue*sigmaValue));
		}
		float* weight = new float[m_Radius];
		float  wsum   = itk::NumericTraits<float>::Zero;
		for( int k=0; k<((int)m_Radius); ++k ){
			weight[k]  = ::exp( -((float)(m_Radius-k)*(float)(m_Radius-k))/(2.0f*sigmaValue*sigmaValue) );
			wsum      += 2.0f*weight[k];
		}
		wsum += weight[m_Radius-1];
		wsum  = 1.0f/wsum;
		for( int k=0; k<((int)m_Radius); ++k ){
			lpf[k]            *= ( weight[k] * wsum );
			lpf[2*m_Radius-k] *= ( weight[k] * wsum );
			hpf[k]            *= ( weight[k] * wsum );
			hpf[2*m_Radius-k] *= ( weight[k] * wsum );
		}

		lpf[m_Radius] *= ( weight[m_Radius-1] * wsum );
		hpf[m_Radius] *= ( weight[m_Radius-1] * wsum );

		delete[] weight;
				
		lpfValuesMap.insert(TFilterValuesPair(sigmaValue,(lpf)));
		hpfValuesMap.insert(TFilterValuesPair(sigmaValue,(hpf)));

		//delete[] lpf;
		//delete[] hpf;	
	}
	std::cout<<"End of coeff computation"<<std::endl;			
}


//=====================================================================================================
template< class TInputImage, class TScalesImage >
void LSDerivativesL0< TInputImage, TScalesImage >
::BeforeThreadedGenerateData( void )
{
	if( m_Coordinate>=TInputImage::ImageDimension )
		itkExceptionMacro( << "Filtering direction exceeds image dimensions" );
	this->ComputeFilters();
}


template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
void LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::BeforeThreadedGenerateData( void )
{
	if( m_Coordinate>=ImageDimension )
		itkExceptionMacro( << "Filtering direction exceeds image dimensions" );
}

template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
void LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::BeforeThreadedGenerateData( void )
{
	if( m_Coordinate>=ImageDimension )
		itkExceptionMacro( << "Filtering direction exceeds image dimensions" );
}
//=====================================================================================================



	

//=====================================================================================================	
template< class TInputImage, class TScalesImage >
void LSDerivativesL0< TInputImage, TScalesImage>
::GenerateInputRequestedRegion()
{
	// Call the superclass' implementation of this method
	Superclass::GenerateInputRequestedRegion();
		
	// Get pointers to the input and output
	InputImagePointer  inputPtr  = const_cast< TInputImage * >( this->GetInput() );
	OutputImagePointer outputPtr = this->GetOutput();
		
	if ( !inputPtr || !outputPtr ){return;}
		
	InputSizeType size;
	size.Fill( 0 );
	if( m_Coordinate<TInputImage::ImageDimension )
		size[m_Coordinate] = m_Radius;
		
	// Get a copy of the input requested region (should equal the output
	// requested region)
	InputRegionType inputRequestedRegion = inputPtr->GetRequestedRegion();
	inputRequestedRegion.PadByRadius( size );
		
	// Crop the input requested region at the input's largest possible region
	inputRequestedRegion.Crop( inputPtr->GetLargestPossibleRegion() );
	inputPtr->SetRequestedRegion( inputRequestedRegion );
	return;
}

template< class TInputImage,class TScalesImage,unsigned int ImageDimension >
void LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::GenerateInputRequestedRegion()
{
	// Call the superclass' implementation of this method
	Superclass::GenerateInputRequestedRegion();
		
	// Get pointers to the input and output
	InputImagePointer  inputPtr  = const_cast< InputImageType* >( this->GetInput() );
	OutputImagePointer outputPtr = this->GetOutput();
		
	if ( !inputPtr || !outputPtr ){return;}
		
	InputSizeType size;
	size.Fill( 0 );
	if( m_Coordinate < ImageDimension )
		size[m_Coordinate] = m_Radius;
		
	// Get a copy of the input requested region (should equal the output
	// requested region)
	InputRegionType inputRequestedRegion = inputPtr->GetRequestedRegion();
	inputRequestedRegion.PadByRadius( size );
		
	// Crop the input requested region at the input's largest possible region
	inputRequestedRegion.Crop( inputPtr->GetLargestPossibleRegion() );
	inputPtr->SetRequestedRegion( inputRequestedRegion );
	return;
}

template< class TInputImage,class TScalesImage, unsigned int ImageDimension >
void LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::GenerateInputRequestedRegion()
{
	// Call the superclass' implementation of this method
	Superclass::GenerateInputRequestedRegion();
		
	// Get pointers to the input and output
	InputImagePointer  inputPtr  = const_cast< InputImageType* >( this->GetInput() );
	OutputImagePointer outputPtr = this->GetOutput();
		
	if ( !inputPtr || !outputPtr ){return;}
		
	InputSizeType size;
	size.Fill( 0 );
	if( m_Coordinate<ImageDimension )
		size[m_Coordinate] = m_Radius;
		
	// Get a copy of the input requested region (should equal the output
	// requested region)
	InputRegionType inputRequestedRegion = inputPtr->GetRequestedRegion();
	inputRequestedRegion.PadByRadius( size );
		
	// Crop the input requested region at the input's largest possible region
	inputRequestedRegion.Crop( inputPtr->GetLargestPossibleRegion() );
	inputPtr->SetRequestedRegion( inputRequestedRegion );
	return;
}
//=====================================================================================================
	
	


//=====================================================================================================
//=====================================================================================================
//=====================================================================================================
//=====================================================================================================
//=====================================================================================================
//=====================================================================================================	

template< class TInputImage, class TScalesImage>
#if ITK_VERSION_MAJOR < 4
void LSDerivativesL0< TInputImage, TScalesImage>
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, int threadId )
#else
void LSDerivativesL0< TInputImage, TScalesImage>
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, ThreadIdType threadId )
#endif
{
	// Boundary conditions for this filter; Neumann conditions are fine
	ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;	
	// Iterators:
	ConstNeighborhoodIterator<InputImageType>        bit;  		// Iterator for the input image
	ImageRegionIterator<OutputImageType>             it;   		// Iterator for the output image

	ImageRegionConstIterator<InputScalesImageType>		 scalesIt; 	// Iterator for the scalesImage 

	// Input and output
	InputImageConstPointer   input   =  this->GetInput();
	OutputImagePointer       output  =  this->GetOutput();
	//--------------------------------------------------------------------------------------------------------------------
	// Auxiliar values to store the filtered values:
	float           ip;
	OutputPixelType op;
	//--------------------------------------------------------------------------------------------------------------------
	InputSizeType size;
	size.Fill( 0 );
	size[m_Coordinate] = m_Radius;
	//--------------------------------------------------------------------------------------------------------------------
	// Find the data-set boundary "faces"
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType           faceList;
	NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                                  bC;
	
	faceList = bC( input, outputRegionForThread, size );
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;
	
	for ( fit=faceList.begin(); fit!=faceList.end(); ++fit ){ // Iterate through facets
		bit = ConstNeighborhoodIterator<InputImageType>( size, input,       *fit );
		it  = ImageRegionIterator<OutputImageType>(      output,            *fit );
		
		scalesIt = ImageRegionConstIterator<InputScalesImageType>(  m_scalesImage, *fit );

		// Boundary condition:
		bit.OverrideBoundaryCondition(&nbc);
		for( bit.GoToBegin(),it.GoToBegin(), scalesIt.GoToBegin(); !bit.IsAtEnd(); ++bit,++it,++scalesIt ){   // Iterate through pixels in the current facet
			// Auxiliar value to store filtered values:
			op.L = itk::NumericTraits<float>::Zero;
			op.H = itk::NumericTraits<float>::Zero;

			double level = scalesIt.Get();
                  
			for( unsigned int k=0; k<2*m_Radius+1; ++k ){
				ip     = (float)( bit.GetPixel(k) );
				op.L  += ip * lpfValuesMap[level][k];
				op.H  += ip * hpfValuesMap[level][k];
			}
			//-------------------------------------------------------------------------------------------------------------
			// Set the output pixel
			it.Set( op );
		}
	}
	//delete[] lpf;
	//delete[] hpf;
}
//=====================================================================================================
//=====================================================================================================
//=====================================================================================================
template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
#if ITK_VERSION_MAJOR < 4
void LSDerivativesL1< TInputImage, TScalesImage, ImageDimension >
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, int threadId )
#else
void LSDerivativesL1< TInputImage,TScalesImage,ImageDimension >
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, ThreadIdType threadId )
#endif
{
	// Boundary conditions for this filter; Neumann conditions are fine
	ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;	
	// Iterators:
	ConstNeighborhoodIterator<InputImageType>        bit;  // Iterator for the input image
	ImageRegionIterator<OutputImageType>             it;   // Iterator for the output image

	ImageRegionConstIterator<InputScalesImageType>		 scalesIt; 	// Iterator for the scalesImage 

	// Input and output
	InputImageConstPointer   input   =  this->GetInput();
	OutputImagePointer       output  =  this->GetOutput();
	//--------------------------------------------------------------------------------------------------------------------
	// Auxiliar values to store the filtered values:
	InputPixelType  ip;
	OutputPixelType op;
	//--------------------------------------------------------------------------------------------------------------------
	InputSizeType size;
	size.Fill( 0 );
	size[m_Coordinate] = m_Radius;
	
	//--------------------------------------------------------------------------------------------------------------------
	// Find the data-set boundary "faces"
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType           faceList;
	NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                                  bC;
	
	faceList = bC( input, outputRegionForThread, size );
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;

	
	for ( fit=faceList.begin(); fit!=faceList.end(); ++fit ){ // Iterate through facets
		bit = ConstNeighborhoodIterator<InputImageType>(     size, input, *fit  );
		it  = ImageRegionIterator<OutputImageType>(          output,      *fit  );

		scalesIt = ImageRegionConstIterator<InputScalesImageType>(  m_scalesImage, *fit );

		// Boundary condition:
		bit.OverrideBoundaryCondition(&nbc);
		for( bit.GoToBegin(),it.GoToBegin(), scalesIt.GoToBegin(); !bit.IsAtEnd(); ++bit,++it,++scalesIt ){   // Iterate through pixels in the current facet
			// Auxiliar value to store filtered values:
			op.LL = itk::NumericTraits<float>::Zero;
			op.HL = itk::NumericTraits<float>::Zero;
			op.LH = itk::NumericTraits<float>::Zero;

			double level = scalesIt.Get();

			for( unsigned int k=0; k<2*m_Radius+1; ++k ){
				ip     = bit.GetPixel(k);
				op.LL += ( ip.L ) * lpfValuesMap[level][k];
				op.HL += ( ip.H ) * lpfValuesMap[level][k];
				op.LH += ( ip.L ) * hpfValuesMap[level][k];
			}
			//-------------------------------------------------------------------------------------------------------------
			// Set the output pixel
			it.Set( op );
		}
	}
	//delete[] lpf;
	//delete[] hpf;
}
//=====================================================================================================
//=====================================================================================================
//=====================================================================================================	
template< class TInputImage, class TScalesImage, unsigned int ImageDimension >
#if ITK_VERSION_MAJOR < 4
void LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, int threadId )
#else
void LSDerivativesL2< TInputImage, TScalesImage, ImageDimension >
::ThreadedGenerateData( const OutputRegionType& outputRegionForThread, ThreadIdType threadId )
#endif
{
	// Boundary conditions for this filter; Neumann conditions are fine
	ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;	
	// Iterators:
	ConstNeighborhoodIterator<InputImageType>        bit;  // Iterator for the input image
	ImageRegionIterator<OutputImageType>             it;   // Iterator for the output image

	ImageRegionConstIterator<InputScalesImageType>		 scalesIt; 	// Iterator for the scalesImage 

	// Input and output
	InputImageConstPointer   input   =  this->GetInput();
	OutputImagePointer       output  =  this->GetOutput();
	//--------------------------------------------------------------------------------------------------------------------
	// Auxiliar values to store the filtered values:
	InputPixelType  ip;
	OutputPixelType op;
	//--------------------------------------------------------------------------------------------------------------------
	InputSizeType size;
	size.Fill( 0 );
	size[m_Coordinate] = m_Radius;
	//--------------------------------------------------------------------------------------------------------------------
	// Find the data-set boundary "faces"
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType           faceList;
	NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                                  bC;
	
	faceList = bC( input, outputRegionForThread, size );
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;
	
	for ( fit=faceList.begin(); fit!=faceList.end(); ++fit ){ // Iterate through facets
		bit = ConstNeighborhoodIterator<InputImageType>(     size, input, *fit  );
		it  = ImageRegionIterator<OutputImageType>(          output,      *fit  );

		scalesIt = ImageRegionConstIterator<InputScalesImageType>(  m_scalesImage, *fit );

		// Boundary condition:
		bit.OverrideBoundaryCondition(&nbc);
		for( bit.GoToBegin(),it.GoToBegin(), scalesIt.GoToBegin(); !bit.IsAtEnd(); ++bit,++it,++scalesIt ){   // Iterate through pixels in the current facet
			// Auxiliar value to store filtered values:
			op.LLL = itk::NumericTraits<float>::Zero;
			op.HLL = itk::NumericTraits<float>::Zero;
			op.LHL = itk::NumericTraits<float>::Zero;
			op.LLH = itk::NumericTraits<float>::Zero;

			double level = scalesIt.Get();

			for( unsigned int k=0; k<2*m_Radius+1; ++k ){
				ip      = bit.GetPixel(k);
				op.LLL += ( ip.LL ) * lpfValuesMap[level][k];
				op.HLL += ( ip.HL ) * lpfValuesMap[level][k];
				op.LHL += ( ip.LH ) * lpfValuesMap[level][k];;
				op.LLH += ( ip.LL ) * hpfValuesMap[level][k];;
			}
			//-------------------------------------------------------------------------------------------------------------
			// Set the output pixel
			it.Set( op );
		}
	}
	//delete[] lpf;
	//delete[] hpf;
}
	

	
} // end namespace itk


#endif
