/*=========================================================================
 
 Program:   Insight Segmentation & Registration Toolkit
 Module:    $RCSfile: itkNrrdVectorImageReadWriteTest.cxx,v $
 Language:  C++
 Date:      $Date: 2005/08/20 22:47:07 $
 Version:   $Revision: 1.1 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even 
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/
/*
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif
*/
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkNLMFilter.h"

int main( int ac, char* av[] )
{
	if(ac < 3)
    {
		std::cerr << "Usage: " << av[0] << " Input Output [sigma=0.0 radiusSearch=3 radiusComp=1 h=1.2 ]\n";
		return EXIT_FAILURE;
    }
	
	typedef float                                PixelType;
	typedef itk::Image<PixelType, 3>             ImageType;
	typedef itk::NLMFilter<ImageType,ImageType>  FilterType;
	
	itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
	reader->SetFileName(av[1]);
	
	
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
	
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	
	//==========================================================================
	// Set the parameters to the filter:
	float sigma = 0.0f;
	if( ac>3 )
		sigma = ::atof( av[3] );
	filter->SetSigma( sigma );
	
	unsigned int rads = 3;
	if( ac>4 )
		rads = ::atoi( av[4] );
	filter->SetRSearch( rads );
	
	unsigned int radc = 1;
	if( ac>5 )
		radc = ::atoi( av[5] );
	filter->SetRComp( radc );
	
	float hpar = 1.2;
	if( ac>6 )
		hpar = ::atof( av[6] );
	filter->SetH( hpar );
	//==========================================================================
	
	
	try
    {
		filter->Update();
    }
	catch ( itk::ExceptionObject & e )
    {
		std::cerr << "exception in UNLM filter" << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
		return EXIT_FAILURE;
    }

	// Generate test image
	itk::ImageFileWriter<ImageType>::Pointer writer = itk::ImageFileWriter<ImageType>::New();
	writer->SetInput( filter->GetOutput() );
	writer->SetFileName(av[2]);
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
