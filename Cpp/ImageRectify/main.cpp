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
	std::string version_test = reader.Get("ImageRectify", "img0", "");
	if (version_test != "")
	{
		SysUtil::errorOutput("Seems like you are using old version of IRConfig.ini config file with a new verison of ImageRecitify.exe, please change to new version of INI.");
		SysUtil::infoOutput("Check Here: https://github.com/zhu-ty/StereoDepth/blob/master/Cpp/ImageRectify/IRconfig.Sample.ini");
		return -1;
	}

	std::string dir = reader.Get("ImageRectify", "dir", ".");
	std::string intname = reader.Get("ImageRectify", "intrinsics", "intrinsics.yml");
	intname = dir + "/" + intname;
	std::string extname = reader.Get("ImageRectify", "extrinsics", "extrinsics.yml");
	extname = dir + "/" + extname;
	std::string masterName = reader.Get("ImageRectify", "Master", "0.jpg");
	masterName = dir + "/" + masterName;
	std::string slaveName = reader.Get("ImageRectify", "Slave", "0.jpg");
	slaveName = dir + "/" + slaveName;
	bool inv = reader.GetBoolean("ImageRectify", "inv", false);
	bool resize_origin = reader.GetBoolean("ImageRectify", "resize", true);

	if (SysUtil::getFileExtention(masterName) == "jpg" || SysUtil::getFileExtention(masterName) == "png")
	{
		cv::Mat master = cv::imread(masterName);
		cv::Mat slave = cv::imread(slaveName);
		cv::Size originSize = master.size();
		StereoRectify sr;
		sr.init(intname, extname, originSize);
		if (!inv)
			sr.rectify(master, slave);
		else
			sr.rectify(slave, master);
		if (resize_origin)
		{
			cv::resize(master, master, originSize);
			cv::resize(slave, slave, originSize);
		}
		cv::imwrite(inv ? "_slave.png" : "_master.png", master);
		cv::imwrite(inv ? "_master.png" : "_slave.png", slave);
	}
	else
	{
		SysUtil::errorOutput("Unsupported file, given extention = " + SysUtil::getFileExtention(masterName));
	}
	return 0;

}
