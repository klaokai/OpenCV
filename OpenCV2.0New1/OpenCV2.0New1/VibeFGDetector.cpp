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

//��һ�������������
int GetRandom(int istart,int iend)  
{  
	int val=istart+rand()%(iend-istart);
	return val;
}  


//-------------------------��VibeFGDetector::Create������---------------------------------
//  ������׼��3���۲촰�ڣ���׼��ԭʼͼ��Ŀ���
//-----------------------------------------------------------------------------------------------
const unsigned int VibeFGDetector::numModel=20;
const unsigned int VibeFGDetector::minCount=2;
const unsigned int VibeFGDetector::radius=20;

void VibeFGDetector::Create(Mat &Source,const Size S)
{
	//namedWindow("ԭ��Ƶ",1);
	//moveWindow("ԭ��Ƶ",0,0);

	namedWindow("�Ҷ�ͼ", 1);
	moveWindow("�Ҷ�ͼ", 330,0);

	namedWindow("ǰ��", 1);
	moveWindow("ǰ��", 660,0);

	m_SourceImg=Source;
	m_SourceSize=S;

	m_FrImg.create(m_SourceSize,CV_8UC1);

	cvtColor(m_SourceImg, grayImg, CV_BGR2GRAY);

	/* =============================��ʼ��============================== */
	//����һ����ӦΪ20����ߺ���Ƶһ��ͼ��
	/*  ����ʵ�ֵ�һ֡��ÿ�����ص�8�����ڵĲ������ܣ�������Ӧ�ı���ģ��*******/  
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
	
	


	
	// �������б�����ģs
	int pixel;
	int ri=0,rj=0;//���������ֵ
	for (int i=WINSIZE;i<m_SourceSize.height-WINSIZE;i++)	{
		for(int j=WINSIZE;j<m_SourceSize.width-WINSIZE;j++){
			//��3*3�����н��в���
			for(int k=0;k<numModel;k++)
			{
				ri=GetRandom(i);
				rj=GetRandom(j);
				pixel=grayImg.at<uchar>(ri,rj);//��ȡԭͼ��Ҷ�ͼ����ֵ
				backModel[k][i][j]=pixel;//����20��ͼ��������
			}
		}
	}


	//��ʼ��ǰ������Ĥ���������� 
	runningTime=new int* [m_SourceSize.height];
	for(int i=0;i<m_SourceSize.height;i++)
	{
		runningTime[i]=new int [m_SourceSize.width];
		for(int j=0;j<m_SourceSize.width;j++)
			runningTime[i][j]=0;
	}
}


//-------------------------��VibeFGDetector::Process������---------------------------------
//  ������һ���ǰ��������Ҫ�ڴ˹����л�ñ���ͼ��
//-----------------------------------------------------------------------------------------
void VibeFGDetector::Process(const cv::Mat &pImg)
{
	m_SourceImg=pImg;
	//imshow("ԭ��Ƶ",m_SourceImg);
	m_FrImg.setTo(Scalar::all(0));
	double pixelModel,pixelCurrent,pixelDiff;
	int R1,R2;//���������
	int Ri,Rj;//����������X��Y�������
	int i,j,k;
	int count=0;//ƥ����ϴ���,����Ƚ�����ֵ�ڵĴ���
	//***************ǰ���������***************************************
	//�Ҷȴ���
	cvtColor(m_SourceImg, grayImg, CV_BGR2GRAY);

		for(i=0;i<m_SourceSize.height;i++)
	{
		for(j=0;j<m_SourceSize.width;j++)
		{
			//������ͼ����ΧһȦ������ȫ������Ϊ0
			if(i<WINSIZE || j<WINSIZE || i>m_SourceSize.height-WINSIZE || j>m_SourceSize.width-WINSIZE)
			{
				m_FrImg.at<uchar>(i,j)=0;//�ҵ���ͨ��ͼ����i����j��λ�õ�ͼ������Ϊ0
				continue;
			}
			else
			{
				count=0;
				pixelCurrent=grayImg.at<uchar>(i,j);//��õ�ǰλ������ֵ

				for(k=0;k<numModel&&count<minCount;k++)//ֻ����Ҫ�ҳ���������
				{
					pixelModel=backModel[k][i][j];//��������������ֵ
					pixelDiff=fabs(pixelModel-pixelCurrent);//�󱣴����ݺ����ڵ�ͼ��Ĳ��
					if(pixelDiff<radius)//��¼��20���ڵ����ص����
					{
						count++;
					}
				}

				//�����һ���õ���ͼ����ڵ���2������ô����Ϊ��ǰλ�ò��ǳ���λ��
				//����ֵƥ��ӽ������صõ��˺ܶ��������Ͽ���Ϊƥ��ɹ�
				if(count>=minCount)
				{
					m_FrImg.at<uchar>(i,j)=0;//���õ�ǰλ��Ϊ��
					R1=GetRandom(0,20);//20��ͼƬ���ѡ��һ��
					if(R1==0)
					{//�����һ��
						R2=GetRandom();//���Ϊ0����ѡһ��
						backModel[R2][i][j]=pixelCurrent;
					}

					R1=GetRandom(0,20);
					if(R1==0)
					{//����ڶ���
						Ri=GetRandom(i);
						Rj=GetRandom(j);
						R2=GetRandom();
						backModel[R2][Ri][Rj]=pixelCurrent;
					}
				}
				else
				{
					m_FrImg.at<uchar>(i,j)=255;//������Ϊ�õ㴦�������ǰ�ɫ�ģ���������������
				}
			}
		}
	}
	DeleteSmallBlob(50);//ɾ��С�����������ֵΪ50
	DeleteLongLifeBlob(200);//ɾ��
	dilate(m_FrImg,m_FrImg,Mat(),Point(-1,-1),6);
	erode(m_FrImg,m_FrImg,Mat(),Point(-1,-1),1);
	dilate(m_FrImg,m_FrImg,Mat(),Point(-1,-1),3);
	erode(m_FrImg,m_FrImg,Mat(),Point(-1,-1),8);
	imshow("�Ҷ�ͼ",grayImg);
	imshow("ǰ��", m_FrImg);
}

void VibeFGDetector::DeleteSmallBlob(double area)
{
	vector< vector < Point > > contours;
	findContours( m_FrImg,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
	for (vector< vector < Point > >::const_iterator count = contours.begin(); count != contours.end();){
		Rect temp=boundingRect(Mat(*count));
		if((temp.height*temp.width)<area){
			count=contours.erase(count);//����Ҫ��ɾ�������������ɾ�������
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
	/* =============================��ʼ��============================== */
	delete [] backModel;
	delete [] runningTime;
}

