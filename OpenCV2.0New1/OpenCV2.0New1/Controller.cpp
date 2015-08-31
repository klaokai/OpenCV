#include <iostream>
using namespace std;

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
using namespace cv;

#include "Controller.h"

Controller::Controller(const VideoCapture& pCap){
	SourceCap = pCap;
	//��ȡ֡��
	rate = SourceCap.get(CV_CAP_PROP_FPS);
	//��ȡ��С
	S = Size((int)SourceCap.get(CV_CAP_PROP_FRAME_WIDTH) ,
		(int)SourceCap.get(CV_CAP_PROP_FRAME_HEIGHT));
	FGD = FGDetectorFactory::Make(3);//����Ķ����Ա任����
}

//-----------------------------��Process������---------------------------------------------  
//  ������ȡ֡��������������ֵ
//-----------------------------------------------------------------------------------------------  
void  Controller::Process()
{
	//���ſ��Ʋ���
	int OneFrameProcess = 0 , key;
	Mat  pImg;
	for ( nFrmNum = 0; (key = waitKey(OneFrameProcess ? 0 : rate)) != 27; nFrmNum++ )
	{
		//��ͣ����
		if ( key != -1 )
		{
			OneFrameProcess = 1;
			if ( key == 'r' )OneFrameProcess = 0;//r������
		}

		//*******************��һ֡׼������********************************
		cout << "���е�" << nFrmNum << "֡" << endl;
		if ( nFrmNum == 0 ){
			if ( !SourceCap.read(pImg) ) break;
			//imshow("run��Ƶ" , pImg);
			FGD->Create(pImg , S);
			BD.Create(pImg , S);
		}
		//*********************************************************************

		if ( !SourceCap.read(pImg) ) break;//����ͼ��
		//imshow("run��Ƶ" , pImg);//�����ʾͼ��
		//moveWindow("run��Ƶ" , 0 , 0);

		FGD->Process(pImg);//ǰ������
		if ( nFrmNum > 100 ){
			try{
				if ( FGD->GetMask().data == NULL ){
					throw "û�л��ͼ����Ĥ";
				}
				BD.Process(FGD->GetMask());//�ſ鴦��
				BD.Draw(pImg);//��ͼ����
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
	//BD.Sort();//��������õ��ĳ�������
}