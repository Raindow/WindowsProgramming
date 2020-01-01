#pragma once

#include<opencv2/opencv.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/features2d.hpp>
#include<opencv2/xfeatures2d.hpp>
#include<iostream>
#include<vector>

typedef struct
{
	cv::Point2f left_top;
	cv::Point2f left_bottom;
	cv::Point2f right_top;
	cv::Point2f right_bottom;
}four_corners_t;

class Combine
{
public:
	Combine();
	~Combine();
	four_corners_t corners;
	cv::Mat normal(cv::Mat imgX, cv::Mat imgY);
	cv::Mat orb(cv::Mat imgX, cv::Mat imgY);
	cv::Mat stitch(cv::Mat imgX, cv::Mat imgY);
	cv::Mat surf(cv::Mat imgX, cv::Mat imgY);
	cv::Mat runState(cv::Mat imgX, cv::Mat imgY, int state);
	void calcCorners(const cv::Mat& H, const cv::Mat& src);
	void optimizeSeam(cv::Mat& img1, cv::Mat& trans, cv::Mat& dst);
	

};


