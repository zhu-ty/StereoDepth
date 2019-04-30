#include "FFVideoReader.h"
#include "SKCommon.hpp"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

int FFVideoReader::init(std::string fileName)
{
	_fileName = fileName;

	cv::VideoCapture vc;
	if (vc.open(_fileName) == false)
	{
		SKCommon::errorOutput("opening file name %s error!", _fileName.c_str());
		return -1;
	}
	cv::Mat tmp;
	vc >> tmp;
	_sz = tmp.size();
	_fCount = vc.get(cv::CAP_PROP_FRAME_COUNT);
	vc.release();

	FILE *tmp_p = popen((std::string("ffmpeg -i ") + _fileName + " -loglevel quiet -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -").c_str(), "rb");
	int c = fread(tmp.data, 1, _sz.width * _sz.height * 3, tmp_p);
	if (c < _sz.width * _sz.height * 3)
	{
		SKCommon::errorOutput("You dont have ffmpeg.exe in your path to run this lib, error, check.");
		return -2;
	}
	pclose(tmp_p);
	_pipe = popen((std::string("ffmpeg -hwaccel auto -i ") + _fileName + " -loglevel quiet -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -").c_str(), "rb");
	_fRead = 0;
	SKCommon::infoOutput("FFVideoReader opened video file %s", _fileName.c_str());
	return 0;
}

int FFVideoReader::getFrameCount()
{
	return _fCount;
}

cv::Mat FFVideoReader::readNext()
{
	if (_fRead == _fCount)
	{
		SKCommon::warningOutput("Video has been read out, returning empty mat.");
		return cv::Mat();
	}
	cv::Mat ret(_sz, CV_8UC3);
	fread(ret.data, 1, _sz.width * _sz.height * 3, _pipe);
	_fRead++;
	return ret;
}

int FFVideoReader::release()
{
	pclose(_pipe);
	return 0;
}
