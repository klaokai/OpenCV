#ifndef  SIMPLEFGDETECTOR_H
#define  SIMPLEFGDETECTOR_H

#include "FGDetector.h"

class Histogram1D{
private:
	int histSize[1];//�������
	float hranges[2];//���ص���С�����ֵ
	const float*ranges[1];
	int channels[1];//����һ��ͨ��
	Mat hist;
public:

	Histogram1D();

	Mat getHistogram(const Mat &image);
	void drawHist();
};



class SimpleFGDetector:public FGDetector{
private:
	Mat m_BkImg;//����ͼƬ��ֻ�г���������������壩
	
	//������Ϊ�˸���ȷ��ͼ��������׼����
	//�ֱ��Ӧ�����ĸ�����
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
	/*��������*/
	virtual void Create(Mat &Source,const Size S);
	/* ����ǰͼƬ*/
	virtual void Process(const cv::Mat &);
};
#endif