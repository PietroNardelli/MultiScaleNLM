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
#ifndef __itkGeneralGeneralEigenValueFunctor_h
#define __itkGeneralEigenValueFunctor_h

#include "itkUnaryFunctorBase.h"
#include "itkComparisonOperators.h"
#include "vnl/vnl_math.h"

namespace itk
{
namespace Functor
{

/** \class GeneralEigenValueFunctor
 * \brief Computes a measure the Hessian eigenvalues.
 *
 *
 * \sa FrangiVesselnessImageFilter
 * \ingroup IntensityImageFilters Multithreaded
 */

template< class TInput, class TOutput >
class GeneralEigenValueFunctor
  : public UnaryFunctorBase< TInput, TOutput >
{
public:
  /** Standard class typedefs. */
  typedef GeneralEigenValueFunctor            Self;
  typedef UnaryFunctorBase< TInput, TOutput > Superclass;
  typedef SmartPointer< Self >                Pointer;
  typedef SmartPointer< const Self >          ConstPointer;

  /** New macro for creation of through a smart pointer. */
  itkNewMacro( Self );

  /** Run-time type information (and related methods). */
  itkTypeMacro( GeneralEigenValueFunctor, UnaryFunctorBase );

  /** Typedef's. */
  typedef typename NumericTraits<TOutput>::RealType RealType;
  typedef TInput                                    EigenValueArrayType;
  typedef typename EigenValueArrayType::ValueType   EigenValueType;

  itkSetMacro( Threshold, double );
  itkGetMacro( Threshold, double );

  /** This does the real computation */
  virtual TOutput Evaluate( const TInput & eigenValues ) const
  {
    /** Sort the eigenvalues by their absolute value, such that |l1| < |l2| < |l3|. */
    EigenValueArrayType sortedEigenValues = eigenValues;
    std::sort( sortedEigenValues.Begin(), sortedEigenValues.End(),
    Functor::AbsLessCompare<EigenValueType>() );

    /** Take the absolute values and abbreviate. */
    const RealType l1 = vnl_math_abs( sortedEigenValues[ 0 ] );
    const RealType l2 = vnl_math_abs( sortedEigenValues[ 1 ] );
    const RealType l3 = vnl_math_abs( sortedEigenValues[ 2 ] );

    /** Check Frobenius Norm. This relates to the background noise*/

    const RealType S  = vcl_sqrt( l1 * l1 + l2 * l2 + l3 * l3 );
    /*if( this->m_Sigma == 2.5 ){
	std::cout<<"S: "<<S<<" "<<"Threshold: "<<m_Threshold<<" "<<"Sigma: "<<this->m_Sigma<<std::endl;
    	std::cout<<"New Threshold: "<< (m_Threshold * 1.0 / ( 1 + this->m_Sigma ))<<std::endl;
    }*/

    /*if( S < ( m_Threshold * 1.0 / ( 1 + this->m_Sigma ) ) )
    {
	return static_cast<TOutput>( exp( - ( S / ( m_Threshold * 1.0 / ( 1 + this->m_Sigma ) ) ) ) );
	//return static_cast<TOutput>(-1);
    }*/

    const RealType alpha = 1.0 - vcl_exp(- S / ( m_Threshold / ( 1.0 + this->m_Sigma ) ) );

    /** Avoid divisions by zero (or close to zero). */
    // if( l2 < 0.01 || l3 < 0.01 )    
    RealType S1, S2, S3;
    if( l2 < vnl_math::eps || l3 < vnl_math::eps )
    {
      	S1 = NumericTraits<TOutput>::Zero;
    }
    else
    {
    	S1 = alpha * l1 / vcl_sqrt( l2 * l3 ) ; // blob-like
    }
    if( l3 < vnl_math::eps )
    {
      	S2 = NumericTraits<TOutput>::Zero;
      	S3 = NumericTraits<TOutput>::Zero;
    }
    else
    {
    	S2 = alpha * (1.0 - ( l2 / l3 ) ); // plate-like
    	S3 = alpha * (l2 / l3 ); // line-like
    }

    if( S1 >= S2 && S1 >= S3 )
    	return static_cast<TOutput>( S1 );
    else if( S2 >= S1 && S2 >= S3 )
    	return static_cast<TOutput>( S2 );	
    else if( S3 >= S1 && S3 >= S2 )
    	return static_cast<TOutput>( S3 );

  } // end operator ()

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro( DimensionIs3Check,
    ( Concept::SameDimension< EigenValueArrayType::Dimension, 3 > ) );
  /** End concept checking */
#endif

protected:
  /** Constructor */
  GeneralEigenValueFunctor(){};
  virtual ~GeneralEigenValueFunctor(){};

private:
  GeneralEigenValueFunctor(const Self &); // purposely not implemented
  void operator=(const Self &);    // purposely not implemented
  double m_Threshold;
}; // end class GeneralEigenValueFunctor

} // end namespace itk::Functor
} // end namespace itk

#endif
