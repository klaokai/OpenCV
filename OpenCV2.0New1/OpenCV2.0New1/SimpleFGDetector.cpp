#include <iostream>
using namespace std;

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

#include "SimpleFGDetector.h"
const int GrayScale=256;

Histogram1D::Histogram1D(){
	//准备1D直方图的参数
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0;//默认情况，考察0号通道
}

Mat Histogram1D::getHistogram(const Mat &image){
	//计算直方图
	calcHist(&image ,
		1 ,//计算单张图像的直方图
		channels ,//通道数量
		Mat() ,//不使用图像作为掩码
		hist ,//返回的直方图
		1 ,//这是1D的直方图
		histSize ,//项的数量
		ranges//像素值范围
		);
	return hist;
}

void Histogram1D::drawHist(){
	namedWindow("Histogram Demo");
	// 创建直方图画布
	int hist_w = 400; int hist_h = 400;
	int histSize = 255;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_w , hist_h , hist.depth());
	Mat temp;
	/// 将直方图归一化到范围 [ 0, histImage.rows ]
	normalize(hist , temp , 0 , histImage.rows , NORM_MINMAX , -1 , Mat());

	for ( int i = 1; i < histSize; i++ )
	{
		line(histImage , Point(bin_w*(i - 1) , hist_h - cvRound(temp.at<float>(i - 1))) ,
			Point(bin_w*(i) , hist_h - cvRound(temp.at<float>(i))) ,
			Scalar(255 , 255 , 255) , 2 , 8 , 0);
	}
	cv::imshow("Histogram Demo" , histImage);
}

//-------------------------【SimpleDetector::Create函数】---------------------------------
//  描述：准备3个观察窗口，并准备原始图像的拷贝
//-----------------------------------------------------------------------------------------------
void SimpleFGDetector::Create(Mat &Source,const Size S){
	namedWindow("背景", 1);
	moveWindow("背景", 330,0);

	namedWindow("前景", 1);
	moveWindow("前景", 660,0);

	m_SourceImg=Source;
	m_SourceSize=S;
	Otsuthreshold=60;
	frameNum=0;

	m_BkImg.create(m_SourceSize,CV_8UC1);
	m_FrImg.create(m_SourceSize,CV_8UC1);

	pFrameMat.create(m_SourceSize,CV_32FC1);
	pBKfirstMat.create(m_SourceSize,CV_32FC1);
	pBkMat.create(m_SourceSize,CV_32FC1);
	pFrMat.create(m_SourceSize,CV_32FC1);

	cvtColor(m_SourceImg, m_BkImg, CV_BGR2GRAY);
	cvtColor(m_SourceImg, m_FrImg, CV_BGR2GRAY);	

	m_FrImg.convertTo(pFrameMat,pFrameMat.type());
	m_FrImg.convertTo(pBKfirstMat,pBKfirstMat.type());
	m_FrImg.convertTo(pFrMat,pFrMat.type());
	m_FrImg.convertTo(pBkMat,pBkMat.type());
}
//-------------------------【SimpleFGDetector::Otsu函数】---------------------------------
//  描述：Otsu大津法自适应阈值
//----------------------------------------------------------------------------------
void SimpleFGDetector::Otsu(Mat & frame){
	assert(frame.channels() == 1);

	//统计每个灰度级中像素的个数
	Mat histogram(Hist.getHistogram(frame));

	int totalpixel = m_SourceSize.width *m_SourceSize.height;

	//计算每个灰度级的像素数目占整幅图像的比例
	/// 将直方图归一化到范围 [ 0, totalpixel ]
	normalize(histogram, histogram, 0, totalpixel, NORM_MINMAX, -1, Mat() );//归一化直方图

	float maxavgvalue = 0;

	for(int i=0;i<256;i++)
		maxavgvalue = i*histogram.at<float>(i);//总的灰度均值，其实在这里可将其设为0

	float PA = 0;//A类出现概率
	float PB = 0;//B类出现概率
	float Agrayvalue = 0;//A类灰度均值
	float Bgrayvalue = 0;//B类灰度均值
	float maxvariance = 0;
	float variance ;

	for(int i=0;i<256;i++)
	{
		PA += histogram.at<float>(i);
		PB = 1-PA;
		Agrayvalue += i*histogram.at<float>(i);//A类灰度均值
		Bgrayvalue += maxavgvalue - Agrayvalue;//B类灰度均值
		//Agrayvalue = Agrayvalue/PA; 
		//Bgrayvalue = Bgrayvalue/PB;
		variance = PA*(Agrayvalue-maxavgvalue)*(Agrayvalue-maxavgvalue)+
			PB*(Bgrayvalue-maxavgvalue)*(Bgrayvalue-maxavgvalue);//计算类间方差


		if(variance>maxvariance)
		{
			maxvariance = variance;
			Otsuthreshold = i;//求得最大类间方差时的像素值，即为最佳阈值
		}
	}
}
//-------------------------【SimpleFGDetector::RenewBKImage函数】---------------------------------
//  描述：选择性更新背景
//-------------------------------------------------------------------------------------
void SimpleFGDetector::RenewBKImage(){
	int i,j;
	int alpha= 0.008;
	Mat_<uchar>
		T_m_FrImg=m_FrImg,
		T_pBkMat=pBkMat,
		T_pFrameMat=pFrameMat;
	for(i=0 ;i<m_SourceSize.height ; i++)  
		for(j=0 ; j<m_SourceSize.width ;j++)  
		{  
			if(T_m_FrImg(i,j)==0)  
			{  
				T_pBkMat(i,j) =  T_pFrameMat(i,j)*alpha + (1-alpha)*T_pBkMat(i,j) ;
			}  
			else  
				T_pBkMat(i,j)  = T_pFrameMat(i,j);
		}

}
//-------------------------【SimpleFGDetector::Process函数】---------------------------------
//  描述：一般的前景处理，需要在此过程中获得背景图案
//------------------------------------------------------------------------------------
void SimpleFGDetector::Process(const cv::Mat &pImg){
	m_SourceImg=pImg;
	//***************前景处理过程***************************************
	//灰度处理
	cvtColor(m_SourceImg, m_FrImg, CV_BGR2GRAY);

	//Otsu(m_FrImg);
	Hist.getHistogram(m_FrImg);

	Hist.drawHist();

	//mat类型转换
	m_FrImg.convertTo(pFrameMat,pFrameMat.type());
	//高斯滤波先，以平滑图像
	GaussianBlur(pFrameMat, pFrameMat, Size(5,5), 3, 0, 0);

	if(frameNum<50){
		addWeighted(pBKfirstMat,1,pFrameMat,0.02,0.0,pBKfirstMat);
	}
	if(frameNum==50){
		pBKfirstMat.copyTo(pBkMat);
	}

	//当前帧跟背景图相减,absdiff计算两个数组差的绝对值
	absdiff(pFrameMat, pBkMat, pFrMat);

	//二值化前景图
	threshold(pFrMat, m_FrImg, Otsuthreshold, 255.0, CV_THRESH_BINARY);

	//显示
	pBkMat.convertTo(m_BkImg,m_BkImg.type());

	erode(m_FrImg, m_FrImg,cv::Mat());//腐蚀，迭代次数
	dilate(m_FrImg, m_FrImg,cv::Mat());//再膨胀，迭代次数

	imshow("前景", m_FrImg);
	imshow("背景", m_BkImg);

	//更新背景
	//pBkMat=(1-0.005)*pBkMat+0.005*pFrameMat;
	if(frameNum>50){
		RenewBKImage();
	}
	frameNum++;
}

//-------------------------【SimpleFGDetector::GetMask函数】---------------------------------
//  描述：获得处理后的前景图片
//-----------------------------------------------------------------------------------------------
//Mat SimpleFGDetector::GetMask(){
//	return m_FrImg.clone();
//}