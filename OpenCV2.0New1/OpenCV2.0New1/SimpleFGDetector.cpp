#include <iostream>
using namespace std;

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

#include "SimpleFGDetector.h"
const int GrayScale=256;

Histogram1D::Histogram1D(){
	//׼��1Dֱ��ͼ�Ĳ���
	histSize[0] = 256;
	hranges[0] = 0.0;
	hranges[1] = 255.0;
	ranges[0] = hranges;
	channels[0] = 0;//Ĭ�����������0��ͨ��
}

Mat Histogram1D::getHistogram(const Mat &image){
	//����ֱ��ͼ
	calcHist(&image ,
		1 ,//���㵥��ͼ���ֱ��ͼ
		channels ,//ͨ������
		Mat() ,//��ʹ��ͼ����Ϊ����
		hist ,//���ص�ֱ��ͼ
		1 ,//����1D��ֱ��ͼ
		histSize ,//�������
		ranges//����ֵ��Χ
		);
	return hist;
}

void Histogram1D::drawHist(){
	namedWindow("Histogram Demo");
	// ����ֱ��ͼ����
	int hist_w = 400; int hist_h = 400;
	int histSize = 255;
	int bin_w = cvRound((double)hist_w / histSize);
	Mat histImage(hist_w , hist_h , hist.depth());
	Mat temp;
	/// ��ֱ��ͼ��һ������Χ [ 0, histImage.rows ]
	normalize(hist , temp , 0 , histImage.rows , NORM_MINMAX , -1 , Mat());

	for ( int i = 1; i < histSize; i++ )
	{
		line(histImage , Point(bin_w*(i - 1) , hist_h - cvRound(temp.at<float>(i - 1))) ,
			Point(bin_w*(i) , hist_h - cvRound(temp.at<float>(i))) ,
			Scalar(255 , 255 , 255) , 2 , 8 , 0);
	}
	cv::imshow("Histogram Demo" , histImage);
}

//-------------------------��SimpleDetector::Create������---------------------------------
//  ������׼��3���۲촰�ڣ���׼��ԭʼͼ��Ŀ���
//-----------------------------------------------------------------------------------------------
void SimpleFGDetector::Create(Mat &Source,const Size S){
	namedWindow("����", 1);
	moveWindow("����", 330,0);

	namedWindow("ǰ��", 1);
	moveWindow("ǰ��", 660,0);

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
//-------------------------��SimpleFGDetector::Otsu������---------------------------------
//  ������Otsu�������Ӧ��ֵ
//----------------------------------------------------------------------------------
void SimpleFGDetector::Otsu(Mat & frame){
	assert(frame.channels() == 1);

	//ͳ��ÿ���Ҷȼ������صĸ���
	Mat histogram(Hist.getHistogram(frame));

	int totalpixel = m_SourceSize.width *m_SourceSize.height;

	//����ÿ���Ҷȼ���������Ŀռ����ͼ��ı���
	/// ��ֱ��ͼ��һ������Χ [ 0, totalpixel ]
	normalize(histogram, histogram, 0, totalpixel, NORM_MINMAX, -1, Mat() );//��һ��ֱ��ͼ

	float maxavgvalue = 0;

	for(int i=0;i<256;i++)
		maxavgvalue = i*histogram.at<float>(i);//�ܵĻҶȾ�ֵ����ʵ������ɽ�����Ϊ0

	float PA = 0;//A����ָ���
	float PB = 0;//B����ָ���
	float Agrayvalue = 0;//A��ҶȾ�ֵ
	float Bgrayvalue = 0;//B��ҶȾ�ֵ
	float maxvariance = 0;
	float variance ;

	for(int i=0;i<256;i++)
	{
		PA += histogram.at<float>(i);
		PB = 1-PA;
		Agrayvalue += i*histogram.at<float>(i);//A��ҶȾ�ֵ
		Bgrayvalue += maxavgvalue - Agrayvalue;//B��ҶȾ�ֵ
		//Agrayvalue = Agrayvalue/PA; 
		//Bgrayvalue = Bgrayvalue/PB;
		variance = PA*(Agrayvalue-maxavgvalue)*(Agrayvalue-maxavgvalue)+
			PB*(Bgrayvalue-maxavgvalue)*(Bgrayvalue-maxavgvalue);//������䷽��


		if(variance>maxvariance)
		{
			maxvariance = variance;
			Otsuthreshold = i;//��������䷽��ʱ������ֵ����Ϊ�����ֵ
		}
	}
}
//-------------------------��SimpleFGDetector::RenewBKImage������---------------------------------
//  ������ѡ���Ը��±���
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
//-------------------------��SimpleFGDetector::Process������---------------------------------
//  ������һ���ǰ��������Ҫ�ڴ˹����л�ñ���ͼ��
//------------------------------------------------------------------------------------
void SimpleFGDetector::Process(const cv::Mat &pImg){
	m_SourceImg=pImg;
	//***************ǰ���������***************************************
	//�Ҷȴ���
	cvtColor(m_SourceImg, m_FrImg, CV_BGR2GRAY);

	//Otsu(m_FrImg);
	Hist.getHistogram(m_FrImg);

	Hist.drawHist();

	//mat����ת��
	m_FrImg.convertTo(pFrameMat,pFrameMat.type());
	//��˹�˲��ȣ���ƽ��ͼ��
	GaussianBlur(pFrameMat, pFrameMat, Size(5,5), 3, 0, 0);

	if(frameNum<50){
		addWeighted(pBKfirstMat,1,pFrameMat,0.02,0.0,pBKfirstMat);
	}
	if(frameNum==50){
		pBKfirstMat.copyTo(pBkMat);
	}

	//��ǰ֡������ͼ���,absdiff�������������ľ���ֵ
	absdiff(pFrameMat, pBkMat, pFrMat);

	//��ֵ��ǰ��ͼ
	threshold(pFrMat, m_FrImg, Otsuthreshold, 255.0, CV_THRESH_BINARY);

	//��ʾ
	pBkMat.convertTo(m_BkImg,m_BkImg.type());

	erode(m_FrImg, m_FrImg,cv::Mat());//��ʴ����������
	dilate(m_FrImg, m_FrImg,cv::Mat());//�����ͣ���������

	imshow("ǰ��", m_FrImg);
	imshow("����", m_BkImg);

	//���±���
	//pBkMat=(1-0.005)*pBkMat+0.005*pFrameMat;
	if(frameNum>50){
		RenewBKImage();
	}
	frameNum++;
}

//-------------------------��SimpleFGDetector::GetMask������---------------------------------
//  ��������ô�����ǰ��ͼƬ
//-----------------------------------------------------------------------------------------------
//Mat SimpleFGDetector::GetMask(){
//	return m_FrImg.clone();
//}