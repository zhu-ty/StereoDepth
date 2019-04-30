/**
@brief FFVideoReader, using ffmpeg to read videos
@author zhu-ty
@date Apr 30, 2019
*/

#ifndef __IMAGE_RECTIFY_FF_VIDEO_READER__
#define __IMAGE_RECTIFY_FF_VIDEO_READER__

#include <stdio.h>

#include <opencv2/opencv.hpp>

class FFVideoReader
{
public:
	~FFVideoReader() {};
	FFVideoReader() {};

	int init(std::string fileName);
	int getFrameCount();
	cv::Mat readNext();
	int release();
private:
	std::string _fileName;
	FILE *_pipe;
	cv::Size _sz;
	int _fCount;
	int _fRead;
};

#endif //__IMAGE_RECTIFY_FF_VIDEO_READER__