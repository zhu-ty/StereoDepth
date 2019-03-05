# StereoDepth
Software workflow for Stereo Camera Depth

How to clone (IMPORTANT!):
```
git clone https://github.com/zhu-ty/StereoDepth
cd StereoDepth
git submodule init
git submodule update
```
Make sure you have clone the submodule.

Please report any cmake bug to me via my email. (Not very sure that this cmake can work for all)

## Camera Calibration
### Use calibration board and capture images at same time (Cpp Project StereoAcquisition) 

### Pick image pairs and do the calibration (Cpp Project StereoCalibration) 

### Retrive extrinsics & intrinsics 

## DepthMap Caculation 

### Put camera at right place & capture 1 clear image pairs (Cpp Project GenCameraDriver) 

### Do the rectify on 2 cameras and resize image back to origin size (Cpp Project ImageRectify) 

### Do the Liuce's Optical Flow (Matlab Project OpticalFlow_LiuCe)

### [Do the FlowConvert for HDR warping (Matlab Project FlowConvert)] 

### Convert disparity to depth map & use real depth(Baidu Map) to manipulate depth map (eg. A building) 
### Convert real depth to distance from center point (Use K matrix)  (Matlab Project DepthRefine Part I) 

### Do the disparity warping for panorama depth refine (Cpp Project DisparityWarping) 

### Do the depth refine to smooth map  (Matlab Project DepthRefine Part II) 

