#ifndef  SIMPLEFGDETECTOR_H
#define  SIMPLEFGDETECTOR_H

#include "FGDetector.h"

class Histogram1D{
private:
	int histSize[1];//项的数量
	float hranges[2];//像素的最小及最大值
	const float*ranges[1];
	int channels[1];//仅用一个通道
	Mat hist;
public:

	Histogram1D();

	Mat getHistogram(const Mat &image);
	void drawHist();
};



class SimpleFGDetector:public FGDetector{
private:
	Mat m_BkImg;//背景图片（只有车辆以外的其他物体）
	
	//下面是为了更精确的图像处理计算而准备的
	//分别对应上面四个数据
	Mat pFrameMat;
	Mat pBKfirstMat;
	Mat	pBkMat;
	Mat	pFrMat;

	int Otsuthreshold;
	Histogram1D Hist;

private:
	void Otsu(Mat & frame);
	void RenewBKImage();

public:
	/*创建函数*/
	virtual void Create(Mat &Source,const Size S);
	/* 处理当前图片*/
	virtual void Process(const cv::Mat &);
};
#endif