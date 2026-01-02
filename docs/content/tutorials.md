@page tutorials Tutorials

## 1. Overall Structure

Retinify is designed by clearly separating the stereo processing pipeline into three phases:

- **Initialize**: Initialization using parameters and input conditions  
- **Execute**: GPU-based image processing and stereo matching  
- **Retrieve**: Transferring result data from GPU to CPU  

By separating initialization from per-frame execution, the runtime loop is kept minimal,
enabling stable real-time processing on the GPU.
This structure is shared across the Python, C++, and ROS2 APIs.

### 1.1 Initialize

The Initialize phase prepares all processing using information that is fixed before execution.

- Fixing the input image size  
- Allocating GPU resources
- Setting camera parameters  
- Precomputing internal data for distortion correction and rectification  

This phase is typically executed once at application startup.
All data required for per-frame processing is prepared here, but no image processing is performed.  

In C++, `retinify::Pipeline.Initialize()` corresponds to this phase.  

### 1.2 Execute

The Execute phase performs all per-frame processing on the GPU.

- Uploading left and right images to the GPU  
- Applying distortion correction and rectification via GPU remapping  
- Running stereo matching on the rectified images  
- Generating disparity data in GPU memory  

This phase is called repeatedly inside the real-time loop.
The CPU handles control flow only, while all image processing and computation are executed on the GPU.  

In C++, `retinify::Pipeline.Execute()` corresponds to this phase.  

### 1.3 Retrieve

The Retrieve phase transfers selected results from GPU to CPU.

- Retrieving stereo matching results (disparity)  
- Retrieving rectified left and right images  
- Retrieving derived results (depth maps or point clouds) computed from disparity  

All conversions are performed on the GPU before transfer to minimize data movement.
Retrieve can be called selectively, depending on application requirements.

In C++, `retinify::Pipeline.RetrieveDisparity()`, `retinify::Pipeline.RetrieveRectifiedImages()`, etc. correspond to this phase.  

### 1.4 Coordinate Systems

Retinify uses the same coordinate system conventions as [OpenCV](https://docs.opencv.org/4.x/d9/d0c/group__calib3d.html).  

![Coordinate Systems](https://docs.opencv.org/4.x/pinhole_camera_model.png)

### 2. Using retinify

There are three options for using retinify:

- 🐍 [Python API](@ref python)  
  The Python API is suitable for prototyping and research, allowing you to quickly experiment with retinify using concise and flexible code.

- 🧬 [C++ API](@ref cpp)  
  The C++ API is designed for production use, providing fine-grained control over performance, memory management, and execution flow.

- 🤖 [ROS2](@ref ros2)  
  The ROS2 integration is intended for robotic systems, enabling seamless interaction with other ROS2 nodes such as sensor drivers, SLAM, and navigation.
