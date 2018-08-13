#include "INIReader.h"
#include "SysUtil.hpp"

#include <opencv2/opencv.hpp>
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define HUGE_WEIGHT 100000000.0f
#define DISTANCE_MAX 50000.0f
#ifndef EPS
#define EPS 1e-3f
#endif

#define POWER_WEIGHT 1.5f

float calc_single_weight_2(int y, int x, int height, int width)
{
	float ret = 0.0f;
	float c_y = abs(y - (height/2.0f));
	float c_x = abs(x - (width/2.0f));
	if ((c_y / c_x) < ((float)height / width))
		//return 1.0f - abs(c_x / (width / 2.0f));
		ret = 1.0f - abs(c_x / (width / 2.0f));
	else ret = 1.0f - abs(c_y / (height / 2.0f));
	return pow(ret, POWER_WEIGHT);
}


float calc_single_weight(int y, int x, int height, int width)
{
	// Smaller the better (close to the center)
	float height2 = height / 2.0f;
	float width2 = width / 2.0f;
	return (y - height2) * (y - height2) + (x - width2)*(x - width2);
	//return sqrtf((y - height2) * (y - height2) + (x - width2)*(x - width2));
}

float calc_weight_ans_2(std::vector<float> weight_list, std::vector<float> &value_list)
{
	if (weight_list.size() == 0)
		return DISTANCE_MAX;
	float denominator = 0.0f;
	float ret = 0.0f;
	for (int i = 0; i < weight_list.size(); i++)
	{
		denominator += weight_list[i];
	}
	//if (denominator < EPS)
	//	return DISTANCE_MAX;
	for (int i = 0; i < weight_list.size(); i++)
	{
		if (denominator < EPS)
			ret += (1.0f / weight_list.size()) * value_list[i];
		else
			ret += weight_list[i] * value_list[i] / denominator;
	}
	return ret;
}

float calc_weight_ans(std::vector<float> weight_list, std::vector<float> &value_list)
{
	float denominator = 0.0f;
	float ret = 0.0f;
	for (int i = 0; i < weight_list.size(); i++)
	{
		if (weight_list[i] < EPS)
			return value_list[i];
		weight_list[i] = 1.0f / weight_list[i];
		denominator += weight_list[i];
	}
	for (int i = 0; i < weight_list.size(); i++)
	{
		ret += weight_list[i] * value_list[i] / denominator;
	}
	return ret;
}

//weight: bigger the heavier
int calc_img_weight_2(int h, int w, cv::Point corner, cv::Mat img_warped, cv::Mat weight_warped, float &weight, float &value)
{
	int rela_h = h - corner.y;
	int rela_w = w - corner.x;
	if (rela_h >= 0 && rela_h < img_warped.rows && rela_w >= 0 && rela_w < img_warped.cols)
	{
		weight = weight_warped.at<float>(rela_h, rela_w);
		value = img_warped.at<float>(rela_h, rela_w);
	}
	else
	{
		weight = 0.0f;
		value = -1.0f;
	}
	return 0;
}

//weight: smaller the heavier
int calc_img_weight(int h, int w,cv::Point corner, cv::Mat img_warped, cv::Mat weight_warped, float &weight, float &value)
{
	int rela_h = h - corner.y;
	int rela_w = w - corner.x;
	if (rela_h >= 0 && rela_h < img_warped.rows && rela_w >= 0 && rela_w < img_warped.cols)
	{
		weight = weight_warped.at<float>(rela_h, rela_w);
		value = img_warped.at<float>(rela_h, rela_w);
	}
	else
	{
		weight = HUGE_WEIGHT;
		value = DISTANCE_MAX;
	}
	return 0;
}

int main(int argc, char** argv)
{
	//read ini
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
	std::vector<cv::Mat> imgs(refCount), imgs_warped(refCount), imgs_averaged(refCount), weights_warped(refCount), Ks(refCount), Rs(refCount);
	std::vector<cv::Point> corner_warped(refCount);

	//init weight
	cv::Mat tmp_img = cv::imread(DepthPath[0], cv::IMREAD_ANYDEPTH);
	int height = tmp_img.rows;
	int width = tmp_img.cols;
	cv::Mat weight;
	weight.create(height, width, CV_32FC1);
	for(int i = 0;i < height;i++)
		for (int j = 0; j < width; j++)
		{
			weight.at<float>(i, j) = calc_single_weight_2(i, j, height, width);
		}

	cv::Ptr<cv::detail::SphericalWarper> w = cv::makePtr<cv::detail::SphericalWarper>(false);
	w->setScale(3000);
	//int min_x = 5000, min_y = 5000, max_x = -5000, max_y = -5000;
	for (int i = 0; i < KRpaths.size(); i++)
	{
		imgs[i] = cv::imread(DepthPath[i], cv::IMREAD_ANYDEPTH);
		cv::FileStorage fs(KRpaths[i], cv::FileStorage::READ);
		fs["K"] >> Ks[i];
		fs["R"] >> Rs[i];
		fs.release();
		
		Ks[i] = Ks[i] * ((float)imgs[i].cols / 2) / Ks[i].at<float>(0, 2); //Scale back to original size
		Ks[i].at<float>(2, 2) = 1.0f;
		cv::Mat K, R;
		Ks[i].convertTo(K, CV_32F);
		Rs[i].convertTo(R, CV_32F);
		corner_warped[i] = w->warp(imgs[i], K, R, cv::INTER_LINEAR, cv::BORDER_REPLICATE, imgs_warped[i]);
		imgs_averaged[i] = imgs_warped[i].clone();
		w->warp(weight,K,R, cv::INTER_LINEAR, cv::BORDER_REPLICATE, weights_warped[i]);
		//if (corner_warped[i].x < min_x)
		//	min_x = corner_warped[i].x;
		//if (corner_warped[i].y < min_y)
		//	min_y = corner_warped[i].y;
		//if (corner_warped[i].x + imgs_warped[i].cols > max_x)
		//	max_x = corner_warped[i].x + imgs_warped[i].cols;
		//if (corner_warped[i].y + imgs_warped[i].rows > max_y)
		//	max_y = corner_warped[i].y + imgs_warped[i].rows;

		//cv::Mat Back;
		//w->warpBackward(imgs_warped[i], K, R, cv::INTER_LINEAR, cv::BORDER_REPLICATE, cv::Size(imgs[i].cols,imgs[i].rows), Back);
	}
	printf("\n\n");

	std::vector<float> weight_list_pt, value_list_pt;
	for (int i = 0; i < refCount; i++)//SuperSuperSuperSlow in Debug mode
	{
		float percent = 0.01;
		for (int h = 0; h < imgs_warped[i].rows; h++)
		{
			for (int w = 0; w < imgs_warped[i].cols; w++)
			{
				weight_list_pt.clear();
				value_list_pt.clear();
				int global_h = h + corner_warped[i].y;
				int global_w = w + corner_warped[i].x;
				for (int j = 0; j < refCount; j++)
				{
					float weight, value;
					calc_img_weight_2(global_h, global_w, corner_warped[j], imgs_warped[j], weights_warped[j], weight, value);
					if (value > 0.0f)
					{
						weight_list_pt.push_back(weight);
						value_list_pt.push_back(value);
					}
				}
				imgs_averaged[i].at<float>(h, w) = calc_weight_ans_2(weight_list_pt, value_list_pt);
			}
			if (((float)h) / ((float)imgs_warped[i].rows) > percent)
			{
				printf("\r%.3f%%", percent * 100);
				percent = percent + 0.01;
			}
		}
		printf("\n");
	}

	for (int i = 0; i < refCount; i++)
	{
		
		cv::Mat K, R;
		Ks[i].convertTo(K, CV_32F);
		Rs[i].convertTo(R, CV_32F);
		cv::Mat Back;
		w->warpBackward(imgs_averaged[i], K, R, cv::INTER_LINEAR, cv::BORDER_REPLICATE, cv::Size(imgs[i].cols,imgs[i].rows), Back);
		cv::Mat Back16U;
		Back.convertTo(Back16U, CV_16UC1);
		cv::imwrite(OutputDepthPath[i], Back16U);
		//cv::imwrite(OutputDepthPath[i], Back);//TODO : opencv can't save 32bit float tiff, fuck
	}


    return 0;
}