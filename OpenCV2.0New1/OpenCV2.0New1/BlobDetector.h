#ifndef  MYBLOBDETECTOR_H
#define  MYBLOBDETECTOR_H

#include <vector>
#include "CarInfo.h"

class BlobDetector{

	//����
private:
	Mat m_FrImg;
	vector<CarInfo> m_BlobListOld;//ǰһ֡���ſ�
	vector<CarInfo> m_BlobListNow;//��ǰ֡���ſ�
	vector<CarInfo> HistoryBlobList;//�����м�¼���ſ�
	vector<CarInfo> SameblobList;//��ʱ�洢ĳ��ID��ȫ����Ϣ
	vector<vector <CarInfo> > ProcblobList;//������ɺ��ȫ��������Ϣ��ID��ű���켣
	
	const int m_Maxinone;//������ſ�����
	unsigned int nFrmNum;//֡��
	unsigned int m_NextBlobID;//ID�ŷ���

	Size m_SourceSize;//Դͼ���С

private:
	//�㷨����ʱ��Ҫʹ�õ��ķ���
	void DetectFrBlob();
	void DetectANamedNewBlob();
	void TempGetBlobByID(int BlobID);
	
	//�û����Ե��õĺ���
public:
	BlobDetector();
	void Create(Mat &Source,const Size S);//��������
	void Process(const cv::Mat &);//����ǰͼƬ
	Mat GetMask();//��ȡ������ǰ��ͼƬ
	void Draw(cv::Mat &);//�þ��ο��ÿһ������
	void Sort();//�����������ݣ�׼������
	vector<CarInfo> GetBlobByID(int BlobID);
};
#endif