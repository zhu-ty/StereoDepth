/**
@brief StereoCalibration.cpp
Online stereo calibration class
@author Shane Yuan
@date Apr 24, 2018
*/

// npp
#include <npp.h>
#include <nppi_compression_functions.h>

#include "SysUtil.hpp"
#include <opencv2/core/utility.hpp>
#include "StereoAcquisition.h"

StereoCalibration::StereoCalibration() : chessBoardSize(11, 8), validFrameInd(0) {}
StereoCalibration::~StereoCalibration() {}


int StereoCalibration::init(cv::Mat K1, cv::Mat K2, cv::Size chessBoardSize) {
	this->K1 = K1.clone();
	this->K2 = K2.clone();
	this->chessBoardSize = chessBoardSize;
	return 0;
}


bool isRotationMatrix(cv::Mat &R) {
	cv::Mat Rt;
	transpose(R, Rt);
	cv::Mat shouldBeIdentity = Rt * R;
	cv::Mat I = cv::Mat::eye(3, 3, shouldBeIdentity.type());
	return cv::norm(I, shouldBeIdentity) < 1e-6;
}


cv::Vec3f rotationMatrixToEulerAngles(cv::Mat &R) {
	assert(isRotationMatrix(R));
	float sy = sqrt(R.at<double>(0, 0) * R.at<double>(0, 0) + R.at<double>(1, 0) * R.at<double>(1, 0));
	bool singular = sy < 1e-6; // If
	float x, y, z;
	if (!singular) {
		x = atan2(R.at<double>(2, 1), R.at<double>(2, 2));
		y = atan2(-R.at<double>(2, 0), sy);
		z = atan2(R.at<double>(1, 0), R.at<double>(0, 0));
	}
	else {
		x = atan2(-R.at<double>(1, 2), R.at<double>(1, 1));
		y = atan2(-R.at<double>(2, 0), sy);
		z = 0;
	}
	return cv::Vec3f(x, y, z);
}


int StereoCalibration::SaveImg(cv::Mat img1, cv::Mat img2) {
	// start record time
	cv::TickMeter tm;
	tm.start();
	// resize image
	cv::Size smallSize(img1.cols / 4, img1.rows / 4);
	cv::Mat smallImg1, smallImg2;
	cv::resize(img1, smallImg1, smallSize);
	cv::resize(img2, smallImg2, smallSize);
	display.create(smallImg1.rows, smallImg1.cols * 2, CV_8UC3);
	// // change to gray image
	// cv::cvtColor(img1, this->img1, cv::COLOR_BGR2GRAY);
	// cv::cvtColor(img2, this->img2, cv::COLOR_BGR2GRAY);

	cv::Rect rect(0, 0, smallImg1.cols, smallImg1.rows);
	smallImg1.copyTo(display(rect));
	rect.x += smallImg1.cols;
	smallImg2.copyTo(display(rect));
	cv::imshow("corners", display);

	cv::imwrite(cv::format("images/0/%05d.png", validFrameInd), img1);
	cv::imwrite(cv::format("images/1/%05d.png", validFrameInd), img2);
	validFrameInd++;
	cv::waitKey(1);

	return 0;
}


cv::cuda::GpuMat StereoCalibration::applyWhiteBalance(cv::cuda::GpuMat img, 
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