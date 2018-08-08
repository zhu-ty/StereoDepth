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
#include "INIReader.h"
#include "SysUtil.hpp"

int old_version(int argc, char* argv[])
{
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


int main(int argc, char* argv[]) 
{
	std::string path = "./IRconfig.ini";
	if (argc > 1)
		path = argv[1];
	INIReader reader(path);
	if (reader.ParseError() < 0)
	{
		SysUtil::errorOutput(std::string("ImageRectify::load_config Can't load :") + path);
		return -1;
	}
	std::string dir = reader.Get("ImageRectify", "dir", ".");
	std::string name1 = reader.Get("ImageRectify", "img0", "0.jpg");
	std::string name2 = reader.Get("ImageRectify", "img1", "1.jpg");
	std::string outname1 = reader.Get("ImageRectify", "outImg0", "Rectify0.jpg");
	std::string outname2 = reader.Get("ImageRectify", "outImg1", "Rectify1.jpg");
	std::string intname = reader.Get("ImageRectify", "intrinsics", "intrinsics.yml"); 
	std::string extname = reader.Get("ImageRectify", "extrinsics", "extrinsics.yml"); 
	bool inv = reader.GetBoolean("ImageRectify", "inv", false);
	bool resize_origin = reader.GetBoolean("ImageRectify", "resize", true);


	name1 = cv::format("%s/%s", dir.c_str(), name1.c_str());
	name2 = cv::format("%s/%s", dir.c_str(), name2.c_str());
	intname = cv::format("%s/%s", dir.c_str(), intname.c_str());
	extname = cv::format("%s/%s", dir.c_str(), extname.c_str());
	outname1 = cv::format("%s/%s", dir.c_str(), outname1.c_str());
	outname2 = cv::format("%s/%s", dir.c_str(), outname2.c_str());



	if (inv)
	{
		std::string tmp = name1;
		name1 = name2;
		name2 = tmp;
	}

	int width, height;
	cv::Mat img0 = cv::imread(name1);
	cv::Mat img1 = cv::imread(name2);

	width = img0.cols;
	height = img0.rows;
	cv::Size imgsize(width, height);
	StereoRectify sr;
	
	sr.init(intname, extname, imgsize);
	sr.rectify(img0, img1);

	if (resize_origin)
	{
		cv::resize(img0, img0, imgsize);
		cv::resize(img1, img1, imgsize);
	}



	if (inv)
	{
		cv::imwrite(outname1, img1);
		cv::imwrite(outname2, img0);
	}
	else
	{
		cv::imwrite(outname1, img0);
		cv::imwrite(outname2, img1);
	}

	return 0;

}
