/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkNLMFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/27 17:01:10 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkNLMFilter_h
#define __itkNLMFilter_h

#include "itkImageToImageFilter.h"
#include "itkImage.h"

namespace itk
{

/** \class NLMFilter
 *
 * DO NOT assume a particular image or pixel type, which is, the input image
 * may be a VectorImage as well as an Image obeject with vectorial pixel type.
 *
 * \sa Image
 */
template <class TInputImage, class TOutputImage>
class ITK_EXPORT NLMFilter : public ImageToImageFilter< TInputImage, TOutputImage >
{
public:
	/** Standard class typedefs. */
	typedef NLMFilter                             Self;
	/** Convenient typedefs for simplifying declarations. */
	typedef TInputImage                           InputImageType;
	typedef typename InputImageType::Pointer      InputImagePointer;
	typedef typename InputImageType::ConstPointer InputImageConstPointer;
	typedef TOutputImage                          OutputImageType;
	typedef typename OutputImageType::Pointer     OutputImagePointer;
	
	/** Standard class typedefs. */
	typedef ImageToImageFilter< InputImageType, OutputImageType> Superclass;
	typedef SmartPointer<Self>                                   Pointer;
	typedef SmartPointer<const Self>                             ConstPointer;

	/** Method for creation through the object factory. */
	itkNewMacro(Self);

	/** Run-time type information (and related methods). */
	itkTypeMacro( NLMFilter, ImageToImageFilter );
  
	/** Image typedef support. */
	typedef typename InputImageType::PixelType           InputPixelType;
	typedef typename OutputImageType::PixelType          OutputPixelType;
	typedef typename InputImageType::RegionType          InputImageRegionType;
	typedef typename InputImageType::SizeType            InputImageSizeType;
	typedef typename InputImageType::IndexType           InputImageIndexType;
	typedef typename OutputImageType::RegionType         OutputImageRegionType;
	
	itkSetMacro( Sigma,      float        );
	itkGetMacro( Sigma,      float        );
	itkSetMacro( H,          float        );
	itkGetMacro( H,          float        );
	itkSetMacro( RSearch,    unsigned int );
	itkGetMacro( RSearch,    unsigned int );
	itkSetMacro( RComp,      unsigned int );
	itkGetMacro( RComp,      unsigned int );
	
protected:
	NLMFilter();
	virtual ~NLMFilter() {}
	// Threaded filter!
#if ITK_VERSION_MAJOR < 4
    void ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, int threadId );
    
#else
    void ThreadedGenerateData( const OutputImageRegionType & outputRegionForThread, ThreadIdType threadId );
    
#endif
	void GenerateInputRequestedRegion();
private:
	NLMFilter(const Self&);         // purposely not implemented
	void operator=(const Self&);    // purposely not implemented
	// The standard deviation of noise (in the complex domain)
	float                m_Sigma;
	// The true parameteres of NLM:
	float                m_H;
	unsigned int         m_RSearch;
	unsigned int         m_RComp;
};


 
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNLMFilter.txx"
#endif

#endif
