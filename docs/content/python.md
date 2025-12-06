@page python Python Docs

@section introduction Introduction

The Python bindings of **retinify** are designed to use **NumPy arrays** as the primary input and output format.  
Images loaded using libraries such as **Pillow** or **opencv-python** can be passed directly as `numpy.ndarray` objects.
  
A typical usage flow looks like this:

- Load images as NumPy arrays  
- Initialize a `Pipeline` with the desired image size, pixel format, depth mode, and calibration parameters  
- Provide left/right images and run stereo matching  
- Retrieve disparity, depth, rectified images, or a point cloud as needed

@section installation Install Python bindings

Install the Python bindings with:
```bash
pip install git+https://github.com/retinify/retinify-python.git
```

@section example Example usage

1. Stereo matching with rectified images
```python
import numpy as np
from PIL import Image
from retinify import Pipeline

# LOAD RECTIFIED STEREO INPUT IMAGES
left = np.asarray(Image.open("path/to/left.png").convert("RGB"))
right = np.asarray(Image.open("path/to/right.png").convert("RGB"))

# CREATE STEREO MATCHING PIPELINE
pipe = Pipeline()

# INITIALIZE THE PIPELINE
pipe.initialize(image_width=left.shape[1], 
                image_height=left.shape[0])

# EXECUTE STEREO MATCHING
pipe.execute(left, right)

# RETRIEVE DISPARITY
disparity = pipe.retrieve_disparity()
```

2. Colorizing and saving the disparity map
```python
from PIL import Image
from retinify import colorize_disparity

disparity_color = colorize_disparity(disparity, max_disparity=128)

Image.fromarray(disparity_color).save("disparity.png")
```

3. Stereo matching with non-rectified images and calibration parameters
```python
import numpy as np
from PIL import Image
from retinify import Pipeline, load_calibration_parameters, PixelFormat, DepthMode

# LOAD NON-RECTIFIED STEREO INPUT IMAGES
left = np.asarray(Image.open("path/to/left.png").convert("RGB"))
right = np.asarray(Image.open("path/to/right.png").convert("RGB"))

# LOAD CALIBRATION PARAMETERS
calib_params = load_calibration_parameters("path/to/calib.json")

# CREATE STEREO MATCHING PIPELINE
pipe = Pipeline()

# INITIALIZE THE PIPELINE WITH CALIBRATION PARAMETERS
pipe.initialize(image_width=left.shape[1], 
                image_height=left.shape[0],
                pixel_format=PixelFormat.RGB8,
                depth_mode=DepthMode.ACCURATE,
                calibration_parameters=calib_params)

# EXECUTE STEREO MATCHING
pipe.execute(left, right)

# RETRIEVE DISPARITY
disparity = pipe.retrieve_disparity()

# RETRIEVE DEPTH
depth = pipe.retrieve_depth()

# RETRIEVE POINT CLOUD
point_cloud = pipe.retrieve_point_cloud()
```