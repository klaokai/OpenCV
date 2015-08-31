#include "FGDetector.h"
/*创建函数*/
void FGDetector::Create(Mat &Source , const Size S){
#ifdef _DEBUG
	printf("正在创建前景检测组件");
#endif
}

/* 处理当前图片*/
void FGDetector::Process(const cv::Mat &){
#ifdef _DEBUG
	printf("处理当前图片……");
#endif
};

//-------------------------【VibeFGDetector::GetMask函数】---------------------------------
//  描述：获得处理后的前景图片
//-----------------------------------------------------------------------------------------------
Mat FGDetector::GetMask(){
	return m_FrImg;
}