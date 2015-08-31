#include "OCVFGDetector.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;


OCVFGDetector::OCVFGDetector()
{
	update_bg_model = true;
}


OCVFGDetector::~OCVFGDetector()
{
}


/*创建函数*/
void OCVFGDetector::Create(Mat &Source , const Size S)
{
	if ( Source.empty() )
		return;
	m_SourceImg = Source;
	namedWindow("image" , WINDOW_AUTOSIZE);
	namedWindow("foreground mask" , WINDOW_AUTOSIZE);
	namedWindow("foreground image" , WINDOW_AUTOSIZE);
	namedWindow("mean background image" , WINDOW_AUTOSIZE);
}

/* 处理当前图片*/
void OCVFGDetector::Process(const cv::Mat & pImg)
{
	if ( pImg.empty() )
		return;
	m_SourceImg = pImg;
	//cvtColor(_img, img, COLOR_BGR2GRAY);

	if ( fgimg.empty() )
		fgimg.create(m_SourceImg.size() , m_SourceImg.type());

	//更新模型
	bg_model(m_SourceImg , m_FrImg , update_bg_model ? -1 : 0);

	fgimg = Scalar::all(0);
	m_SourceImg.copyTo(fgimg , m_FrImg);

	Mat bgimg;
	bg_model.getBackgroundImage(bgimg);

	imshow("image" , m_SourceImg);
	imshow("foreground mask" , m_FrImg);
	imshow("foreground image" , fgimg);
	if ( !bgimg.empty() )
		imshow("mean background image" , bgimg);

	char k = (char)waitKey(1);
	if ( k == ' ' )
	{
		update_bg_model = !update_bg_model;
		if ( update_bg_model )
			printf("\t>背景更新(Background update)已打开\n");
		else
			printf("\t>背景更新(Background update)已关闭\n");
	}
}