#pragma once
#pragma execution_character_set("utf-8")
#include <QImage>
#include "Combine.h"
#include <deque>
#include <string>

class ImgProcessor
{
public:
	ImgProcessor();
	~ImgProcessor();
	cv::Mat QImage2cvMat(QImage image);
	QImage Mat2QImage(const cv::Mat& InputMat);
	QImage processing(int state, std::deque<std::string> srcs);
	QImage processing(int state, QImage img1, QImage img2);
};

