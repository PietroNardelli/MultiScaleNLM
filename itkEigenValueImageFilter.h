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
#ifndef __itkEigenValueImageFilter_h
#define __itkEigenValueImageFilter_h

#include "itkUnaryFunctorImageFilter2.h"
#include "itkEigenValueFunctor.h"


namespace itk
{
/** \class EigenValueImageFilter
 * \brief Computes the Hessian eigenvalues.
 *
 * \sa UnaryFunctorImageFilter
 * \ingroup IntensityImageFilters Multithreaded
 */

template< class TInputImage, class TOutputImage >
class ITK_EXPORT EigenValueImageFilter :
  public UnaryFunctorImageFilter2< TInputImage, TOutputImage >
{
public:
  /** Standard class typedefs. */
  typedef EigenValueImageFilter       Self;
  typedef UnaryFunctorImageFilter2<
    TInputImage, TOutputImage >             Superclass;
  typedef SmartPointer<Self>                Pointer;
  typedef SmartPointer<const Self>          ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Runtime information support. */
  itkTypeMacro( EigenValueImageFilter, UnaryFunctorImageFilter2 );

  /** Some typedefs. */
  typedef typename Superclass::InputImageType         InputImageType;
  typedef typename Superclass::InputImagePointer      InputImagePointer;
  typedef typename Superclass::InputImageRegionType   InputImageRegionType;
  typedef typename Superclass::InputImagePixelType    InputImagePixelType;
  typedef typename Superclass::OutputImageType        OutputImageType;
  typedef typename Superclass::OutputImagePointer     OutputImagePointer;
  typedef typename Superclass::OutputImageRegionType  OutputImageRegionType;
  typedef typename Superclass::OutputImagePixelType   OutputImagePixelType;
  typedef EigenValueFunctor<
    InputImagePixelType, OutputImagePixelType >       ThisFunctorType;

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  typedef typename TInputImage::PixelType InputPixelType;
  itkConceptMacro(BracketOperatorsCheck,
    (Concept::BracketOperator< InputPixelType, unsigned int, double >));
  itkConceptMacro(DoubleConvertibleToOutputCheck,
    (Concept::Convertible<double, typename TOutputImage::PixelType>));
  /** End concept checking */
#endif

protected:
  EigenValueImageFilter()
  {
    /** Create and set this functor. */
    typename ThisFunctorType::Pointer thisFunctor
      = ThisFunctorType::New();
    this->SetFunctor( thisFunctor );
  }
  virtual ~EigenValueImageFilter() {}

private:
  EigenValueImageFilter(const Self&); // purposely not implemented
  void operator=(const Self&);              // purposely not implemented

}; // end class EigenValueImageFilter

} // end namespace itk

#endif
