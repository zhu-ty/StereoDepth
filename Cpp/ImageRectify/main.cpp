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

#define GPU_MODE


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
	else if (SysUtil::getFileExtention(masterName) == "mp4" || SysUtil::getFileExtention(masterName) == "avi")
		//TODO Something is wrong with this
	{
		cv::VideoCapture vc[2];
		vc[0].open(masterName);
		vc[1].open(slaveName);

		int framesC = vc[0].get(cv::CAP_PROP_FRAME_COUNT);

		cv::Mat firstMaster;
		vc[0] >> firstMaster;
		cv::Mat firstSlave;
		vc[1] >> firstSlave;
		cv::Size originSize = firstMaster.size();
		StereoRectify sr;
		sr.init(intname, extname, originSize);
		if (!inv)
			sr.rectify(firstMaster, firstSlave);
		else
			sr.rectify(firstSlave, firstMaster);
		if (resize_origin)
		{
			cv::resize(firstMaster, firstMaster, originSize);
			cv::resize(firstSlave, firstSlave, originSize);
		}
		cv::VideoWriter vw[2];
		vw[0].open(inv ? "_slave.avi" : "_master.avi", CV_FOURCC('D', 'I', 'V', 'X'), 10, firstMaster.size());
		vw[1].open(inv ? "_master.avi" : "_slave.avi", CV_FOURCC('D', 'I', 'V', 'X'), 10, firstSlave.size());
		vw[0].write(firstMaster);
		vw[1].write(firstSlave);

		cv::imwrite(inv ? "@VideoSample_slave.png" : "@VideoSample_master.png", firstMaster);
		cv::imwrite(inv ? "@VideoSample_master.png" : "@VideoSample_slave.png", firstSlave);

		double stat = 0;
		int lastFram = 0;
		int currentTime = SysUtil::getCurrentTimeMicroSecond();
		int lastTime = currentTime;

#ifdef GPU_MODE
		cv::cuda::GpuMat tmp_g[4];
		tmp_g[0].upload(firstMaster);
		tmp_g[1].upload(firstSlave);
		tmp_g[2].create(tmp_g[0].size(), tmp_g[0].type());
		tmp_g[3].create(tmp_g[1].size(), tmp_g[1].type());
#endif // GPU_MODE
		for (int fram = 0;; fram++)
		{
			if ((double)fram / framesC > stat)
			{
				currentTime = SysUtil::getCurrentTimeMicroSecond();
				SysUtil::infoOutput(SysUtil::format("Video Rectify %d%% percent. Fps = %f", fram * 100 / framesC,
					(double)(fram - lastFram) / (currentTime - lastTime) * 1000000.0));
				stat += 0.01;
				lastFram = fram;
				lastTime = currentTime;
			}

			cv::Mat tmp[2];
			bool brk = false;
			for (int i = 0; i < 2; i++)
			{
				vc[i] >> tmp[i];
				if (tmp[i].empty())
				{
					brk = true;
					break;
				}
			}
			if (brk)
				break;
#ifdef GPU_MODE
			tmp_g[0].upload(tmp[0]);
			tmp_g[1].upload(tmp[1]);
			if (!inv)
				sr.rectify(tmp_g[0], tmp_g[2], tmp_g[1], tmp_g[3], false);
			else
				sr.rectify(tmp_g[1], tmp_g[3], tmp_g[0], tmp_g[2], false);
			tmp_g[2].download(tmp[0]);
			tmp_g[3].download(tmp[1]);
#else
			if (!inv)
				sr.rectify(tmp[0], tmp[1]);
			else
				sr.rectify(tmp[1], tmp[0]);
#endif

			for (int i = 0; i < 2; i++)
			{
				if (resize_origin)
					cv::resize(tmp[i], tmp[i], originSize);
				vw[i].write(tmp[i]);
			}
		}
		for (int i = 0; i < 2; i++)
		{
			vc[i].release();
			vw[i].release();
		}
	}
	else
	{
		SysUtil::errorOutput("Unsupported file, given extention = " + SysUtil::getFileExtention(masterName));
	}
	return 0;

}
