@page demos Demos

Please refer to the following repository for the code examples used throughout this demo:  
👉 [Example Repository](https://github.com/retinify/retinify-opencv-template)

## 1. Install retinify
Please refer to the [Installation](@ref installation) section for details.  

## 2. Install OpenCV
```
sudo apt install libopencv-dev
```

## 3. Build the Example Project
This section demonstrates how to perform stereo matching on an arbitrary stereo image pair.  
First, clone the repository used for the demo:  

```bash
git clone https://github.com/retinify/retinify-opencv-example.git
cd retinify-opencv-example
```

Next, build the project:  

```bash
mkdir build
cd build
cmake ..
make
```

An executable will be produced in the build directory.  
Sample stereo images are available in the `retinify-opencv-example/img` directory (you may also use your own images).  
Run the executable with the left and right image paths as arguments:  

@note
If TensorRT is used as the backend, creating the engine file may take some time.

```bash
./retinify-opencv-example <left_image_path> <right_image_path>
```

After successful execution, a file named `disparity.png` will be saved in the same directory as the executable.  
If you use the images provided in the `retinify-opencv-example/img` directory, you should obtain a disparity image similar to the following:

![demo_output](https://raw.githubusercontent.com/retinify/retinify-opencv-example/main/img/disparity.png)
