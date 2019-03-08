/**
@brief main.cpp
Online Stereo Image Capture
@author Shane Yuan
@date Apr 24, 2018
*/

// #include "StereoAcquisition.h"

#include "NPPJpegCoder.h"
#include "GenCameraDriver.h"

#include "SysUtil.hpp"

//TODO 这个项目本来是实时采集和标定的，现在只剩下采集的功能，因为实时标定太慢了，故将两个流程分开

cv::cuda::GpuMat applyWhiteBalanceBGR(cv::cuda::GpuMat img,
	float red, float green, float blue) {
	Npp32f wbTwist[3][4] = {
		{ 1.0, 0.0, 0.0, 0.0 },
		{ 0.0, 1.0, 0.0, 0.0 },
		{ 0.0, 0.0, 1.0, 0.0 }
	};
	// update wbTwist
	wbTwist[0][0] = blue;
	wbTwist[1][1] = green;
	wbTwist[2][2] = red;
	NppiSize osize;
	osize.width = img.cols;
	osize.height = img.rows;
	nppiColorTwist32f_8u_C3IR(img.data,
		img.step, osize, wbTwist);
	return img;
}


int main(int argc, char* argv[]) {
	std::vector<cam::GenCamInfo> camInfos;
	std::shared_ptr<cam::GenCamera> cameraPtr = cam::createCamera(cam::CameraModel::XIMEA_xiC);
	//std::shared_ptr<cam::GenCamera> cameraPtr = cam::createCamera(cam::CameraModel::PointGrey_u3);
	cameraPtr->init();

	cameraPtr->setSyncType(cam::GenCamSyncType::Hardware); //HARDWARE NEEDED!

	// set camera setting
	cameraPtr->setFPS(-1, 10);
	cameraPtr->startCapture();
	
	//cameraPtr->setAutoExposure(-1, cam::Status::on);
	//cameraPtr->setAutoExposureLevel(-1, 20);
	cameraPtr->setExposure(-1, 20000);
	//cameraPtr->setWhiteBalance(-1, 2.0, 1.0, 2.0); 
	cameraPtr->makeSetEffective();
	// set capturing setting
	
	cameraPtr->setCamBufferType(cam::GenCamBufferType::Raw);
	cameraPtr->setCaptureMode(cam::GenCamCaptureMode::Continous, 200);
	cameraPtr->setCapturePurpose(cam::GenCamCapturePurpose::Streaming);
	//cameraPtr->setAutoWhiteBalance(-1);
	// get camera info
	cameraPtr->getCamInfos(camInfos);
	cameraPtr->startCaptureThreads();


	std::vector<cam::Imagedata> imgdatas(2);
	std::vector<cv::Mat> imgs(2);
	std::vector<cv::Mat> imgs_c(2);
	std::vector<cv::cuda::GpuMat> imgs_bayer_d(2);
	std::vector<cv::cuda::GpuMat> imgs_d(2);

	SysUtil::sleep(1000);
	int nframe = 0;
	cv::Rect rect;

	SysUtil::mkdir("./images");
	SysUtil::mkdir("./images/0");
	SysUtil::mkdir("./images/1");
	cv::Mat display;
	for (;;) {

		cameraPtr->captureFrame(imgdatas);

		for (int i = 0; i < 2; i++)
		{
			imgs[i] = cv::Mat(camInfos[i].height, camInfos[i].width,
				CV_8U, reinterpret_cast<void*>(imgdatas[i].data));
			imgs_bayer_d[i].upload(imgs[i]);
			cv::cuda::demosaicing(imgs_bayer_d[i], imgs_d[i],
				npp::bayerPatternNPP2CVRGB(static_cast<NppiBayerGridPosition>(
					static_cast<int>(camInfos[i].bayerPattern))), -1);
			cv::cuda::cvtColor(imgs_d[i], imgs_d[i], cv::COLOR_RGB2BGR);
			if (camInfos[i].isWBRaw == false)
			{
				imgs_d[i] = applyWhiteBalanceBGR(imgs_d[i], 2.03, 1, 2.21); //RGB
			}
			imgs_d[i].download(imgs_c[i]);
		}

		cv::Size smallSize(imgs_c[0].cols / 5, imgs_c[0].rows / 5);
		cv::Mat smallImg1, smallImg2;
		cv::resize(imgs_c[0], smallImg1, smallSize);
		cv::resize(imgs_c[1], smallImg2, smallSize);
		display.create(smallImg1.rows, smallImg1.cols * 2, CV_8UC3);

		cv::Rect rect(0, 0, smallImg1.cols, smallImg1.rows);
		smallImg1.copyTo(display(rect));
		rect.x += smallImg1.cols;
		smallImg2.copyTo(display(rect));
		cv::imshow("corners", display);

		cv::imwrite(cv::format("images/0/%05d.png", nframe), imgs_c[0]);
		cv::imwrite(cv::format("images/1/%05d.png", nframe), imgs_c[1]);
		cv::waitKey(1);
		SysUtil::sleep(50);

		std::cout << nframe++ << std::endl;
	}

	cameraPtr->stopCaptureThreads();
	cameraPtr->release();
	//int a = 1000000;
	//Sleep(a);
	return 0;
}