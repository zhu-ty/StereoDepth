/**
@brief main.cpp
Online Stereo Image Capture
@author Shane Yuan
@date Apr 24, 2018
*/

#include "StereoAcquisition.h"

#include "NPPJpegCoder.h"
#include "GenCameraDriver.h"

#include "SysUtil.hpp"

//TODO 这个项目本来是实时采集和标定的，现在只剩下采集的功能，因为实时标定太慢了，故将两个流程分开

int main(int argc, char* argv[]) {
	std::vector<cam::GenCamInfo> camInfos;
	//std::shared_ptr<cam::GenCamera> cameraPtr = cam::createCamera(cam::CameraModel::XIMEA_xiC);
	std::shared_ptr<cam::GenCamera> cameraPtr = cam::createCamera(cam::CameraModel::PointGrey_u3);
	cameraPtr->init();
	// set camera setting
	cameraPtr->startCapture();
	cameraPtr->setFPS(-1, 10);
	//cameraPtr->setAutoExposure(-1, cam::Status::on);
	//cameraPtr->setAutoExposureLevel(-1, 20);
	cameraPtr->setExposure(-1, 10000);
	cameraPtr->makeSetEffective();
	// set capturing setting
	cameraPtr->setCaptureMode(cam::GenCamCaptureMode::Continous, 200);
	cameraPtr->setAutoWhiteBalance(-1);
	// get camera info
	cameraPtr->getCamInfos(camInfos);
	cameraPtr->startCaptureThreads();

	cv::Mat K1 = cv::Mat::zeros(3, 3, CV_32F);
	K1.at<float>(0, 0) = 7138;
	K1.at<float>(1, 1) = 7138;
	K1.at<float>(2, 2) = 1;
	K1.at<float>(0, 2) = 1232;
	K1.at<float>(1, 2) = 1028;
	cv::Mat K2 = K1.clone();
	cv::Size chessBoardSize(11, 8);

	//cv::Mat img1 = cv::imread("1.png");
	//cv::Mat img2 = cv::imread("2.png");

	StereoCalibration stereoCalibrator;
	stereoCalibrator.init(K1, K2, chessBoardSize);

	std::vector<cam::Imagedata> imgdatas(2);
	std::vector<cv::Mat> imgs(2);
	std::vector<cv::Mat> imgs_c(2);
	std::vector<cv::cuda::GpuMat> imgs_bayer_d(2);
	std::vector<cv::cuda::GpuMat> imgs_d(2);

	//cv::Mat wbMat(camInfos[0].height, camInfos[0].width, CV_8UC3);
	//wbMat.setTo(cv::Scalar(2, 1, 2));
	//cv::cuda::GpuMat wbMat_d;
	//wbMat_d.upload(wbMat);

	SysUtil::sleep(1000);
	int nframe = 0;
	cv::Rect rect;

	SysUtil::mkdir("./images");
	SysUtil::mkdir("./images/0");
	SysUtil::mkdir("./images/1");
	for (;;) {
		std::cout << nframe++ << std::endl;
		cameraPtr->captureFrame(imgdatas);

		imgs[0] = cv::Mat(camInfos[0].height, camInfos[0].width,
			CV_8U, reinterpret_cast<void*>(imgdatas[0].data));
		imgs[1] = cv::Mat(camInfos[1].height, camInfos[1].width,
			CV_8U, reinterpret_cast<void*>(imgdatas[1].data));

		imgs_bayer_d[0].upload(imgs[0]);
		imgs_bayer_d[1].upload(imgs[1]);

		cv::cuda::demosaicing(imgs_bayer_d[0], imgs_d[0], cv::COLOR_BayerRG2BGR, -1);
		cv::cuda::demosaicing(imgs_bayer_d[1], imgs_d[1], cv::COLOR_BayerRG2BGR, -1);

		imgs_d[0] = StereoCalibration::applyWhiteBalance(imgs_d[0], 1, 1, 1.0);
		imgs_d[1] = StereoCalibration::applyWhiteBalance(imgs_d[1], 1, 1, 1.0);

		imgs_d[0].download(imgs_c[0]);
		imgs_d[1].download(imgs_c[1]);

		stereoCalibrator.SaveImg(imgs_c[0], imgs_c[1]);
		SysUtil::sleep(50);
	}

	cameraPtr->stopCaptureThreads();
	cameraPtr->release();

	return 0;
}