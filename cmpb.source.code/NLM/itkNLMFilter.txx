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

#include "itkConstNeighborhoodIterator.h"
#include "itkImageRegionIterator.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "math.h"

namespace itk
{
template< class TInputImage, class TOutputImage >
NLMFilter< TInputImage, TOutputImage >
::NLMFilter()
{
	m_Sigma         = 1.0f;
	m_H             = 1.2f;
	m_RSearch       = 2;
	m_RComp         = 1;
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
	InputImageSizeType radius;
	radius.Fill( m_RSearch + m_RComp );
	inputRequestedRegion.PadByRadius( radius );
		
	// Crop the input requested region at the input's largest possible region
	inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion());
	inputPtr->SetRequestedRegion( inputRequestedRegion );
	return;
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
	// Boundary conditions for this filter; Neumann conditions are fine
	ZeroFluxNeumannBoundaryCondition<InputImageType> nbc;	
	// Iterators:
	ConstNeighborhoodIterator<InputImageType>        bit;  // Iterator for the input image
	ImageRegionIterator<OutputImageType>             it;   // Iterator for the output image
	// Input and output
	InputImageConstPointer   input   =  this->GetInput();
	OutputImagePointer       output  =  this->GetOutput();
	// Find the data-set boundary "faces"
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType           faceList;
	NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>                                  bC;
	
	InputImageSizeType radius;
	radius.Fill( m_RComp );
	faceList = bC( input, outputRegionForThread, radius );
	typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<InputImageType>::FaceListType::iterator fit;
	// Region to search and iterator:
	ConstNeighborhoodIterator<InputImageType> search;
	InputImageRegionType                      searchRegion;
	InputImageSizeType                        searchSize, baseSearchSize;
	baseSearchSize.Fill(2*m_RSearch+1);
		
	unsigned int numNeighbours    = 1;
	for( unsigned int d=0; d<TInputImage::ImageDimension; ++d ) // The number of voxels which are going to be accounted in the WA
		numNeighbours    *= baseSearchSize[d];
	// Generate the Gaussian window (std=1)
	// Auxiliar iterator:
	ConstNeighborhoodIterator<InputImageType> test = ConstNeighborhoodIterator<InputImageType>(  radius, input, outputRegionForThread  );
	unsigned int neighborhoodSize = test.Size();
	float* gw = new float[neighborhoodSize];        // The window itself
	float sum = itk::NumericTraits<float>::Zero;    // To normalize the window to sum to 1
	for( unsigned int k=0; k<neighborhoodSize; ++k ){
		if( k!=neighborhoodSize/2 ){  // Not the center of the neighbourhhod
			typename ConstNeighborhoodIterator<InputImageType>::OffsetType idx = test.GetOffset(k);
			gw[k] = itk::NumericTraits<float>::Zero;
			for( unsigned int d=0; d<InputImageType::ImageDimension; ++d )
				gw[k] += static_cast<float>( idx[d]*idx[d] );
			gw[k] = ::exp( -gw[k]/2 ); // sigma=1
		}
		else{  // In the center of the neighbourhood, we correct the weight to avoid over-weighting
			if( m_RComp>0 )
				gw[k] = gw[k-1]; // The previous value is the one corresponding to the closest pixel to the center
			else
				gw[k] = 1;
		}
		sum += gw[k];
	}
	// Normalize the Gaussian kernel:
	for( unsigned int k=0; k<neighborhoodSize; ++k )
		gw[k] /= sum;
	// Auxiliar variables:
	float  norm;
	float  sqh = 1.0f/(m_H*m_H*m_Sigma*m_Sigma);
	float* distB = new float[numNeighbours];
	float* valsB = new float[numNeighbours];
	
	for ( fit=faceList.begin(); fit != faceList.end(); ++fit){ // Iterate through facets
		// Iterators:
		radius.Fill( m_RComp );
		bit = ConstNeighborhoodIterator<InputImageType>(  radius, input, *fit  );
		it  = ImageRegionIterator<OutputImageType>(         output,    *fit          );
		// Boundary condition:
		bit.OverrideBoundaryCondition(&nbc);
		InputImageIndexType originR;
		InputImageSizeType  radiusR;
		radiusR.Fill( m_RSearch );
		for( bit.GoToBegin(),it.GoToBegin(); !bit.IsAtEnd(); ++bit,++it ){   // Iterate through pixels in the current facet
			//-------------------------------------------------------------------------------------------------------------
			// CREATE THE REGION TO SEARCH AND THE ITERATORS:
			searchSize = baseSearchSize;
			originR = bit.GetIndex() - radiusR;
			bool needToComputeCenter = false;
			for( unsigned int d=0; d<TInputImage::ImageDimension; ++d ){
				if( originR[d]<0 ){
					originR[d]=0;
					needToComputeCenter = true;
				}
				if( originR[d]+searchSize[d] >= input->GetLargestPossibleRegion().GetSize()[d] ){
					searchSize[d] = input->GetLargestPossibleRegion().GetSize()[d] - originR[d];
					needToComputeCenter = true;
				}
			}
			// ---------------------
			// Compute the index corresponding to the original center:
			unsigned int midPosition = numNeighbours/2;
			if( needToComputeCenter ){
				unsigned int aux = 1;
				for( unsigned int d=0; d<TInputImage::ImageDimension; ++d )
					aux *= searchSize[d];
				midPosition = 0;
				if( aux>0 ){
					for( int d=(int)TInputImage::ImageDimension-1; d>=0; --d ){
						aux /= searchSize[d];
						midPosition += ( bit.GetIndex()[d] - originR[d] )*aux;
					}
				}
			}
			// ---------------------
			searchRegion.SetIndex( originR );
			searchRegion.SetSize( searchSize );
			search = ConstNeighborhoodIterator<InputImageType>(  radius, input, searchRegion  );
			//-------------------------------------------------------------------------------------------------------------
			// FILTER THE PIXEL
			norm = itk::NumericTraits<float>::Zero;    // To normalize the weights to sum to 1
			unsigned int pos; // Auxiliar variable
			for( pos=0,search.GoToBegin(); !search.IsAtEnd(); ++search,++pos ){ // For each pixel in the search neighbourhood
				if( pos!=midPosition ){
					distB[pos] = itk::NumericTraits<float>::Zero;
					for( unsigned int k=0; k<neighborhoodSize; ++k ){ // For each pixel in the comparison neighbourhood
						float aux   = (float)bit.GetPixel(k) - (float)search.GetPixel(k);
						distB[pos] += gw[k] * aux * aux;
					}
					//distB[pos]   = ::exp( -distB[pos]*sqh );
					//----------------------------------------------------------------------
					// Alternative computation of the exponential
					float weight     = -distB[pos]*sqh;
					if( weight > -2.3f ){
						float temp       = 1.0f/( 1.0f - weight );
						distB[pos]       = temp*(0.5f*(2.0f+weight)) - temp*temp*(0.5f*weight);
					}
					else
						distB[pos]       = 0.0f;
					//----------------------------------------------------------------------
					norm            += distB[pos];
				}
				valsB[pos]   = (float)search.GetCenterPixel();
			}
			distB[midPosition] = 0.367879441171442f;
			norm               = 1.0f/(norm+0.367879441171442f);
			float value = itk::NumericTraits<float>::Zero;
			for( unsigned int k=0; k<pos; ++k )
				value += distB[k] * valsB[k] * valsB[k] * norm;
			value -= ( 2.0f * m_Sigma * m_Sigma );
			value  = ( value>0.0f ? ::sqrt(value) : 0.0f );
			// Set the output pixel
			it.Set( static_cast<OutputPixelType>(value) );
		}
	}
	// Delete previously allocated memory:
	delete[] gw;
	delete[] distB;
	delete[] valsB;
}
	

	
} // end namespace itk


#endif
