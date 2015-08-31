#ifndef  VIBEFGDETECTOR_H
#define  VIBEFGDETECTOR_H

#include <vector>
#include "FGDetector.h"

class VibeFGDetector:public FGDetector{
private:
	Mat grayImg;//�Ҷ�ͼ

	Size m_SourceSize;//Դͼ���С

	unsigned char ***backModel;//����ģ��
	int **runningTime;//����ʱ��

public:
	static const unsigned int numModel;//�̶�ֵΪ20��������20��ģ�������Ա�
	static const unsigned int minCount;//�̶�ֵΪ2����ʾƥ�������С����
	static const unsigned int radius;//�̶�ֵΪ20����ʾ�����ͱ���ģ����Χ

private:
	void DeleteSmallBlob(double area);//ɾ��С����
	void DeleteLongLifeBlob(int time);//������Ӱ
	
public:
	~VibeFGDetector();
	/*��������*/
	void Create(Mat &Source,const Size S);
	/*����ǰͼƬ*/
	void Process(const cv::Mat &);
};
#endif