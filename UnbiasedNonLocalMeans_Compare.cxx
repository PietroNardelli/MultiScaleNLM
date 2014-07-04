#include "itkWin32Header.h"
#include <iostream>
#include <fstream>
#include "itkNumericTraits.h"
#include "itkImageFileReader.h"
#include "itkImageRegionConstIterator.h"

int RegressionTestImage (const char *, const char *, int, bool);

int main(int argc, char **argv)
{
   if(argc < 3){
      std::cerr << "Usage:" << std::endl;
      std::cerr << "testImage, compareImage" << std::endl;
      return -1;
   }
   
   typedef float PixelType;
   const unsigned int DIMENSION = 3;
   typedef itk::Image<PixelType,DIMENSION>          ImageType;
   typedef itk::ImageFileReader<ImageType>          ReaderType;
   typedef itk::ImageRegionConstIterator<ImageType> IteratorType;
   
   ReaderType::Pointer reader1 = ReaderType::New();
   ReaderType::Pointer reader2 = ReaderType::New();
   reader1->SetFileName( argv[1] );
   reader2->SetFileName( argv[2] );
   
   try{
      reader1->Update();
      reader2->Update();
   }
   catch(itk::ExceptionObject& e){
      std::cerr << "Error updating readers: " << e.GetDescription() << std::endl;
      return EXIT_FAILURE;
   }
   
   for( unsigned int k=0; k<DIMENSION; ++k ){
      if( reader1->GetOutput()->GetLargestPossibleRegion().GetSize()[k] != reader2->GetOutput()->GetLargestPossibleRegion().GetSize()[k] ){
         std::cerr << "Mismatch in dimension: " << k;
         return EXIT_FAILURE;
      }
   }
   
   IteratorType it1( reader1->GetOutput(), reader1->GetOutput()->GetLargestPossibleRegion() );
   IteratorType it2( reader2->GetOutput(), reader2->GetOutput()->GetLargestPossibleRegion() );
   
   double difference = itk::NumericTraits<double>::Zero;
   for( it1.GoToBegin(),it2.GoToBegin(); !it1.IsAtEnd(); ++it1,++it2 ){
      float temp = it1.Get() - it2.Get();
      difference += ( temp>itk::NumericTraits<float>::Zero ? temp : -temp );
   }
   
   if( difference>1e-9 ){
      std::cerr << "Error (" << difference << ") is above tolerance (1e-9)";
      return EXIT_FAILURE;
   }
   else{
      return EXIT_SUCCESS;
   }
}
