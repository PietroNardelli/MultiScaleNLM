/*=========================================================================
*
* Copyright Marius Staring, Stefan Klein, David Doria. 2011.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0.txt
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*=========================================================================*/
#ifndef __itkGenerateScalesOutput_txx
#define __itkGenerateScalesOutput_txx

#include "itkGenerateScalesOutput.h"

// ITK include files
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

namespace itk
{


/**
 * ********************* Constructor ****************************
 */

template< typename TInputImage, typename TOutputImage >
GenerateScalesOutput< TInputImage, TOutputImage >
::GenerateScalesOutput()
{
  this->m_NonNegativeHessianBasedMeasure = true;

  this->m_SigmaMinimum = 1.0;
  this->m_SigmaMaximum = 4.0;
  this->m_NumberOfSigmaSteps = 4;

  this->m_SigmaStepMethod = Self::LogarithmicSigmaSteps;
  this->m_Rescale = true;

  typename ScalesImageType::Pointer scalesImage = ScalesImageType::New();
  this->ProcessObject::SetNumberOfRequiredOutputs( 2 );
  this->ProcessObject::SetNthOutput( 1, scalesImage.GetPointer() );

  // Construct ComputeFeaturesFilter
  this->m_ComputeFeaturesFilter = SingleScaleFilterType::New();

} // end Constructor


/**
 * ********************* SetUnaryFunctor ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::SetUnaryFunctor( UnaryFunctorBaseType * _arg )
{
  if ( this->m_ComputeFeaturesFilter->GetUnaryFunctor() != _arg )
  {
    this->m_ComputeFeaturesFilter->SetUnaryFunctor( _arg );
    this->Modified();
  }
} // end SetUnaryFunctor()

/**
 * ********************* SetNormalizeAcrossScale ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::SetNormalizeAcrossScale( bool normalize )
{
  if ( this->m_ComputeFeaturesFilter->GetNormalizeAcrossScale() != normalize )
  {
    this->m_ComputeFeaturesFilter->SetNormalizeAcrossScale( normalize );
    this->Modified();
  }
} // end SetNormalizeAcrossScale()


/**
 * ********************* MakeOutput ****************************
 */

template< typename TInputImage, typename TOutputImage >
typename GenerateScalesOutput< TInputImage, TOutputImage >::DataObjectPointer
GenerateScalesOutput< TInputImage, TOutputImage >
::MakeOutput( unsigned int idx )
{
  if ( idx == 1 )
  {
    return static_cast<DataObject*>( ScalesImageType::New().GetPointer() );
  }

  return Superclass::MakeOutput( idx );
} // end MakeOutput()


/**
 * ********************* EnlargeOutputRequestedRegion ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::EnlargeOutputRequestedRegion( DataObject *output )
{
  // currently this filter can not stream so we must set the outputs
  // requested region to the largest
  output->SetRequestedRegionToLargestPossibleRegion();

} // end EnlargeOutputRequestedRegion()


/**
 * ********************* SetNumberOfThreads ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::SetNumberOfThreads( ThreadIdType nt )
{
  Superclass::SetNumberOfThreads( nt );
  this->m_ComputeFeaturesFilter->SetNumberOfThreads( nt );
  this->Modified();

} // end SetNumberOfThreads()


/**
 * ********************* GenerateData ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::GenerateData( void )
{
  // TODO: Move the allocation to a derived AllocateOutputs method
  // Allocate the output
  this->GetOutput()->SetBufferedRegion( this->GetInput()->GetBufferedRegion() );
  this->GetOutput()->Allocate();
  if ( this->m_NonNegativeHessianBasedMeasure )
  {
    this->GetOutput()->FillBuffer( itk::NumericTraits<OutputPixelType>::Zero );
  }
  else
  {
    this->GetOutput()->FillBuffer( itk::NumericTraits<OutputPixelType>::NonpositiveMin() );
  }

  /*typename ScalesImageType::Pointer scalesImage = ScalesImageType::New();//dynamic_cast<ScalesImageType*>( this->ProcessObject::GetOutput( 1 ) );
  scalesImage->SetBufferedRegion( scalesImage->GetRequestedRegion() );
  scalesImage->Allocate();
  scalesImage->FillBuffer( itk::NumericTraits<ScalesPixelType>::Zero );*/

  // Check stuff here before starting
  if ( this->m_SigmaMinimum > this->m_SigmaMaximum )
  {
    itkExceptionMacro( << "ERROR: SigmaMinimum: " << this->m_SigmaMinimum
      << " cannot be greater than SigmaMaximum: " << this->m_SigmaMaximum );
  }

  typename InputImageType::ConstPointer input = this->GetInput();

  // Set filter input
  this->m_ComputeFeaturesFilter->SetInput( input );
  this->m_ComputeFeaturesFilter->SetRescale( this->m_Rescale );

  unsigned int scaleLevel = 0;
  while ( scaleLevel < this->m_NumberOfSigmaSteps )
  {
    // Determine sigma for this level
    double sigma = this->ComputeSigmaValue( scaleLevel );

    // Compute vesselness for this level.
    this->m_ComputeFeaturesFilter->SetSigma( sigma );
    this->m_ComputeFeaturesFilter->Update();

     // Get the maximum so far.
    this->UpdateMaximumResponse( this->m_ComputeFeaturesFilter->GetOutput(), scaleLevel );
    scaleLevel++;
  }

} // end GenerateData()


/**
 * ********************* UpdateMaximumResponse ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::UpdateMaximumResponse(
  const OutputImageType *seOutput,
  const unsigned int &scaleLevel )
{
    double sigma = this->ComputeSigmaValue( scaleLevel );

    OutputRegionType outputRegion = seOutput->GetBufferedRegion();
    typename ScalesImageType::Pointer scalesImage = ScalesImageType::New();
    scalesImage->SetBufferedRegion( this->GetOutput()->GetBufferedRegion() );
    scalesImage->Allocate();
    scalesImage->FillBuffer( itk::NumericTraits<ScalesPixelType>::Zero );
    ImageRegionIterator<ScalesImageType> scalesIter( scalesImage, outputRegion );
    ImageRegionConstIterator<OutputImageType> prevMaxResponseIter( this->GetOutput(), outputRegion );
    ImageRegionConstIterator<OutputImageType> currentResponseIter( seOutput, outputRegion );

    scalesIter.GoToBegin();
    prevMaxResponseIter.GoToBegin();
    currentResponseIter.GoToBegin();
    while ( !scalesIter.IsAtEnd() )
    {
      if ( prevMaxResponseIter.Value() < currentResponseIter.Value() )
      {
        scalesIter.Set( static_cast<ScalesPixelType>( sigma ) );
      }
      ++scalesIter; ++prevMaxResponseIter; ++currentResponseIter;
    }
    
    this->GraftOutput( scalesImage );

  // Generate the current maximum response.
  /*typedef MaximumImageFilter< OutputImageType,
    OutputImageType, OutputImageType> MaxFilterType;

  typename MaxFilterType::Pointer maxFilter = MaxFilterType::New();
  maxFilter->SetInput1( this->GetOutput() );
  maxFilter->SetInput2( seOutput );
  maxFilter->SetNumberOfThreads(this->GetNumberOfThreads());
  maxFilter->InPlaceOn();
  maxFilter->Update();
  this->GraftOutput( maxFilter->GetOutput() );*/
  

} // end GenerateScalesOutput()


/**
 * ********************* ComputeSigmaValue ****************************
 */

template< typename TInputImage, typename TOutputImage >
double
GenerateScalesOutput< TInputImage, TOutputImage >
::ComputeSigmaValue( const unsigned int & scaleLevel )
{
  double sigmaValue = this->m_SigmaMinimum;

  switch ( this->m_SigmaStepMethod )
  {
  case Self::EquispacedSigmaSteps:
    {
      const double stepSize = vnl_math_max( 1e-10,
        ( this->m_SigmaMaximum - this->m_SigmaMinimum ) / ( this->m_NumberOfSigmaSteps - 1 ) );
      sigmaValue = this->m_SigmaMinimum + stepSize * scaleLevel;
      break;
    }
  case Self::LogarithmicSigmaSteps:
    {
      const double stepSize = vnl_math_max( 1e-10,
        ( vcl_log( this->m_SigmaMaximum ) - vcl_log( this->m_SigmaMinimum ) )
        / ( this->m_NumberOfSigmaSteps - 1 ) );
      sigmaValue = vcl_exp( vcl_log ( this->m_SigmaMinimum ) + stepSize * scaleLevel );
      break;
    }
  default:
    itkExceptionMacro( << "ERROR: Invalid SigmaStepMethod" );
    break;
  }

  return sigmaValue;
} // end ComputeSigmaValue()


/**
 * ********************* SetSigmaStepMethodToEquispaced ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::SetSigmaStepMethodToEquispaced( void )
{
  this->SetSigmaStepMethod( Self::EquispacedSigmaSteps );
} // end SetSigmaStepMethodToEquispaced()


/**
 * ********************* SetSigmaStepMethodToLogarithmic ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::SetSigmaStepMethodToLogarithmic( void )
{
  this->SetSigmaStepMethod(Self::LogarithmicSigmaSteps);
} // end SetSigmaStepMethodToLogarithmic()


/**
 * ********************* GetScalesOutput ****************************


template< typename TInputImage, typename TOutputImage >
const typename GenerateScalesOutput<TInputImage, TOutputImage >::ScalesImageType *
GenerateScalesOutput< TInputImage, TOutputImage >
::GetScalesOutput( void ) const
{
  return static_cast<const ScalesImageType*>(this->ProcessObject::GetOutput(0));
} // end GetScalesOutput() */


/**
 * ********************* PrintSelf ****************************
 */

template< typename TInputImage, typename TOutputImage >
void
GenerateScalesOutput< TInputImage, TOutputImage >
::PrintSelf( std::ostream & os, Indent indent ) const
{
  Superclass::PrintSelf( os, indent );

  os << indent << "SigmaMinimum:  " << this->m_SigmaMinimum << std::endl;
  os << indent << "SigmaMaximum:  " << this->m_SigmaMaximum << std::endl;
  os << indent << "NumberOfSigmaSteps: " << this->m_NumberOfSigmaSteps << std::endl;
  os << indent << "SigmaStepMethod: " << this->m_SigmaStepMethod << std::endl;
  os << indent << "NonNegativeHessianBasedMeasure: "
    << this->m_NonNegativeHessianBasedMeasure << std::endl;
  //os << indent << "GenerateScalesOutput: " << this->m_GenerateScalesOutput << std::endl;
  os << indent << "Rescale: " << this->m_Rescale << std::endl;
  os << indent << "NormalizeAcrossScale: "
    << this->m_ComputeFeaturesFilter->GetNormalizeAcrossScale() << std::endl;

} // end PrintSelf()

} // end namespace itk

#endif
