#include "FGDetector.h"
/*��������*/
void FGDetector::Create(Mat &Source , const Size S){
#ifdef _DEBUG
	printf("���ڴ���ǰ��������");
#endif
}

/* ����ǰͼƬ*/
void FGDetector::Process(const cv::Mat &){
#ifdef _DEBUG
	printf("����ǰͼƬ����");
#endif
};

//-------------------------��VibeFGDetector::GetMask������---------------------------------
//  ��������ô�����ǰ��ͼƬ
//-----------------------------------------------------------------------------------------------
Mat FGDetector::GetMask(){
	return m_FrImg;
}