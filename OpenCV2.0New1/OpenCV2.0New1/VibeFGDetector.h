#ifndef  VIBEFGDETECTOR_H
#define  VIBEFGDETECTOR_H

#include <vector>
#include "FGDetector.h"

class VibeFGDetector:public FGDetector{
private:
	Mat grayImg;//灰度图

	Size m_SourceSize;//源图像大小

	unsigned char ***backModel;//背景模型
	int **runningTime;//更新时间

public:
	static const unsigned int numModel;//固定值为20，表明有20个模板用作对比
	static const unsigned int minCount;//固定值为2，表示匹配符合最小次数
	static const unsigned int radius;//固定值为20，表示背景和背景模型相差范围

private:
	void DeleteSmallBlob(double area);//删除小轮廓
	void DeleteLongLifeBlob(int time);//消除残影
	
public:
	~VibeFGDetector();
	/*创建函数*/
	void Create(Mat &Source,const Size S);
	/*处理当前图片*/
	void Process(const cv::Mat &);
};
#endif