#include<vector>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

#include "CarInfo.h"
#include "FGDetectorFactory.h"
#include "BlobDetector.h"


class Controller{
private:
	//--------------算法类--------------------
	FGDetector *FGD;
	BlobDetector BD;

	//--------------数据----------------------
	VideoCapture SourceCap;
	double rate;
	Size S;
	
	int nFrmNum;//运行时的当前帧

	//处理完成的前景
	//Mat pFrImg;
	
	//------------函数------------------
public:
	//主要函数
	Controller(const VideoCapture& pCap);
	void Process(/*VideoCapture& pCap*/);
};