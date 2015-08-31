#include <iostream>
using namespace std;

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

#include "Controller.h"

Controller::Controller(const VideoCapture& pCap){
	SourceCap = pCap;
	//获取帧率
	rate = SourceCap.get(CV_CAP_PROP_FPS);
	//获取大小
	S = Size((int)SourceCap.get(CV_CAP_PROP_FRAME_WIDTH) ,
		(int)SourceCap.get(CV_CAP_PROP_FRAME_HEIGHT));
	FGD = FGDetectorFactory::Make(3);//这里改动可以变换处理
}

//-----------------------------【Process函数】---------------------------------------------  
//  描述：取帧处理函数，控制阈值
//-----------------------------------------------------------------------------------------------  
void  Controller::Process()
{
	//播放控制参数
	int OneFrameProcess = 0 , key;
	Mat  pImg;
	for ( nFrmNum = 0; (key = waitKey(OneFrameProcess ? 0 : rate)) != 27; nFrmNum++ )
	{
		//暂停控制
		if ( key != -1 )
		{
			OneFrameProcess = 1;
			if ( key == 'r' )OneFrameProcess = 0;//r键续播
		}

		//*******************第一帧准备数据********************************
		cout << "运行到" << nFrmNum << "帧" << endl;
		if ( nFrmNum == 0 ){
			if ( !SourceCap.read(pImg) ) break;
			//imshow("run视频" , pImg);
			FGD->Create(pImg , S);
			BD.Create(pImg , S);
		}
		//*********************************************************************

		if ( !SourceCap.read(pImg) ) break;//读入图像
		//imshow("run视频" , pImg);//输出显示图像
		//moveWindow("run视频" , 0 , 0);

		FGD->Process(pImg);//前景处理
		if ( nFrmNum > 100 ){
			try{
				if ( FGD->GetMask().data == NULL ){
					throw "没有获得图像掩膜";
				}
				BD.Process(FGD->GetMask());//团块处理
				BD.Draw(pImg);//绘图处理
			}
			catch ( string a ){
				cout << a;
			}
			catch ( cv::Exception a ){
				cout << a.what();
			}
			catch ( exception a ){
				cout << a.what();
			}
		}
	}
	//BD.Sort();//用来整理得到的车辆数据
}