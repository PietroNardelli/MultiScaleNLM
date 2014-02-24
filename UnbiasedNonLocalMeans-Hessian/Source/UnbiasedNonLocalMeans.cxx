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

#include "itkNoiseImageFilter.h"
#include "itkScalarImageToHistogramGenerator.h"
#include "itkMinimumMaximumImageCalculator.h"
 
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
	typedef itk::Image<double,DIMENSION> ScalesImageType;
	typedef itk::Image<double,DIMENSION> StrengthImageType;
	typedef itk::VectorImage< double, DIMENSION > FeaturesImageType;

	
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
	
	
	typedef itk::NLMFilter<ImageType,ImageType> FilterType;
	typename FilterType::Pointer filter = FilterType::New();
	filter->SetInput( reader->GetOutput() );
	
	/** SET PARAMETERS TO THE FILTER */
	// The power of noise:

	/*typedef itk::NoiseImageFilter< ImageType, ImageType > NoiseImageFilterType;
	typename NoiseImageFilterType::Pointer noiseImageFilter = NoiseImageFilterType::New();

	noiseImageFilter->SetInput(reader->GetOutput());    
	noiseImageFilter->SetRadius(2);
	noiseImageFilter->Update();

	typedef itk::MinimumMaximumImageCalculator< ImageType >  ImageCalculatorFilterType;
	typename ImageCalculatorFilterType::Pointer imageCalculatorFilter = ImageCalculatorFilterType::New();

	imageCalculatorFilter->SetImage(noiseImageFilter->GetOutput());
	imageCalculatorFilter->Compute();

	typedef itk::Statistics::ScalarImageToHistogramGenerator<ImageType> HistogramFilterType;
	typename HistogramFilterType::Pointer histogramFilter = HistogramFilterType::New();

	histogramFilter->SetInput( noiseImageFilter->GetOutput());	  
	histogramFilter->SetNumberOfBins( imageCalculatorFilter->GetMaximum() );
	histogramFilter->SetMarginalScale( 1.0 );
	histogramFilter->Compute();

	typename HistogramFilterType::HistogramType::ConstIterator itr=histogramFilter->GetOutput()->Begin();
	typename HistogramFilterType::HistogramType::ConstIterator end=histogramFilter->GetOutput()->End();

	double maxFrequency = 0;
	double maxBin = 0;

	while( itr != end )
	  {
	    if ( itr.GetFrequency() > maxFrequency )
	      {
		maxFrequency = itr.GetFrequency();
		maxBin = floor(histogramFilter->GetOutput()->GetBinMin(0,itr.GetInstanceIdentifier()) + 0.5);
	      }
		++itr;
	  }
  
	//std::cout<< " Mode: " << maxBin << std::endl; 

	filter->SetSigma( maxBin );*/
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
        filter->SetMinimumLevel( iMinScale );
	filter->SetMaximumLevel( iMaxScale );
        filter->SetNumberOfLevelSteps( iNScales );
	filter->SetOrder( iOrder );
	filter->SetAlpha( iAlpha );
	filter->SetUseEstimatedDistanceMean(1); //fix this!
	filter->SetNormalizedByFeatureStrength(0); //fix this!
	filter->SetUseDeltaFeatureStrength(0); //fix this!

	if(scalesImageVolume.c_str() != std::string("None"))
	{
		filter->SetGenerateScalesOutput( 1 );
	}

	if(strengthImageVolume.c_str() != std::string("None"))
	{
		filter->SetGenerateStrengthOutput( 1 );
	}
	
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

	// Generate features image
	typename itk::ImageFileWriter<FeaturesImageType>::Pointer featuresWriter = itk::ImageFileWriter<FeaturesImageType>::New();
	featuresWriter->SetInput( filter->GetFeaturesOutput() );
	featuresWriter->SetFileName( featuresVolume.c_str() );
	try
    {
		featuresWriter->Update();
    }
	catch ( itk::ExceptionObject & e )
    {
		std::cerr << "exception in features writer " << std::endl;
		std::cerr << e.GetDescription() << std::endl;
		std::cerr << e.GetLocation() << std::endl;
		return EXIT_FAILURE;
    }
	if( scalesImageVolume.c_str() != std::string("None") )
	{
    		// Generate scales image
    		typename itk::ImageFileWriter<ScalesImageType>::Pointer scalesWriter = itk::ImageFileWriter<ScalesImageType>::New();
    		scalesWriter->SetInput( filter->GetScalesOutput() );
		scalesWriter->SetFileName( scalesImageVolume.c_str() );
		try
    		{
			scalesWriter->Update();
    		}
		catch ( itk::ExceptionObject & e )
    		{
			std::cerr << "exception in scales writer " << std::endl;
			std::cerr << e.GetDescription() << std::endl;
			std::cerr << e.GetLocation() << std::endl;
			return EXIT_FAILURE;
    		}
	}

	if( strengthImageVolume.c_str() != std::string("None") )
	{
    		// Generate scales image
    		typename itk::ImageFileWriter<StrengthImageType>::Pointer strengthWriter = itk::ImageFileWriter<StrengthImageType>::New();
    		strengthWriter->SetInput( filter->GetFunctorOutput() );
		strengthWriter->SetFileName( strengthImageVolume.c_str() );
		try
    		{
			strengthWriter->Update();
    		}
		catch ( itk::ExceptionObject & e )
    		{
			std::cerr << "exception in strength writer " << std::endl;
			std::cerr << e.GetDescription() << std::endl;
			std::cerr << e.GetLocation() << std::endl;
			return EXIT_FAILURE;
    		}
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


