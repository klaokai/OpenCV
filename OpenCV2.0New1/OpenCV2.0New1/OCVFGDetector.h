#pragma once
#include "FGDetector.h"

#include "opencv2/video/background_segm.hpp"
using namespace cv;

class OCVFGDetector :
	public FGDetector
{
	bool update_bg_model;
	BackgroundSubtractorMOG2 bg_model;//(100, 3, 0.3, 5);
	Mat fgimg;
public:
	OCVFGDetector();
	~OCVFGDetector();

public:
	/*创建函数*/
	virtual void Create(Mat &Source , const Size S);
	/* 处理当前图片*/
	virtual void Process(const cv::Mat &);
};

