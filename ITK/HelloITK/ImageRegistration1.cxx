/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

//  Software Guide : BeginCommandLineArgs
//    INPUTS:  {BrainProtonDensitySliceBorder20.png}
//    INPUTS:  {BrainProtonDensitySliceShifted13x17y.png}
//    OUTPUTS: {ImageRegistration1Output.png}
//    OUTPUTS: {ImageRegistration1DifferenceAfter.png}
//    OUTPUTS: {ImageRegistration1DifferenceBefore.png}
//  Software Guide : EndCommandLineArgs

// Software Guide : BeginLatex
//
// This example illustrates the use of the image registration framework in
// Insight.  It should be read as a ``Hello World'' for ITK registration.
// Instead of means to an end, this example should be read as a basic
// introduction to the elements typically involved when solving a problem
// of image registration.
//
// \index{itk::Image!Instantiation}
// \index{itk::Image!Header}
//
// A registration method requires the following set of components: two input
// images, a transform, a metric and an optimizer. Some of these components
// are parameterized by the image type for which the registration is intended.
// The following header files provide declarations of common types used for
// these components.
//
// Software Guide : EndLatex


// Software Guide : BeginCodeSnippet
#include "itkImageRegistrationMethodv4.h"
#include "itkTranslationTransform.h"
#include "itkMeanSquaresImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
// Software Guide : EndCodeSnippet


#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSubtractImageFilter.h"


class CommandIterationUpdate : public itk::Command
{
public:
  using Self = CommandIterationUpdate;
  using Superclass = itk::Command;
  using Pointer = itk::SmartPointer<Self>;
  itkNewMacro(Self);

protected:
  CommandIterationUpdate() = default;

public:
  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  using OptimizerPointer = const OptimizerType *;

  void
  Execute(itk::Object * caller, const itk::EventObject & event) override
  {
    Execute((const itk::Object *)caller, event);
  }

  void
  Execute(const itk::Object * object, const itk::EventObject & event) override
  {
    auto optimizer = static_cast<OptimizerPointer>(object);

    if (!itk::IterationEvent().CheckEvent(&event))
    {
      return;
    }

    std::cout << optimizer->GetCurrentIteration() << " = ";
    std::cout << optimizer->GetValue() << " : ";
    std::cout << optimizer->GetCurrentPosition() << std::endl;
  }
};


int
main(int argc, char * argv[])
{
  if (argc < 4)
  {
    std::cerr << "参数不足 " << std::endl;
    std::cerr << "用法: " << argv[0];
    std::cerr << " fixedImageFile  movingImageFile ";
    std::cerr << "outputImagefile [differenceImageAfter]";
    std::cerr << "[differenceImageBefore] [useEstimator]" << std::endl;
    return EXIT_FAILURE;
  }


  // 软件指南：BeginLatex
  //
  // 首先应实例化每个配准组件的类型。我们从选择图像维度
  // 和用于表示图像像素的类型开始。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  constexpr unsigned int Dimension = 2;
  using PixelType = float;
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 以下几行代码实例化了输入图像的类型。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using FixedImageType = itk::Image<PixelType, Dimension>;
  using MovingImageType = itk::Image<PixelType, Dimension>;
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 定义下面将把固定图像空间映射到移动图像空间的变换。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using TransformType = itk::TranslationTransform<double, Dimension>;
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 需要一个优化器来探索变换的参数空间，以寻找最佳的度量值。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using OptimizerType = itk::RegularStepGradientDescentOptimizerv4<double>;
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 度量将比较两幅图像之间的匹配程度。度量类型通常是
  // 模板化的，如以下类型声明所示。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using MetricType =
    itk::MeanSquaresImageToImageMetricv4<FixedImageType, MovingImageType>;
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 配准方法类型使用固定图像和移动图像的类型以及输出变换类型进行实例化。
  // 此类负责连接我们到目前为止描述的所有组件。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using RegistrationType = itk::
    ImageRegistrationMethodv4<FixedImageType, MovingImageType, TransformType>;
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 使用 \code{New()} 方法创建每个配准组件，并将其分配给
  // 各自的 \doxygen{SmartPointer}。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto metric = MetricType::New();
  auto optimizer = OptimizerType::New();
  auto registration = RegistrationType::New();
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 现在将每个组件连接到配准方法的实例。
  //
  // \index{itk::RegistrationMethodv4!SetMetric()}
  // \index{itk::RegistrationMethodv4!SetOptimizer()}
  // \index{itk::RegistrationMethodv4!SetFixedImage()}
  // \index{itk::RegistrationMethodv4!SetMovingImage()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  registration->SetMetric(metric);
  registration->SetOptimizer(optimizer);
  // 软件指南：EndCodeSnippet


  // 软件指南：BeginLatex
  //
  // 在此示例中，不需要创建并将变换对象传递给配准方法，
  // 因为配准滤波器将使用传递给它的模板参数类型
  // 实例化一个内部变换对象。
  //
  // 度量需要一个插值器来评估固定和移动图像在非网格位置的强度。
  // 这里声明了固定和移动插值器的类型。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using FixedLinearInterpolatorType =
    itk::LinearInterpolateImageFunction<FixedImageType, double>;

  using MovingLinearInterpolatorType =
    itk::LinearInterpolateImageFunction<MovingImageType, double>;
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 然后，创建固定和移动插值器并将其传递给度量。由于在本例中
  // 使用了线性插值器作为默认设置，我们可以跳过以下步骤。
  //
  // \index{itk::MeanSquaresImageToImageMetricv4!SetFixedInterpolator()}
  // \index{itk::MeanSquaresImageToImageMetricv4!SetMovingInterpolator()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto fixedInterpolator = FixedLinearInterpolatorType::New();
  auto movingInterpolator = MovingLinearInterpolatorType::New();

  metric->SetFixedInterpolator(fixedInterpolator);
  metric->SetMovingInterpolator(movingInterpolator);
  // 软件指南：EndCodeSnippet

  using FixedImageReaderType = itk::ImageFileReader<FixedImageType>;
  using MovingImageReaderType = itk::ImageFileReader<MovingImageType>;
  auto fixedImageReader = FixedImageReaderType::New();
  auto movingImageReader = MovingImageReaderType::New();

  fixedImageReader->SetFileName(argv[1]);
  movingImageReader->SetFileName(argv[2]);

  // 软件指南：BeginLatex
  //
  // 在此示例中，固定图像和移动图像从文件中读取。
  // 这要求 \doxygen{ImageRegistrationMethodv4} 从读取器的输出中获取其输入。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  registration->SetFixedImage(fixedImageReader->GetOutput());
  registration->SetMovingImage(movingImageReader->GetOutput());
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 现在应初始化配准过程。ITKv4 配准框架提供了固定图像和移动图像的初始变换。
  // 这些变换可用于设置虚拟域和固定/移动图像空间之间已知的初始校正。
  // 在这个特定的案例中，使用平移变换来初始化移动图像空间。
  // 初始移动变换的参数数组仅由沿各维度的平移值组成。
  // 将参数的值设置为零将变换初始化为 \emph{Identity}（单位）变换。
  // 请注意，数组构造函数需要传递元素数量作为参数。
  //
  // \index{itk::TranslationTransform!GetNumberOfParameters()}
  // \index{itk::RegistrationMethodv4!SetMovingInitialTransform()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto movingInitialTransform = TransformType::New();

  TransformType::ParametersType initialParameters(
    movingInitialTransform->GetNumberOfParameters());
  initialParameters[0] = 0.0; // 沿 X 方向的初始偏移量 (mm)
  initialParameters[1] = 0.0; // 沿 Y 方向的初始偏移量 (mm)

  movingInitialTransform->SetParameters(initialParameters);

  registration->SetMovingInitialTransform(movingInitialTransform);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 在配准滤波器中，该移动初始变换将被添加到已经包含输出可优化变换实例的复合变换中；
  // 然后，优化器将使用生成的复合变换来评估每次迭代的度量值。
  //
  // 尽管如此，固定初始变换不会对优化过程产生影响。它仅用于从度量评估发生的虚拟图像空间访问固定图像。
  //
  // 虚拟图像是 ITKv4 配准框架中新增加的一个概念，它允许我们在一个与固定和移动图像域完全不同的物理域中进行配准过程。
  // 实际上，度量评估在其中进行的区域称为虚拟图像域。该域定义了评估的分辨率以及物理坐标系。
  //
  // 虚拟参考域取自“虚拟图像”缓存区域，输入图像应使用固定和移动初始变换从该参考空间进行访问。
  //
  // 传统的直观配准框架可以被视为虚拟域与固定图像域相同的特例。
  // 由于这种情况在大多数实际应用中几乎都发生，因此虚拟图像默认设置为与固定图像相同。
  // 然而，用户可以通过调用 \code{SetVirtualDomain} 或 \code{SetVirtualDomainFromImage} 将虚拟域定义为与固定图像域不同。
  //
  // 在本例中，与本章的其他大多数示例一样，虚拟图像被视为与固定图像相同。
  // 由于配准过程发生在固定图像物理域中，因此固定初始变换保持其默认的身份值，不需要设置。
  //
  // 但是，一个“Hello World!”示例应该展示所有基础知识，因此此处明确设置了所有配准组件。
  //
  // 在本章的下一部分中，当初始固定变换不是单位变换时，您将更好地理解配准过程的幕后工作。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto identityTransform = TransformType::New();
  identityTransform->SetIdentity();

  registration->SetFixedInitialTransform(identityTransform);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 请注意，上述过程仅展示了一种初始化配准配置的方法。
  // 另一种选择是直接初始化输出可优化变换。在此方法中，创建一个变换对象，进行初始化，然后通过 \code{SetInitialTransform()} 将其传递给配准方法。
  // 这种方法在章节~\ref{sec:RigidRegistrationIn2D} 中展示。
  //
  // 此时，配准方法已准备好执行。优化器是驱动配准执行的组件。
  // 但是，ImageRegistrationMethodv4 类协调整个系统，以确保在将控制权交给优化器之前一切就绪。
  //
  // 通常，调整优化器的参数是有利的。
  // 每个优化器都有特定的参数，必须在其实现的优化策略的上下文中进行解释。
  // 在本例中使用的优化器是一种梯度下降变体，试图防止其采取过大的步骤。
  // 在每次迭代中，此优化器将沿 \doxygen{ImageToImageMetricv4} 导数方向前进。
  // 每次导数方向突然变化时，优化器假定已经经过了局部极值，并通过降低步长来做出反应。
  // 缓解因子应该介于 0 到 1 之间。该因子默认为 0.5，可以通过 \code{SetRelaxationFactor()} 更改为不同的值。
  // 此外，初始步长的默认值为 1，可以通过 \code{SetLearningRate()} 方法手动更改该值。
  //
  // 除了手动设置外，还可以通过分配 ScalesEstimator（将在后续示例中看到）自动估算初始步长，无论是每次迭代还是仅在第一次迭代时。
  //
  // 在多次减少步长后，优化器可能在变换参数空间的非常有限的区域内移动。
  // 用户可以通过 \code{SetMinimumStepLength()} 方法定义步长应有多小，以便认为已达到收敛。
  // 这相当于定义最终变换的精度。
  // 用户还可以手动设置其他停止标准，例如最大迭代次数。
  //
  // 在 ITKv4 框架的其他基于梯度下降的优化器中，例如 \doxygen{GradientDescentLineSearchOptimizerv4} 和 \doxygen{ConjugateGradientLineSearchOptimizerv4}，
  // 收敛标准通过 \code{SetMinimumConvergenceValue()} 设置，该标准基于最后几次迭代的结果计算。
  // 参与计算的迭代次数由收敛窗口大小通过 \code{SetConvergenceWindowSize()} 定义，这将在本章后续示例中展示。
  //
  // 还要注意，与以前的版本不同，ITKv4 优化器没有“最大化/最小化”选项来修改度量导数的影响。
  // 假定每个分配的度量都返回一个改进优化的参数导数结果。
  //
  // \index{itk::Gradient\-Descent\-Optimizerv4\-Template!SetLearningRate()}
  // \index{itk::Gradient\-Descent\-Optimizerv4\-Template!SetMinimumStepLength()}
  // \index{itk::Gradient\-Descent\-Optimizerv4\-Template!SetRelaxationFactor()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  optimizer->SetLearningRate(4);
  optimizer->SetMinimumStepLength(0.001);
  optimizer->SetRelaxationFactor(0.5);
  // 软件指南：EndCodeSnippet

  bool useEstimator = false;
  if (argc > 6)
  {
    useEstimator = std::stoi(argv[6]) != 0;
  }

  if (useEstimator)
  {
    using ScalesEstimatorType =
      itk::RegistrationParameterScalesFromPhysicalShift<MetricType>;
    auto scalesEstimator = ScalesEstimatorType::New();
    scalesEstimator->SetMetric(metric);
    scalesEstimator->SetTransformForward(true);
    optimizer->SetScalesEstimator(scalesEstimator);
    optimizer->SetDoEstimateLearningRateOnce(true);
  }

  // 软件指南：BeginLatex
  //
  // 如果优化器未能达到所需的精度公差，建议设定要执行的最大迭代次数限制。
  // 该最大值由 \code{SetNumberOfIterations()} 方法定义。
  //
  // \index{itk::Gradient\-Descent\-Optimizerv4\-Template!SetNumberOfIterations()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  optimizer->SetNumberOfIterations(200);
  // 软件指南：EndCodeSnippet

  // 连接观察者
  auto observer = CommandIterationUpdate::New();
  optimizer->AddObserver(itk::IterationEvent(), observer);

  // 软件指南：BeginLatex
  //
  // ITKv4 提供了多级配准框架，其中每个阶段的虚拟空间分辨率和固定/移动图像的平滑度各不相同。
  // 在配准开始之前，需要定义这些标准。否则，将使用默认值。
  // 在此示例中，我们在一级上运行简单的配准，没有对输入数据进行空间缩小或平滑处理。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  constexpr unsigned int numberOfLevels = 1;

  RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
  shrinkFactorsPerLevel.SetSize(1);
  shrinkFactorsPerLevel[0] = 1;

  RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
  smoothingSigmasPerLevel.SetSize(1);
  smoothingSigmasPerLevel[0] = 0;

  registration->SetNumberOfLevels(numberOfLevels);
  registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
  registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 通过调用 \code{Update()} 方法触发配准过程。如果在初始化或执行配准期间发生错误，将抛出异常。
  // 因此，我们应将 \code{Update()} 方法放在 \code{try/catch} 块中，如下所示。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  try
  {
    registration->Update();
    std::cout << "优化器停止条件: "
              << registration->GetOptimizer()->GetStopConditionDescription()
              << std::endl;
  }
  catch (const itk::ExceptionObject & err)
  {
    std::cerr << "捕获到异常对象!" << std::endl;
    std::cerr << err << std::endl;
    return EXIT_FAILURE;
  }
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 在实际应用中，您可能会尝试通过在 catch 块中采取更有效的操作来恢复错误。
  // 在这里，我们只是打印一条消息，然后终止程序的执行。
  //
  //
  // 配准过程的结果是通过 \code{GetTransform()} 方法获得的，该方法返回一个指向输出变换的常量指针。
  //
  // \index{itk::ImageRegistrationMethodv4!GetTransform()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  TransformType::ConstPointer transform = registration->GetTransform();
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 对于 \doxygen{TranslationTransform}，参数的解释非常简单。
  // 数组中的每个元素对应于沿一个空间维度的平移。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  TransformType::ParametersType finalParameters = transform->GetParameters();
  const double TranslationAlongX = finalParameters[0];
  const double TranslationAlongY = finalParameters[1];
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 可以通过调用 \code{GetCurrentIteration()} 方法查询优化器执行达到收敛所需的实际迭代次数。
  // 迭代次数过多可能表明学习率设置得太小，这是不理想的，因为它会导致较长的计算时间。
  //
  // \index{itk::Gradient\-Descent\-Optimizerv4\-Template!GetCurrentIteration()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  const unsigned int numberOfIterations = optimizer->GetCurrentIteration();
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 可以通过优化器的 \code{GetValue()} 方法获取最后一组参数对应的图像度量值。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  const double bestValue = optimizer->GetValue();
  // 软件指南：EndCodeSnippet

  // 打印结果
  //
  std::cout << "结果 = " << std::endl;
  std::cout << " 平移 X = " << TranslationAlongX << std::endl;
  std::cout << " 平移 Y = " << TranslationAlongY << std::endl;
  std::cout << " 迭代次数 = " << numberOfIterations << std::endl;
  std::cout << " 度量值 = " << bestValue << std::endl;

  // 软件指南：BeginLatex
  //
  // 让我们在 \code{Examples/Data} 中提供的两个图像上执行此示例：
  //
  // \begin{itemize}
  // \item \code{BrainProtonDensitySliceBorder20.png}
  // \item \code{BrainProtonDensitySliceShifted13x17y.png}
  // \end{itemize}
  //
  // 第二张图像是通过将第一张图像有意平移 $(13,17)$ 毫米得到的。两幅图像都具有单位间距，显示在图 \ref{fig:FixedMovingImageRegistration1} 中。
  // 配准需要 20 次迭代，得到的变换参数为：
  //
  // \begin{verbatim}
  // Translation X = 13.0012
  // Translation Y = 16.9999
  // \end{verbatim}
  //
  // 正如预期的那样，这些值非常符合我们在移动图像中故意引入的错位。
  //
  // \begin{figure}
  // \center
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceBorder20}
  // \includegraphics[width=0.44\textwidth]{BrainProtonDensitySliceShifted13x17y}
  // \itkcaption[Fixed and Moving images in registration framework]{输入到配准方法中的固定图像和移动图像。}
  // \label{fig:FixedMovingImageRegistration1}
  // \end{figure}
  //
  //
  // 软件指南：EndLatex

  // 软件指南：BeginLatex
  //
  // 在配准任务的最后一步中，通常使用生成的变换将移动图像映射到固定图像空间。
  //
  // 在映射过程之前，请注意我们在本示例中没有使用输出变换的直接初始化，因此移动初始变换的参数未反映在配准滤波器的输出参数中。
  // 因此，需要一个复合变换来将初始和输出变换组合在一起。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using CompositeTransformType = itk::CompositeTransform<double, Dimension>;
  auto outputCompositeTransform = CompositeTransformType::New();
  outputCompositeTransform->AddTransform(movingInitialTransform);
  outputCompositeTransform->AddTransform(
    registration->GetModifiableTransform());
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 现在可以轻松使用 \doxygen{ResampleImageFilter} 进行映射过程。
  // 有关此滤波器的详细用法，请参阅 Section~\ref{sec:ResampleImageFilter}。
  // 首先，使用图像类型实例化 ResampleImageFilter 类型。使用固定图像类型作为输出类型是方便的，因为移动图像的变换后图像可能会与固定图像进行比较。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using ResampleFilterType =
    itk::ResampleImageFilter<MovingImageType, FixedImageType>;
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 创建重采样滤波器并将移动图像连接为其输入。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto resampler = ResampleFilterType::New();
  resampler->SetInput(movingImageReader->GetOutput());
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 创建的输出复合变换也作为输入传递给重采样滤波器。
  //
  // \index{itk::ImageRegistrationMethod!Resampling image}
  // \index{itk::ImageRegistrationMethod!Pipeline}
  // \index{itk::ImageRegistrationMethod!DataObjectDecorator}
  // \index{itk::ImageRegistrationMethod!GetOutput()}
  // \index{itk::DataObjectDecorator!Use in Registration}
  // \index{itk::DataObjectDecorator!Get()}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  resampler->SetTransform(outputCompositeTransform);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 如 Section \ref{sec:ResampleImageFilter} 中所述，ResampleImageFilter 需要指定其他参数，
  // 特别是输出图像的间距、原点和大小。还将默认像素值设置为明显的灰度级，以突出显示移动图像之外的区域。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  FixedImageType::Pointer fixedImage = fixedImageReader->GetOutput();
  resampler->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
  resampler->SetOutputOrigin(fixedImage->GetOrigin());
  resampler->SetOutputSpacing(fixedImage->GetSpacing());
  resampler->SetOutputDirection(fixedImage->GetDirection());
  resampler->SetDefaultPixelValue(100);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // \begin{figure}
  // \center
  // \includegraphics[width=0.32\textwidth]{ImageRegistration1Output}
  // \includegraphics[width=0.32\textwidth]{ImageRegistration1DifferenceBefore}
  // \includegraphics[width=0.32\textwidth]{ImageRegistration1DifferenceAfter}
  // \itkcaption[HelloWorld 配准输出图像]{映射的移动图像及其与配准前后固定图像的差异。}
  // \label{fig:ImageRegistration1Output}
  // \end{figure}
  //
  // 软件指南：EndLatex

  // 软件指南：BeginLatex
  //
  // 滤波器的输出传递给一个写入器，后者将图像存储在文件中。
  // 使用 \doxygen{CastImageFilter} 将重采样图像的像素类型转换为写入器使用的最终类型。
  // 下面实例化了转换和写入器滤波器。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using OutputPixelType = unsigned char;

  using OutputImageType = itk::Image<OutputPixelType, Dimension>;

  using CastFilterType =
    itk::CastImageFilter<FixedImageType, OutputImageType>;

  using WriterType = itk::ImageFileWriter<OutputImageType>;
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 通过调用 \code{New()} 方法创建滤波器。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto writer = WriterType::New();
  auto caster = CastFilterType::New();
  // 软件指南：EndCodeSnippet

  writer->SetFileName(argv[3]);

  // 软件指南：BeginLatex
  //
  // 连接滤波器并调用写入器的 \code{Update()} 方法，以触发管道的执行。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  caster->SetInput(resampler->GetOutput());
  writer->SetInput(caster->GetOutput());
  writer->Update();
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // \begin{figure}
  // \center
  // \includegraphics[width=\textwidth]{ImageRegistration1Pipeline}
  // \itkcaption[配准示例的管道结构]{配准示例的管道结构。}
  // \label{fig:ImageRegistration1Pipeline}
  // \end{figure}
  //
  //
  // 软件指南：EndLatex

  // 软件指南：BeginLatex
  //
  // 可以轻松使用 \doxygen{SubtractImageFilter} 比较固定图像和变换后的移动图像。
  // 该像素级滤波器计算其两个输入图像的同源像素之间的差异。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using DifferenceFilterType =
    itk::SubtractImageFilter<FixedImageType, FixedImageType, FixedImageType>;

  auto difference = DifferenceFilterType::New();

  difference->SetInput1(fixedImageReader->GetOutput());
  difference->SetInput2(resampler->GetOutput());
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 请注意，使用减法作为图像比较方法在这里是合适的，因为我们选择使用像素类型 \code{float} 表示图像。
  // 如果图像的像素类型是任何 \code{unsigned} 整数类型，则应使用不同的滤波器。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginLatex
  //
  // 由于两幅图像之间的差异可能对应于非常低的强度值，
  // 我们使用 \doxygen{RescaleIntensityImageFilter} 来重新调整这些强度值，以使它们更为可见。
  // 这种重新调整还使我们能够在保存差异图像为仅支持无符号像素值的文件格式时（如 PNG、BMP、JPEG 和 TIFF 等常见文件格式），
  // 仍然可以可视化负值。我们还将 \code{DefaultPixelValue} 减小到“1”，以防止该值吸收两幅图像之间差异的动态范围。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  using RescalerType =
    itk::RescaleIntensityImageFilter<FixedImageType, OutputImageType>;

  auto intensityRescaler = RescalerType::New();

  intensityRescaler->SetInput(difference->GetOutput());
  intensityRescaler->SetOutputMinimum(0);
  intensityRescaler->SetOutputMaximum(255);

  resampler->SetDefaultPixelValue(1);
  // 软件指南：EndCodeSnippet

  // 软件指南：BeginLatex
  //
  // 其输出可以传递给另一个写入器。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  auto writer2 = WriterType::New();
  writer2->SetInput(intensityRescaler->GetOutput());
  // 软件指南：EndCodeSnippet

  if (argc > 4)
  {
    writer2->SetFileName(argv[4]);
    writer2->Update();
  }

  // 软件指南：BeginLatex
  //
  // 为了进行比较，还可以通过简单地将变换设置为身份变换来计算配准前固定图像和移动图像之间的差异。
  // 请注意，仍然需要进行重采样，因为移动图像不一定具有与固定图像相同的间距、原点和像素数。
  // 因此，通常不能执行像素级别的操作。使用身份变换的重采样过程将确保我们在固定图像的网格中表示移动图像。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginCodeSnippet
  resampler->SetTransform(identityTransform);
  // 软件指南：EndCodeSnippet

  if (argc > 5)
  {
    writer2->SetFileName(argv[5]);
    writer2->Update();
  }

  // 软件指南：BeginLatex
  //
  // 本示例的完整管道结构如图~\ref{fig:ImageRegistration1Pipeline} 所示。
  // 还描述了配准方法的组件。图~\ref{fig:ImageRegistration1Output}（左）显示了为了将移动图像映射到固定图像空间而重采样后的结果。
  // 图像的顶部和右侧边框显示为在 ResampleImageFilter 中使用 \code{SetDefaultPixelValue()} 选择的灰度级。
  // 中间图像显示了固定图像和原始移动图像之间的差异（即在执行配准之前的差异）。
  // 右侧图像显示了固定图像和变换后移动图像之间的差异（即在执行配准之后的差异）。
  // 两个差异图像的强度均已重新调整，以突出存在差异的像素。
  // 请注意，最终的配准仍然偏离了一个像素的一部分，这导致解剖结构边缘周围的条带出现在差异图像中。
  // 完美的配准将生成一个无差异图像。
  //
  // 软件指南：EndLatex

  // 软件指南：BeginLatex
  //
  // \begin{figure}
  // \center
  // \includegraphics[height=0.44\textwidth]{ImageRegistration1TraceTranslations}
  // \includegraphics[height=0.44\textwidth]{ImageRegistration1TraceMetric}
  // \itkcaption[配准期间的平移和度量跟踪]{优化器每次迭代的平移和图像度量值序列。}
  // \label{fig:ImageRegistration1Trace}
  // \end{figure}
  //
  // 始终记住，配准本质上是一个优化问题是很有用的。
  // 图~\ref{fig:ImageRegistration1Trace} 通过显示优化器每次迭代时的平移和图像度量值，帮助我们加强这一概念。
  // 从顶部图可以看出，随着优化器接近度量极值，步长逐渐减小。
  // 底部图清楚地显示了优化随着优化的进行而减少的度量值。
  // 日志图有助于突出优化器围绕极值的正常波动。
  //
  // 在本节中，我们使用了一个非常简单的示例来介绍 ITKv4 中配准过程的基本组件。
  // 然而，仅研究此示例还不足以开始使用 \doxygen{ImageRegistrationMethodv4}。
  // 为了为特定应用选择最佳配准实践，需要了解其他配准方法实例及其功能。
  // 例如，章节~\ref{sec:RigidRegistrationIn2D} 中展示了输出可优化变换的直接初始化方法。
  // 该方法在许多情况下可以简化配准过程。还在章节~\ref{sec:MultiResolutionRegistration} 和~\ref{sec:MultiStageRegistration} 中说明了多分辨率和多阶段配准方法。
  // 这些示例展示了 ITKv4 配准方法框架的灵活性，有助于提供更快且更可靠的配准过程。
  //
  // 软件指南：EndLatex

  return EXIT_SUCCESS;
}
