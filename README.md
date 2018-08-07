# StereoDepth
Software workflow for Stereo Camera Depth

## Camera Calibration
### Use calibration board and capture images at same time (Cpp Project StereoAcquisition) 

[comment]: # (TODO : Split project to new solution --> StereoAcquisition )

### Pick image pairs and do the calibration (Cpp Project StereoCalibration) 

[comment]: # (TODO : Split project to new solution --> StereoCalibration )

### Retrive extrinsics & intrinsics 

## DepthMap Caculation 

### Put camera at right place & capture 1 clear image pairs (Cpp Project GenCameraDriver) 

### Do the rectify on 2 cameras and resize image back to origin size (Cpp Project ImageRectify) 

[comment]: # (TODO : Split project to new solution --> ImageRectify )

### Do the Liuce's Optical Flow (Matlab Project OpticalFlow_LiuCe)

[comment]: # (Matlab Input/Output interface] OR [Cpp conversion])

### [Do the FlowConvert for HDR warping (Matlab Project FlowConvert)] 

### Do the disparity warping for panorama depth refine (Cpp Project DisparityWarping) 

[comment]: # (TODO : This project is still empty, working on it. )

### Convert disparity to depth map & use real depth(Baidu Map) to manipulate depth map (eg. A building) (Matlab Project DepthRefine Part I) 

### Convert real depth to distance from center point (Use K matrix) & Do the depth refine to smooth map (Matlab Project DepthRefine Part II) 

[comment]: # (TODO : DepthRefine Code Review )
