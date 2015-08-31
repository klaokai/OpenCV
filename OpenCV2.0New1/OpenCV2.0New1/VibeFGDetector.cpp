#include <iostream>
using namespace std;

#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace cv;

#include "VibeFGDetector.h"

const int WINSIZE=1;

int GetRandom()  
{
	int val = VibeFGDetector::numModel* 1.0 * rand() / RAND_MAX;
	if( val == VibeFGDetector::numModel )  
		return val - 1;
	else  
		return val;
}  

int GetRandom(int random)
{
	int val=random-WINSIZE+rand()%(2*WINSIZE);
	if (val<random-WINSIZE)
	{
		val=random-WINSIZE;
	}
	if (val>random+WINSIZE)
	{
		val=random+WINSIZE;
	}
	return val;
}

//从一个区间里面随机
int GetRandom(int istart,int iend)  
{  
	int val=istart+rand()%(iend-istart);
	return val;
}  


//-------------------------【VibeFGDetector::Create函数】---------------------------------
//  描述：准备3个观察窗口，并准备原始图像的拷贝
//-----------------------------------------------------------------------------------------------
const unsigned int VibeFGDetector::numModel=20;
const unsigned int VibeFGDetector::minCount=2;
const unsigned int VibeFGDetector::radius=20;

void VibeFGDetector::Create(Mat &Source,const Size S)
{
	//namedWindow("原视频",1);
	//moveWindow("原视频",0,0);

	namedWindow("灰度图", 1);
	moveWindow("灰度图", 330,0);

	namedWindow("前景", 1);
	moveWindow("前景", 660,0);

	m_SourceImg=Source;
	m_SourceSize=S;

	m_FrImg.create(m_SourceSize,CV_8UC1);

	cvtColor(m_SourceImg, grayImg, CV_BGR2GRAY);

	/* =============================初始化============================== */
	//建立一个对应为20幅宽高和视频一样图像
	/*  以下实现第一帧在每个像素的8邻域内的采样功能，建立对应的背景模型*******/  
	backModel=new unsigned char**[numModel];

	for(int k=0;k<numModel;k++)
	{
		backModel[k]=new unsigned char*[m_SourceSize.height];
		for(int i=0;i<m_SourceSize.height;i++)
		{
			backModel[k][i]=new unsigned char[m_SourceSize.width];

			for(int j=0;j<m_SourceSize.width;j++)
				backModel[k][i][j]=0;
		}
	}
	
	


	
	// 采样进行背景建模s
	int pixel;
	int ri=0,rj=0;//随机采样数值
	for (int i=WINSIZE;i<m_SourceSize.height-WINSIZE;i++)	{
		for(int j=WINSIZE;j<m_SourceSize.width-WINSIZE;j++){
			//在3*3邻域中进行采样
			for(int k=0;k<numModel;k++)
			{
				ri=GetRandom(i);
				rj=GetRandom(j);
				pixel=grayImg.at<uchar>(ri,rj);//获取原图像灰度图像素值
				backModel[k][i][j]=pixel;//设置20幅图像的随机点
			}
		}
	}


	//初始化前景点掩膜的生命长度 
	runningTime=new int* [m_SourceSize.height];
	for(int i=0;i<m_SourceSize.height;i++)
	{
		runningTime[i]=new int [m_SourceSize.width];
		for(int j=0;j<m_SourceSize.width;j++)
			runningTime[i][j]=0;
	}
}


//-------------------------【VibeFGDetector::Process函数】---------------------------------
//  描述：一般的前景处理，需要在此过程中获得背景图案
//-----------------------------------------------------------------------------------------
void VibeFGDetector::Process(const cv::Mat &pImg)
{
	m_SourceImg=pImg;
	//imshow("原视频",m_SourceImg);
	m_FrImg.setTo(Scalar::all(0));
	double pixelModel,pixelCurrent,pixelDiff;
	int R1,R2;//产生随机数
	int Ri,Rj;//产生邻域内X和Y的随机数
	int i,j,k;
	int count=0;//匹配符合次数,距离比较在阈值内的次数
	//***************前景处理过程***************************************
	//灰度处理
	cvtColor(m_SourceImg, grayImg, CV_BGR2GRAY);

		for(i=0;i<m_SourceSize.height;i++)
	{
		for(j=0;j<m_SourceSize.width;j++)
		{
			//把整个图像周围一圈的像素全部设置为0
			if(i<WINSIZE || j<WINSIZE || i>m_SourceSize.height-WINSIZE || j>m_SourceSize.width-WINSIZE)
			{
				m_FrImg.at<uchar>(i,j)=0;//找到单通道图像（行i，列j）位置的图像设置为0
				continue;
			}
			else
			{
				count=0;
				pixelCurrent=grayImg.at<uchar>(i,j);//获得当前位置像素值

				for(k=0;k<numModel&&count<minCount;k++)//只至少要找出其中两个
				{
					pixelModel=backModel[k][i][j];//储存下来的像素值
					pixelDiff=fabs(pixelModel-pixelCurrent);//求保存数据和现在的图像的差分
					if(pixelDiff<radius)//记录在20以内的像素点个数
					{
						count++;
					}
				}

				//如果上一步得到的图像大于等于2幅，那么就认为当前位置不是车辆位置
				//像素值匹配接近的像素得到了很多样本的认可则为匹配成功
				if(count>=minCount)
				{
					m_FrImg.at<uchar>(i,j)=0;//设置当前位置为零
					R1=GetRandom(0,20);//20张图片随机选择一个
					if(R1==0)
					{//随机第一次
						R2=GetRandom();//如果为0就再选一张
						backModel[R2][i][j]=pixelCurrent;
					}

					R1=GetRandom(0,20);
					if(R1==0)
					{//随机第二次
						Ri=GetRandom(i);
						Rj=GetRandom(j);
						R2=GetRandom();
						backModel[R2][Ri][Rj]=pixelCurrent;
					}
				}
				else
				{
					m_FrImg.at<uchar>(i,j)=255;//否则认为该点处的像素是白色的，即车辆所在像素
				}
			}
		}
	}
	DeleteSmallBlob(50);//删除小轮廓，面积阈值为50
	DeleteLongLifeBlob(200);//删除
	dilate(m_FrImg,m_FrImg,Mat(),Point(-1,-1),6);
	erode(m_FrImg,m_FrImg,Mat(),Point(-1,-1),1);
	dilate(m_FrImg,m_FrImg,Mat(),Point(-1,-1),3);
	erode(m_FrImg,m_FrImg,Mat(),Point(-1,-1),8);
	imshow("灰度图",grayImg);
	imshow("前景", m_FrImg);
}

void VibeFGDetector::DeleteSmallBlob(double area)
{
	vector< vector < Point > > contours;
	findContours( m_FrImg,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	for (vector< vector < Point > >::const_iterator count = contours.begin(); count != contours.end();){
		Rect temp=boundingRect(Mat(*count));
		if((temp.height*temp.width)<area){
			count=contours.erase(count);//可能要先删除里面的内容在删除外面的
		}
		else{
			count++;
		}
	}
	m_FrImg.setTo(Scalar::all(0));
	drawContours(m_FrImg, contours, -1, Scalar::all(255), CV_FILLED);
}

void VibeFGDetector::DeleteLongLifeBlob(int time)
{
	double pixelCurrent;
	int R1,R2;
	for(int i=WINSIZE;i<m_SourceSize.height-WINSIZE;i++)
	{
		for(int j=WINSIZE;j<m_SourceSize.width-WINSIZE;j++)
		{
			if(runningTime[i][j]>time)
			{
				runningTime[i][j]=0;
				pixelCurrent=grayImg.at<uchar>(i,j);
				R1=GetRandom();
				R2=GetRandom();
				backModel[R1][i][j]=pixelCurrent;
				backModel[R2][i][j]=pixelCurrent;
			}
			else
				runningTime[i][j]++;
		}
	}
}

VibeFGDetector::~VibeFGDetector(){
	/* =============================初始化============================== */
	delete [] backModel;
	delete [] runningTime;
}

