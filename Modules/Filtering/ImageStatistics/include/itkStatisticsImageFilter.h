/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef itkStatisticsImageFilter_h
#define itkStatisticsImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkNumericTraits.h"
#include "itkArray.h"
#include "itkSimpleDataObjectDecorator.h"
#include "itkSimpleFastMutexLock.h"
#include "itkCompensatedSummation.h"

namespace itk
{
/** \class StatisticsImageFilter
 * \brief Compute min, max, variance and mean of an Image.
 *
 * StatisticsImageFilter computes the minimum, maximum, sum, sum of squares, mean, variance
 * sigma of an image.  The filter needs all of its input image.  It
 * behaves as a filter with an input and output. Thus it can be inserted
 * in a pipline with other filters and the statistics will only be
 * recomputed if a downstream filter changes.
 *
 * The filter passes its input through unmodified.  The filter is
 * threaded. It computes statistics in each thread then combines them in
 * its AfterThreadedGenerate method.
 *
 * Internally a compensated summation algorithm is used for the
 * accumulation of intensities to improve accuracy for large images.
 *
 * \ingroup MathematicalStatisticsImageFilters
 * \ingroup ITKImageStatistics
 *
 * \wiki
 * \wikiexample{Statistics/StatisticsImageFilter,Compute min\, max\, variance and mean of an Image.}
 * \endwiki
 */
template< typename TInputImage >
class ITK_TEMPLATE_EXPORT StatisticsImageFilter:
  public ImageToImageFilter< TInputImage, TInputImage >
{
public:
  ITK_DISALLOW_COPY_AND_ASSIGN(StatisticsImageFilter);

  /** Standard Self type alias */
  using Self = StatisticsImageFilter;
  using Superclass = ImageToImageFilter< TInputImage, TInputImage >;
  using Pointer = SmartPointer< Self >;
  using ConstPointer = SmartPointer< const Self >;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(StatisticsImageFilter, ImageToImageFilter);

  /** Image related type alias. */
  using InputImagePointer = typename TInputImage::Pointer;

  using RegionType = typename TInputImage::RegionType;
  using SizeType = typename TInputImage::SizeType;
  using IndexType = typename TInputImage::IndexType;
  using PixelType = typename TInputImage::PixelType;

  /** Image related type alias. */
  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;

  /** Type to use for computations. */
  using RealType = typename NumericTraits< PixelType >::RealType;

  /** Smart Pointer type to a DataObject. */
  using DataObjectPointer = typename DataObject::Pointer;

  /** Type of DataObjects used for scalar outputs */
  using RealObjectType = SimpleDataObjectDecorator< RealType >;
  using PixelObjectType = SimpleDataObjectDecorator< PixelType >;

  /** Return the computed Minimum. */
  PixelType GetMinimum() const
  { return this->GetMinimumOutput()->Get(); }
  PixelObjectType * GetMinimumOutput();

  const PixelObjectType * GetMinimumOutput() const;

  /** Return the computed Maximum. */
  PixelType GetMaximum() const
  { return this->GetMaximumOutput()->Get(); }
  PixelObjectType * GetMaximumOutput();

  const PixelObjectType * GetMaximumOutput() const;

  /** Return the computed Mean. */
  RealType GetMean() const
  { return this->GetMeanOutput()->Get(); }
  RealObjectType * GetMeanOutput();

  const RealObjectType * GetMeanOutput() const;

  /** Return the computed Standard Deviation. */
  RealType GetSigma() const
  { return this->GetSigmaOutput()->Get(); }
  RealObjectType * GetSigmaOutput();

  const RealObjectType * GetSigmaOutput() const;

  /** Return the computed Variance. */
  RealType GetVariance() const
  { return this->GetVarianceOutput()->Get(); }
  RealObjectType * GetVarianceOutput();

  const RealObjectType * GetVarianceOutput() const;

  /** Return the compute Sum. */
  RealType GetSum() const
  { return this->GetSumOutput()->Get(); }
  RealObjectType * GetSumOutput();

  const RealObjectType * GetSumOutput() const;

  /** Return the compute Sum of Squares. */
  RealType GetSumOfSquares() const
  { return this->GetSumOfSquaresOutput()->Get(); }
  RealObjectType * GetSumOfSquaresOutput();

  const RealObjectType * GetSumOfSquaresOutput() const;

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  using DataObjectPointerArraySizeType = ProcessObject::DataObjectPointerArraySizeType;
  using Superclass::MakeOutput;
  DataObjectPointer MakeOutput(DataObjectPointerArraySizeType idx) override;

#ifdef ITK_USE_CONCEPT_CHECKING
  // Begin concept checking
  itkConceptMacro( InputHasNumericTraitsCheck,
                   ( Concept::HasNumericTraits< PixelType > ) );
  // End concept checking
#endif

protected:
  StatisticsImageFilter();
  ~StatisticsImageFilter() override {}
  void PrintSelf(std::ostream & os, Indent indent) const override;

  /** Pass the input through unmodified. Do this by Grafting in the
   *  AllocateOutputs method.
   */
  void AllocateOutputs() override;

  /** Initialize some accumulators before the threads run. */
  void BeforeThreadedGenerateData() override;

  /** Do final mean and variance computation from data accumulated in threads.
   */
  void AfterThreadedGenerateData() override;

  void DynamicThreadedGenerateData( const RegionType &) override;

  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion() override;

  // Override since the filter produces all of its output
  void EnlargeOutputRequestedRegion(DataObject *data) override;

private:
  CompensatedSummation<RealType> m_ThreadSum;
  CompensatedSummation<RealType> m_SumOfSquares;

  SizeValueType m_Count;
  PixelType     m_ThreadMin;
  PixelType     m_ThreadMax;

  SimpleFastMutexLock m_Mutex;
}; // end of class
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkStatisticsImageFilter.hxx"
#endif

#endif
