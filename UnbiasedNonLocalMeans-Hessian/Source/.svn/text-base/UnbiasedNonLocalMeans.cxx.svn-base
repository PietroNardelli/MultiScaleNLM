/*=========================================================================
 
 Program:   Diffusion Applications
 Language:  C++
 Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Applications/CLI/DiffusionApplications/dwiNoiseFilter/dwiNoiseFilter.cxx $
 Date:      $Date: 2008-11-25 18:46:58 +0100 (Tue, 25 Nov 2008) $
 Version:   $Revision: 7972 $
 
 Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.
 
 See License.txt or http://www.slicer.org/copyright/copyright.txt for details.
 
 ==========================================================================*/

#ifdef _WIN32
// to pick up M_SQRT2 and other nice things...
#define _USE_MATH_DEFINES
#endif
#include <math.h>



#ifdef THISISASLICERBUILD

#include "itkPluginUtilities.h"

#else

#ifdef SLICERV4
#include "itkPluginUtilities.h"
#else
#include "SlicerExecutionModel/itkPluginUtilities.h"
#endif

#endif

#include <itkImageFileWriter.h>
#include <itkImageFileReader.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionConstIteratorWithIndex.h>
#include "itkCastImageFilter.h"
#include "UnbiasedNonLocalMeansCLP.h"
//--------------------------------------------------
// Specific includes:
#include "itkNLMFilter.h"
//--------------------------------------------------
#define DIMENSION 3

template<class PixelType>
int DoIt( int argc, char * argv[], PixelType )
{
	PARSE_ARGS;
    
	// do the typedefs
	typedef itk::Image<PixelType,DIMENSION> ImageType;
	
	typename itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	reader->SetFileName( inputVolume.c_str() );
	
	
	try
    {
		reader->Update();
    }
	catch ( itk::ExceptionObject & e )
    {
		std::cerr << "exception in file reader " << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
		return EXIT_FAILURE;
    }
	
	
	typedef itk::NLMFilter< ImageType, ImageType > FilterType;
	typename FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	
	/** SET PARAMETERS TO THE FILTER */
	// The power of noise:
	filter->SetSigma( iSigma );
	// The search radius
	typename FilterType::InputImageSizeType radius;
	for( unsigned int d=0; d<DIMENSION; ++d )
		radius[d] = iRadiusSearch[d];
	filter->SetRSearch( radius );
	// The comparison radius:
	for( unsigned int d=0; d<DIMENSION; ++d )
		radius[d] = iRadiusComp[d];
	filter->SetRComp( radius );
	// The "h" parameter:
	filter->SetH( iH );
	// The preselection threshold:
	filter->SetPSTh( iPs );
	
	// Run the filter:
	try
    {
		filter->Update();
    }
	catch ( itk::ExceptionObject & e )
    {
		std::cerr << "exception in filter" << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
		return EXIT_FAILURE;
    }
	
	// Generate output image
	typename itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	writer->SetInput( filter->GetOutput() );
	writer->SetFileName( outputVolume.c_str() );
	try
    {
		writer->Update();
    }
	catch ( itk::ExceptionObject & e )
    {
		std::cerr << "exception in file writer " << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
		return EXIT_FAILURE;
    }
	return EXIT_SUCCESS;
}

int main( int argc, char * argv[] )
{
    PARSE_ARGS;
    
    itk::ImageIOBase::IOPixelType pixelType;
    itk::ImageIOBase::IOComponentType componentType;    
    itk::GetImageType (inputVolume, pixelType, componentType);
    
    // This filter handles all types
    
    switch (componentType)
    {
#ifndef WIN32
        case itk::ImageIOBase::UCHAR:
            return DoIt( argc, argv, static_cast<unsigned char>(0));
            break;
        case itk::ImageIOBase::CHAR:
            return DoIt( argc, argv, static_cast<char>(0));
            break;
#endif
        case itk::ImageIOBase::USHORT:
            return DoIt( argc, argv, static_cast<unsigned short>(0));
            break;
        case itk::ImageIOBase::SHORT:
            return DoIt( argc, argv, static_cast<short>(0));
            break;
        case itk::ImageIOBase::UINT:
            return DoIt( argc, argv, static_cast<unsigned int>(0));
            break;
        case itk::ImageIOBase::INT:
            return DoIt( argc, argv, static_cast<int>(0));
            break;
#ifndef WIN32
        case itk::ImageIOBase::ULONG:
            return DoIt( argc, argv, static_cast<unsigned long>(0));
            break;
        case itk::ImageIOBase::LONG:
            return DoIt( argc, argv, static_cast<long>(0));
            break;
#endif
        case itk::ImageIOBase::FLOAT:
            return DoIt( argc, argv, static_cast<float>(0));
            break;
        case itk::ImageIOBase::DOUBLE:
            std::cout << "DOUBLE type not currently supported." << std::endl;
            break;
        case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
        default:
            std::cout << "unknown component type" << std::endl;
            break;
    }
    
    return EXIT_SUCCESS;
}


