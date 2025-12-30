@page cpp C++ Docs

Retinify is written in C++ and defined under the `retinify::` namespace.  
The key classes for using retinify are as follows:
  
- `retinify::Status`  
Holds the result of a function call.
- `retinify::Pipeline`  
Executes stereo matching on rectified image pairs.

@note  
Retinify is designed to avoid throwing exceptions. Instead, it uses `retinify::Status` to report and manage the result of each operation.
  
## Create a retinify project
We recommend using a CMake-based project when integrating retinify.  
Retinify requires **GCC 11 or later**.  
```cmake
find_package(retinify REQUIRED)
target_link_libraries(${PROJECT_NAME} retinify::retinify)
```

In this tutorial, we will use image data in the form of `cv::Mat`.  
Stereo matching can be performed using the `retinify::Pipeline`.  
  
```cpp
#include <retinify/retinify.hpp>
#include <opencv2/opencv.hpp>

// LOAD INPUT IMAGES
cv::Mat leftImage = cv::imread("path/to/left.png");
cv::Mat rightImage = cv::imread("path/to/right.png");

// PREPARE OUTPUT CONTAINER
cv::Mat disparity = cv::Mat::zeros(leftImage.size(), CV_32FC1);

// CREATE STEREO MATCHING PIPELINE
retinify::Pipeline pipeline;

// INITIALIZE THE PIPELINE
auto statusInitialize = pipeline.Initialize(static_cast<std::uint32_t>(leftImage.cols), static_cast<std::uint32_t>(leftImage.rows));
if (!statusInitialize.IsOK())
{
    return 1;
}

// EXECUTE STEREO MATCHING
auto statusExecute = pipeline.Execute(leftImage.ptr<std::uint8_t>(), leftImage.step[0], rightImage.ptr<std::uint8_t>(), rightImage.step[0]);
if (!statusExecute.IsOK())
{
    return 1;
}

// RETRIEVE DISPARITY
auto statusRetrieve = pipeline.RetrieveDisparity(disparity.ptr<float>(), disparity.step[0]);
if (!statusRetrieve.IsOK())
{
    return 1;
}

// SHOW DISPARITY
auto statusColorize = retinify::ColorizeDisparity(disparity.ptr<float>(), disparity.step[0], disparityColored.ptr<std::uint8_t>(), disparityColored.step[0], disparity.cols, disparity.rows, 256.0F);
if (!statusColorize.IsOK())
{
    return 1;
}

cv::cvtColor(disparityColored, disparityColored, cv::COLOR_RGB2BGR);
cv::imshow("disparity", disparityColored);
cv::waitKey(0);
```
