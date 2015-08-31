#ifndef  FGDETECTOR_H
#define  FGDETECTOR_H

#include "opencv2/imgproc/imgproc.hpp" 
using namespace cv;

class FGDetector{
protected:
	Mat m_SourceImg;//ԴͼƬ
	Mat m_FrImg;//ǰ��ͼƬ��ֻ�г�����

	Size m_SourceSize;//Դͼ���С
	unsigned int frameNum;//���ڵڼ�֡

public:
	/*��������*/
	virtual void Create(Mat &Source , const Size S);
	/* ����ǰͼƬ*/
	virtual void Process(const cv::Mat &);
	/* ��ȡ������ǰ��ͼƬ*/
	Mat GetMask();
};

#endif