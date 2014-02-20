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
		std::cerr << "Usage: " << av[0] << " Input Output [sigma=5.0f Rsearch=5 Rcomp=2 h=1.2f ps=2.3f]" << std::endl;
		return EXIT_FAILURE;
    }
	
	typedef float                                 PixelType;
	typedef itk::Image<PixelType,3>               ImageType;
	
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
	
	
	typedef itk::NLMFilter< ImageType, ImageType > FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	
	float        sigma = 5.0f;
	unsigned int rs    = 5;
	unsigned int rc    = 2;
	float        h     = 1.2f;
	float        ps    = 2.3f;
	if( ac>3 )
		sigma = (float)::atof( av[3] );
	if( ac>4 )
		rs    = (unsigned int)::atoi( av[4] );
	if( ac>5 )
		rc    = (unsigned int)::atoi( av[5] );
	if( ac>6 )
		h     = (float)::atof( av[6] );
	if( ac>7 )
		ps    = (float)::atof( av[7] );
	FilterType::InputImageSizeType radius;
	filter->SetSigma( sigma );
	radius.Fill( rs );
	filter->SetRSearch( radius );
	radius.Fill( rc );
	filter->SetRComp( radius );
	filter->SetH( h );
	filter->SetPSTh( ps );
	std::cerr << "Executing filter: " << filter << std::endl;	
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
