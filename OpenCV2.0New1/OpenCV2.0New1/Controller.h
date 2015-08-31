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
	//--------------�㷨��--------------------
	FGDetector *FGD;
	BlobDetector BD;

	//--------------����----------------------
	VideoCapture SourceCap;
	double rate;
	Size S;
	
	int nFrmNum;//����ʱ�ĵ�ǰ֡

	//������ɵ�ǰ��
	//Mat pFrImg;
	
	//------------����------------------
public:
	//��Ҫ����
	Controller(const VideoCapture& pCap);
	void Process(/*VideoCapture& pCap*/);
};