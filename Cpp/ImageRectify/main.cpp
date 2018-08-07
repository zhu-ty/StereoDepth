/**
@brief main file 
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <memory>

#include <opencv2/opencv.hpp>
#include <opencv2/cudastereo.hpp>

#include "StereoRectify.h"

int main2(int argc, char* argv[])
{
	//std::string dir = std::string("E:/data/stereo/No2");
	//std::string name1 = cv::format("%s/CUCAU1731016_03_00010.jpg", dir.c_str());
	//std::string name2 = cv::format("%s/CUCAU1731034_02_00010.jpg", dir.c_str());
	//bool inv = true;

	//std::string dir = std::string("E:/data/stereo/No1");
	//std::string name1 = cv::format("%s/CUCAU1731012_00_00010.jpg", dir.c_str());
	//std::string name2 = cv::format("%s/CUCAU1731023_01_00010.jpg", dir.c_str());
	//bool inv = true;

	std::string dir = std::string("E:/data/stereo/test");
	std::string name1 = cv::format("%s/CICAU1641057_00_00000.jpg", dir.c_str());
	std::string name2 = cv::format("%s/CICAU1641089_01_00000.jpg", dir.c_str());
	bool inv = false;


	if (inv)
	{
		std::string tmp = name1;
		name1 = name2;
		name2 = tmp;
	}
	std::string leftoutname = cv::format("%s/rectify0.jpg", dir.c_str());
	std::string rightoutname = cv::format("%s/rectify1.jpg", dir.c_str());

	int width, height;
	cv::Mat left = cv::imread(name1);
	cv::Mat right = cv::imread(name2);
	width = left.cols;
	height = left.rows;
	cv::Size imgsize(width, height);
	StereoRectify sr;
	std::string intname = cv::format("%s/intrinsics.yml", dir.c_str());
	std::string extname = cv::format("%s/extrinsics.yml", dir.c_str());
	sr.init(intname, extname, imgsize);
	sr.rectify(left, right);

	if (inv)
	{
		cv::imwrite(leftoutname, right);
		cv::imwrite(rightoutname, left);
	}
	else
	{
		cv::imwrite(leftoutname, left);
		cv::imwrite(rightoutname, right);
	}

	return 0;
}


int main(int argc, char* argv[]) {
	main2(argc, argv);
	return 0;
	std::string dir = std::string(argv[1]);
	std::string leftname = cv::format("%s/%s", dir.c_str(), argv[2]);
	std::string rightname = cv::format("%s/%s", dir.c_str(), argv[3]);
	std::string leftoutname = cv::format("%s/%s", dir.c_str(), argv[4]);
	std::string rightoutname = cv::format("%s/%s", dir.c_str(), argv[5]);


	int width, height, frame_count;
	cv::Mat left, right;
	right = cv::imread(leftname);
	left = cv::imread(rightname);
	cv::Size imgsize = left.size();
	
	StereoRectify sr;
	std::string intname = cv::format("%s/intrinsics.yml", dir.c_str());
	std::string extname = cv::format("%s/extrinsics.yml", dir.c_str());
	sr.init(intname, extname, imgsize);

	sr.rectify(left, right);
	cv::imwrite(leftoutname, left);
	cv::imwrite(rightoutname, right);

	return 0;
}
