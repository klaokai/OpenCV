#ifndef  CARINFO_H
#define  CARINFO_H

class CarInfo{
public:
	CarInfo(){ ID = 0; }
	CarInfo(float x, float y, float width, float height, int ID = -1)
		:x(x), y(y), w(width), h(height)	{}

private:
	float   x, y; /* ����λ�� */
	float   w, h; /*������С  */

public:
	int     ID;  /* ����ID*/
	//���λ��
	cv::Point getCentroid(){
		cv::Point a((int)x, (int)y);
		return a;
	}
	//������
	float getArea(){
		return w*h;
	}
	//�����������������ľ���
	float BlobDistance(CarInfo* b){
		return sqrt(pow((x + w / 2 - b->x - (b->w) / 2), 2) + pow((y + h / 2 - b->y - (b->h) / 2), 2));
	}
	//��ֱ��������������
	void Line(const CarInfo*  b, cv::Mat &dst){
		line(dst, cv::Point((int)(x + w / 2), (int)y), cv::Point((int)(b->x + b->w / 2), (int)(b->y)), cv::Scalar(0, 0, 255), 1, 8, 0);
	}
	//����CarInfo����
	void Rectangle(cv::Mat &dst){
		cv::Rect temp((int)x , (int)y , (int)w , (int)h);
		cv::rectangle(dst, temp, cv::Scalar::all(255));
	}
};

#endif