#include "INIReader.h"
#include "SysUtil.hpp"

#include <opencv2/opencv.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

int main(int argc, char** argv)
{
	std::string path = "./DWconfig.ini";
	if (argc > 1)
	{
		path = argv[1];
	}
	INIReader reader(path);
	if (reader.ParseError() < 0)
	{
		SysUtil::errorOutput(std::string("ImageRectify::load_config Can't load :") + path);
		return -1;
	}
	int refCount = reader.GetInteger("DisparityWarping", "refCount", 0);
	std::string dir = reader.Get("DisparityWarping", "dir", "./");
	
	std::vector<std::string> KRpaths(refCount);
	std::vector<std::string> DepthPath(refCount);
	std::vector<std::string> OutputDepthPath(refCount);

	for (int i = 0; i < KRpaths.size(); i++)
	{
		KRpaths[i] = reader.Get("DisparityWarping", SysUtil::format("KRpath%d", i), "kr.yml");
		KRpaths[i] = SysUtil::format("%s/%s", dir.c_str(), KRpaths[i].c_str());
		DepthPath[i] = reader.Get("DisparityWarping", SysUtil::format("DepthPath%d", i), "depth.tiff");
		DepthPath[i] = SysUtil::format("%s/%s", dir.c_str(), DepthPath[i].c_str());
		OutputDepthPath[i] = reader.Get("DisparityWarping", SysUtil::format("OutputDepthPath%d", i), "outputdepth.tiff");
		OutputDepthPath[i] = SysUtil::format("%s/%s", dir.c_str(), OutputDepthPath[i].c_str());
	}
	std::vector<cv::Mat> imgs(refCount), imgs_warped(refCount), Ks(refCount), Rs(refCount);
	std::vector<cv::Point> corner_warpd(refCount);
	for (int i = 0; i < KRpaths.size(); i++)
	{
		imgs[i] = cv::imread(DepthPath[i]);
		cv::FileStorage fs(KRpaths[i], cv::FileStorage::READ);
		fs["K"] >> Ks[i];
		fs["R"] >> Rs[i];
		fs.release();
		cv::Ptr<cv::detail::SphericalWarper> w = cv::makePtr<cv::detail::SphericalWarper>(false);
		Ks[i] = Ks[i] * (float)imgs[i].cols / Ks[i].at<float>(0, 2); //Scale back to original size
		w->setScale(Ks[i].at<float>(0, 0));

		cv::Mat K, R;
		Ks[i].convertTo(K, CV_32F);
		Rs[i].convertTo(R, CV_32F);
		corner_warpd[i] = w->warp(imgs[i], K, R, cv::INTER_LINEAR, cv::BORDER_CONSTANT, imgs_warped[i]);
	}

    return 0;
}