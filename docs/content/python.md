@page python Python Docs


## Installation

First, follow the instructions in the [Installation](@ref installation) section to install retinify.  
Then, install the Python bindings using:

```bash
pip install git+https://github.com/retinify/retinify-python.git
```


## Writing Python code

The Python bindings of retinify are designed to use **NumPy arrays** as the primary input and output format.  
Images loaded using libraries such as **Pillow** or **opencv-python** can be passed directly as `numpy.ndarray` objects.


To perform stereo matching with rectified stereo images, use the following code:  

```python
import numpy as np
from PIL import Image
from retinify import Pipeline

# LOAD RECTIFIED STEREO INPUT IMAGES
left = np.asarray(Image.open("path/to/left.png").convert("RGB"))
right = np.asarray(Image.open("path/to/right.png").convert("RGB"))

# CREATE THE STEREO MATCHING PIPELINE
pipe = Pipeline()

# INITIALIZE THE PIPELINE
pipe.initialize(image_width=left.shape[1], 
                image_height=left.shape[0])

# EXECUTE STEREO MATCHING
pipe.execute(left, right)

# RETRIEVE DISPARITY
disparity = pipe.retrieve_disparity()
```

When initializing the pipeline, you can provide calibration parameters to enable the following features:

- Distortion correction and rectification
- Depth and point cloud generation

To perform stereo matching with non-rectified stereo images, use the following code:

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

# RETRIEVE RECTIFIED LEFT IMAGE
left_rectified = pipe.retrieve_rectified_left_image()

# RETRIEVE DISPARITY
disparity = pipe.retrieve_disparity()

# RETRIEVE DEPTH
depth = pipe.retrieve_depth()

# RETRIEVE POINT CLOUD
point_cloud = pipe.retrieve_point_cloud()
```