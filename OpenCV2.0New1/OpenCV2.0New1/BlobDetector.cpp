#include <iostream>
using namespace std;

#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
using namespace cv;

#include "BlobDetector.h"

BlobDetector::BlobDetector() :m_Maxinone(10), nFrmNum(0), m_NextBlobID(0){
	namedWindow("���", 1);
	moveWindow("���", 0, 270);
}
//------------------------��BlobDetector::Create������--------------------------------
//  ������׼��1���۲촰�ڣ���׼��ԭʼͼ��Ŀ���
//-----------------------------------------------------------------------------------------------
void BlobDetector::Create(Mat &Source, const Size S)
{
	//BlobDetector();
	m_FrImg = Source;
	m_SourceSize = S;
}

//--------------------------��TempGetBlobByID������-------------------------------------  
//  ������������ͬһID�ŵ��ſ�װ��һ��Vector�����Թ�ʹ��
//-----------------------------------------------------------------------------------------------  
void BlobDetector::TempGetBlobByID(int BlobID)
{
	//��ʱʹ��һ��vector����¼������Ϣ�����ҵ�����ID�ŵ��ſ����뵽�б���
	SameblobList.clear();
	for (vector<int>::size_type i = 0; i < HistoryBlobList.size(); i++)
		//for(vector<int>::size_type i=0; i<ProcBlobList.size(); i++)
		if (BlobID == HistoryBlobList.at(i).ID){
			CarInfo *pB = &HistoryBlobList.at(i);
			SameblobList.push_back(*pB);
		}
}

//--------------------��BlobDetector::DetectFrBlob������---------------------------
//  ������׼��1����ͨ����ǰ��ͼ��pIB�ò���������ֻ��ȡ����10��������
//����̫С����ȥ
//----------------------------------------------------------------------------------------------
void BlobDetector::DetectFrBlob(){
	//׼������
	m_BlobListNow.clear();
	Mat pIB(m_SourceSize, CV_8UC1);
	m_FrImg.convertTo(pIB, pIB.type());//��findContoursʱͼ��pFrImg�����޸ģ����Կ���һ��ͼ�����
	vector<CarInfo> Blobs;//pNewBlobList�洢ǰ10��blob��Blobs�洢ȫ����blob
	vector<vector<cv::Point>> contours;//����������
	contours.clear();

	//�Ӷ�ֵͼ���м����������洢��������contours �����ؼ�⵽������ͷָ��
	//findContour���������Ĵ�С CV_RETR_EXTERNALΪ��ⷽʽ��ֻ���ҳ�����������
	try{
	findContours(pIB, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	}
	catch ( cv::Exception msg ){
		cout << msg.what();
	}
	//���������Ƕ�ÿһ�����������곤����Ϊ�ſ�洢
	for (vector<int>::size_type i = 0; i < contours.size(); i++)
	{
		CarInfo  NewBlob;
		Rect  r = boundingRect(Mat(contours.at((int)i)));
		//������ж���Ϊ�˺��Խ�С���ſ飬�����0.02�����޸�Ϊ���ʵ�ֵ
		if (r.height < m_SourceSize.height*0.02 || r.width < m_SourceSize.width*0.02) continue;
		NewBlob = CarInfo(r.x, r.y, r.width, r.height);
		Blobs.push_back(NewBlob);//����Blobs
	}

	//����ǰ���ſ�ð�������ɴ�С
	int N = Blobs.size();
	for (int i = 1; i < N; ++i)
	{
		for (int j = i; j > 0; --j)
		{
			CarInfo  temp;
			float   AreaP, AreaN;
			CarInfo* pP = &Blobs.at(j - 1);
			CarInfo* pN = &Blobs.at(j);
			AreaP = pP->getArea();
			AreaN = pN->getArea();
			if (AreaN < AreaP)break;
			temp = *pN;
			*pN = *pP;
			*pP = temp;
		}
	}

	//ֻ��������m_Maxinone(10)���ſ�
	for (int i = 0; i < MIN(N, m_Maxinone); ++i)
	{
		m_BlobListNow.push_back(Blobs.at(i));
	}
}

//----------��BlobDetector::DetectANamedNewBlob������-----------------------
//  ��������DetectFrBlob�Ļ���֮�������ſ����ţ��ɵ��ſ���Ϯ��ţ�
//�µ��ſ������µı��
//-----------------------------------------------------------------------------------------------
void BlobDetector::DetectANamedNewBlob(){
	//��pNowBlobList��pOldBlobList��������ľ�����Now[i][0]
	const int NowSize = m_BlobListNow.size();
	const int OldSize = m_BlobListOld.size();
	if (NowSize == 0){
		cout << "��ǰͼ����������" << endl;
		return;
	}

	int Now[10 + 1][10 + 1];//��ʱ�洢�������� 
	for (int i = 0; i < m_Maxinone + 1; i++)
		for (int j = 0; j < m_Maxinone + 1; j++)
			Now[i][j] = m_SourceSize.height + m_SourceSize.width;

	for (int i = NowSize; i > 0; i--){
		CarInfo pBNow = m_BlobListNow.at(i - 1);
		Now[i][0] = m_SourceSize.height + m_SourceSize.width;//ͼ��Ŀ��֮�ͣ�ֻ��Ϊ�˷�ֹ����Ϊ�գ�
		for (int j = OldSize; j > 0; j--){
			CarInfo pBOld = m_BlobListOld.at(j - 1);
			Now[i][j] = (int)pBNow.BlobDistance(&pBOld);//���������ſ����С��Ӿ������ľ���
			Now[i][0] = Now[i][0] > Now[i][j] ? Now[i][j] : Now[i][0];
		}
	}

	//�����ǰ֡�ſ�������ǰ������ſ�
	int TempNow[10 + 1];//������¼����ſ�����
	for (int i = 1; i < m_Maxinone + 1; i++)
		for (int j = 1; j < m_Maxinone + 1; j++)
			if (Now[i][j] == Now[i][0])
				TempNow[i] = j;//��i������ſ������Ϊj

	//���ĳ����pNowBlobList�е��ſ�ĵ�pOldBlobList�е�����ſ���ͬһ����������С��
	//���������һ��һά�����¼�ȽϵĽ��
	for (int i = NowSize; i > 0; i--){
		//����ʹ��ߵ�ѭ����������֮ǰ�Ѵ��ڵľ����ֹ������������Ƕ��ǵ����
		for (int j = i - 1; j > 0; j--){
			//���k==l����ɾ���Ƚϴ���Ǹ���������С��
			if (TempNow[i] == TempNow[j] && Now[i][0] < Now[j][0]){
				Now[j][0] = m_SourceSize.height + m_SourceSize.width;
			}
			else if (TempNow[i] == TempNow[j] && Now[i][0] >= Now[j][0]){
				Now[i][0] = m_SourceSize.height + m_SourceSize.width;
			}
			if (Now[i][0]>20)Now[i][0] = m_SourceSize.height + m_SourceSize.width;//������ǰ�������20�ģ���Ϊ���ſ�
		}
	}

	//Ϊ���е��ſ���
	for (int i = NowSize; i > 0; i--){
		//��"û��"���������ſ���Ϊ���ſ飬���
		if (Now[i][0] == m_SourceSize.height + m_SourceSize.width){
			CarInfo *pB = &m_BlobListNow.at(i - 1);
			pB->ID = m_NextBlobID;//������״μ�⵽���ſ飬���ǵ�ǰͼ����ſ飨�������֡������ſ飩
			m_NextBlobID++;
		}

		//��������ſ���ſ�IDͳһ
		else{
			CarInfo *pBNow = &m_BlobListNow.at(i - 1);
			CarInfo *pBOld = &m_BlobListOld.at(TempNow[i] - 1);
			pBNow->ID = pBOld->ID;//�ɵ��ſ�����Ϯ���µ�
		}
	}
}

//------------------------��BlobDetector::Process������------------------------------
//  �������Ȳ��ұ�֡����DetectFrBlob���ٽ��бȶԴ����Ը��±��
//DetectANamedNewBlob 
//-----------------------------------------------------------------------------------------------
void BlobDetector::Process(const Mat &FrMask){
	//��⵱ǰ�ſ�DetectFrBlob
	m_FrImg = FrMask;
#ifdef _DEBUG
	cout << "����";
#endif // _DEBUG
	DetectFrBlob();

	if (nFrmNum == 0){
		m_BlobListOld = m_BlobListNow;
	}
	//�Ƚ�pNowBlobList��pOldBlobList������ͬ���ſ�IDͳһ��pNewBlobListָ�����ſ�
	DetectANamedNewBlob();
	//***************************�ſ鴦��ʼ********************************
	if (m_BlobListOld.size() != 0){
		/*if(m_NextBlobID==0 || m_NextBlobID>ProcblobList.size()){
			ProcblobList.resize(m_NextBlobID);
			}*/
		for (int i = m_BlobListOld.size(); i > 0; i--){
			HistoryBlobList.push_back(m_BlobListOld.at(i - 1));
			//ProcblobList.at(m_BlobListOld.at(i-1).ID).push_back(m_BlobListOld.at(i-1));
		}
	}


	m_BlobListOld.clear();
	m_BlobListOld = m_BlobListNow;
	//*************************�ſ鴦�����*********************************
	nFrmNum++;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//�����ǿ�����Ա���Ե��õĺ��������Ը����Լ�������ѡ���ʺ��Լ���
//	����
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//-------------------------��BlobDetector::Draw������----------------------------------  
//  ���������л�ͼ�������Ƴ���ǰͼ�������е��ſ�λ�ã��˶��켣�Լ�
//ID��
//-----------------------------------------------------------------------------------------------  
void BlobDetector::Draw(cv::Mat &pImg){
	//��ID
	Mat   pI = pImg.clone();//��ɫԭͼ
	char        str1[1024];

	for (int i = m_BlobListNow.size(); i > 0; i--)
	{
		CarInfo *pB = &m_BlobListNow.at(i - 1);
		sprintf(str1, "%03d", pB->ID);
		Point p(pB->getCentroid());
		//if( areaImg.at<cv::Vec3b>(p.y,p.x)[0]==255) //�����ж�
		//����ı�����
		//�ֱ���ͼ����������ַ�������ʼ�㣨���£������壬���ű�������ɫ����ϸ��
		cv::putText(pI, str1, p, FONT_HERSHEY_SIMPLEX, 1.0, Scalar(0, 255, 255), 1);
		//p.y-=8;//����λ��
		//cvPutText( pI, str2,p, &font, CV_RGB(255,0,0));//�����Ϣ2
	}

	//���켣��
	for (int i = m_BlobListNow.size(); i > 0; i--)
	{
		CarInfo *pB = &m_BlobListNow.at(i - 1);
		TempGetBlobByID(pB->ID);
		//SameblobList=GetBlobByID(pB->ID);
		for (int j = SameblobList.size(); j > 1; j--){
			CarInfo *pB1 = &SameblobList.at(j - 1);
			CarInfo *pB2 = &SameblobList.at(j - 2);
			//cvLine(pI,cvPoint(pB1->x+pB1->w/2,pB1->y),cvPoint(pB2->x+pB2->w/2,pB2->y),CV_RGB(255,0,0),1,8,0);
			pB1->Line(pB2, pI);
		}
	}

	//����
	for (int i = m_BlobListNow.size(); i > 0; i--)
	{
		CarInfo *pB = &m_BlobListNow.at(i - 1);
		//cvRectangle(pI,cvPoint(pB->x,pB->y),cvPoint(pB->x+pB->w,pB->y+pB->h),cvScalar(255),1,8, 0);
		pB->Rectangle(pI);
	}

	//�ж��Ƿ���������
	/*for(int i= m_BlobListNow.GetBlobNum();i>0;i--)
	{
	CvBlob *pB=m_BlobListNow.GetBlob(i-1);
	if( ((uchar *)(areaImg->imageData + (int)pB->y*areaImg->widthStep))[(int)pB->x*areaImg->nChannels + 0]==255)
	cvRectangle(pI,cvPoint(pB->x,pB->y),cvPoint(pB->x+pB->w,pB->y+pB->h),CV_RGB(255,0,0),1,8, 0);
	}*/

	imshow("���", pI);
}


//-------------------------��BlobDetector::Sort������----------------------------------  
//  �������������ݴ���ȷ�����е����ݱ������ProcblobListȻ����д���
//-----------------------------------------------------------------------------------------------
void BlobDetector::Sort(){
	ProcblobList.resize(m_NextBlobID);
	for (vector<int>::size_type i = 0; i < HistoryBlobList.size(); i++)	{
		TempGetBlobByID(i);
		ProcblobList.push_back(SameblobList);
	}
}

//---------------------��BlobDetector::GetBlobByID������----------------------------  
//  ���������Ե�����������õ�ĳһ��ID��������Ϣ��ʹ��֮ǰ����Ҫ����
//һ��sort����������
//-----------------------------------------------------------------------------------------------
vector<CarInfo> BlobDetector::GetBlobByID(int BlobID)
{
	//ʹ��vector<vector<CarInfo>>����¼������Ϣ���ҵ�����ID�ŵ��ſ�㷵�ظó����ĸ�����⵽��λ��
	if (0 <= BlobID && (unsigned)BlobID <= ProcblobList.size()){
		return ProcblobList.at(BlobID);
	}
	else{
		vector<CarInfo>  null(0);
		return null;
	}
}