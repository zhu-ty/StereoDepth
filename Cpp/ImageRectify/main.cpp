/**
@brief main file 
*/

#include <cstdio>
#include <cstdlib>
#include <string>
#include <thread>
#include <memory>
#include <stdio.h>

#include <opencv2/opencv.hpp>
#include <opencv2/cudastereo.hpp>

#include "StereoRectify.h"
#include "INIReader.h"
#include "SysUtil.hpp"
#include "SKEncoder/SKEncoder.h"
#include "FFVideoReader.h"


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
	else if (SysUtil::getFileExtention(masterName) == "mp4" ||
		SysUtil::getFileExtention(masterName) == "avi")
		//TODO Something is wrong with this
	{
		FFVideoReader fvr[2];

		fvr[0].init(masterName);
		fvr[1].init(slaveName);


		int framesC = fvr[0].getFrameCount();

		cv::Mat firstMaster = fvr[0].readNext();
		cv::Mat firstSlave = fvr[1].readNext();
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
		//cv::VideoWriter vw[2];
		SKEncoder encoder[2];
		encoder[0].init(framesC, firstMaster.size(), inv ? "_slave.h265" : "_master.h265", SKEncoder::FrameType::ABGR);
		encoder[1].init(framesC, firstSlave.size(), inv ? "_master.h265" : "_slave.h265", SKEncoder::FrameType::ABGR);

		{
			cv::Mat tmp[2];
			cv::cvtColor(firstMaster, tmp[0], cv::COLOR_RGB2RGBA);
			cv::cvtColor(firstSlave, tmp[1], cv::COLOR_RGB2RGBA);
			cv::cuda::GpuMat gtmp0(tmp[0]), gtmp1(tmp[1]);
			encoder[0].encode(gtmp0.data, gtmp0.step);
			encoder[1].encode(gtmp1.data, gtmp1.step);
		}

		{
			cv::Mat tmp[2];
			cv::cvtColor(firstMaster, tmp[0], cv::COLOR_RGB2BGR);
			cv::cvtColor(firstSlave, tmp[1], cv::COLOR_RGB2BGR);
			cv::imwrite(inv ? "@VideoSample_slave.png" : "@VideoSample_master.png", tmp[0]);
			cv::imwrite(inv ? "@VideoSample_master.png" : "@VideoSample_slave.png", tmp[1]);
		}

		double stat = 0;
		int lastFram = 0;
		int currentTime = SysUtil::getCurrentTimeMicroSecond();
		int lastTime = currentTime;

		cv::cuda::GpuMat tmp_g[4];
		tmp_g[0].upload(firstMaster);
		tmp_g[1].upload(firstSlave);
		tmp_g[2].create(tmp_g[0].size(), tmp_g[0].type());
		tmp_g[3].create(tmp_g[1].size(), tmp_g[1].type());

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
			//auto st = SKCommon::getCurrentTimeMicroSecond();
			cv::Mat tmp[2];
			bool brk = false;
			for (int i = 0; i < 2; i++)
			{
				//vc[i] >> tmp[i];
				tmp[i] = fvr[i].readNext();
				if (tmp[i].empty())
				{
					brk = true;
					break;
				}
			}
			if (brk)
				break;
			//SKCommon::infoOutput("Read from video cost %d ms", (SKCommon::getCurrentTimeMicroSecond() - st) / 1000);
			//st = SKCommon::getCurrentTimeMicroSecond();
			tmp_g[0].upload(tmp[0]);
			tmp_g[1].upload(tmp[1]);
			if (!inv)
				sr.rectify(tmp_g[0], tmp_g[2], tmp_g[1], tmp_g[3], false);
			else
				sr.rectify(tmp_g[1], tmp_g[3], tmp_g[0], tmp_g[2], false);
			//SKCommon::infoOutput("Upload & rectify cost %d ms", (SKCommon::getCurrentTimeMicroSecond() - st)/1000);
			for (int i = 0; i < 2; i++)
			{
				cv::cuda::GpuMat t1, t2;
				if (resize_origin)
					cv::cuda::resize(tmp_g[i + 2], t1, originSize);
				else
					t1 = tmp_g[i + 2];
				cv::cuda::cvtColor(t1, t2, cv::COLOR_RGB2RGBA);
				encoder[i].encode(t2.data, t2.step);
			}
		}
		for (int i = 0; i < 2; i++)
		{
			//vc[i].release();
			fvr[i].release();
			//vw[i].release();
			encoder[i].endEncode();
		}
	}
	else
	{
		SysUtil::errorOutput("Unsupported file, given extention = " + SysUtil::getFileExtention(masterName));
	}
	return 0;

}
