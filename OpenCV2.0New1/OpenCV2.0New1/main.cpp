//-----------------------------------��ͷ�ļ��������֡�---------------------------------------  
//  ����������������������ͷ�ļ�  
//----------------------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

//#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
//-----------------------------------�������ռ��������֡�---------------------------------------  
//  ����������������ʹ�õ������ռ�  
//------------------------------------------------------------------------------------------------------     
using namespace std;
using namespace cv;

/* ѡ����ȷ�ĺ������в����ִ�Сд���ַ����ȶ� */
#ifdef WIN32
#define MY_STRNICMP strnicmp
#define MY_STRICMP stricmp
#else
#define MY_STRNICMP strncasecmp
#define MY_STRICMP strcasecmp
#endif
#include "Controller.h"
////ǰ������ͷ�ļ�
//#include "SimpleFGDetector.h"
///* ǰ�����ģ���б� */
//static FGDetector* cvCreateSimpleFGDetector(){return (new SimpleFGDetector());}
//
//typedef struct DefModule_FGDetector
//{
//	FGDetector* (*create)();//ָ������ָ�롣ʹ������д��������ʹc���Ծ��������������ԡ�
//	char* nickname;//���FGDETECTOR�ı�����Nickname�൱��С��
//	char* description;//�����fgDetector�ļ�������
//} DefModule_FGDetector;
//
//DefModule_FGDetector FGDetector_Modules[] =//���Ǹ����顣
//{
//	{cvCreateSimpleFGDetector, "FG_SIMPLE","������ǰ400֡��ostu�Ż���Surdra���㷨"},
//	{NULL,NULL,NULL},//ȫ��Ϊ�գ������е�null�кܴ�����ã���ָʾ���������������һ���������������㷨����main����ʹ���������ʱ��ʹ�õ���һ�㡣
//};
///* ǰ�����ģ���б� */

int main(int argc, char* argv[])
{
	char* avi_name = NULL;
	/*
	if(argc!=2)
	{
	cout<<"usage:display_image imagetoloadanddisplay"<<endl;
	return -1;
	}

	//����Ƶ�ļ���λ��
	
	char* fg_name=NULL;
	char* avi_name=NULL;

	DefModule_FGDetector*           pFGModule = NULL;

	for(int i=1;i<argc;++i)
	{
		int bParsed = 0;
		//size_t ���Ͷ�����cstddefͷ�ļ��У����ļ���C��׼���ͷ�ļ�stddef.h��C++�档
		//����һ���������ص�unsigned���ͣ����С���Ա�֤�洢�ڴ��ж���Ĵ�С��
		size_t len = strlen(argv[i]);
		//����������"fg="��"fgavi="�ȶ�����ַ�ȥ��,���õ���ָ���λ��
		//���ط�������꣬���argv[i]����"fg="����ַ�����
		//���Ǿ���Ϊ�Ǹ�fg_name���������ֵ�����º���ͬ��
#define RO(_n1,_n2) if(MY_STRNICMP(argv[i],_n1,strlen(_n1))==0) {_n2 = argv[i]+strlen(_n1);bParsed=1;cout<<_n1<<_n2<<endl;};
		RO("fg=",fg_name);


		//���潫��ȡ��Ƶ���ļ���
#undef RO
		{
			char* ext = argv[i] + len-4;
			//strrchr()�����������ǣ�
			//����һ���ַ�������һ���ַ����� ĩ�� ���ֵ�λ�ã������ش��ַ����е����λ����һֱ���ַ��������������ַ���
			//���δ���ҵ�ָ���ַ�����ô����������False��
			if( strrchr(argv[i],'=') == NULL &&
				!bParsed &&
				(len>3 && (MY_STRICMP(ext,".avi") == 0 )))
			{
				avi_name = argv[i];//���������if�����͸�avi_name��ֵ������Ҫ�򿪵���Ƶ�ļ����ơ�
				break;
			}
		}// next argument ��һ������
	}

	for(pFGModule=FGDetector_Modules;pFGModule->nickname;++pFGModule)
		//��pFGModule��ֵ��ע�⵽�ļ��ͷ�ļ�������������null��β�ġ�
		if( fg_name && MY_STRICMP(fg_name,pFGModule->nickname)==0 ) break;//null�ǽ����ı�־��

	{//set default parameters for one processing Ϊһ����������Ĭ�ϵĲ���
		if(!fg_name) fg_name = FGDetector_Modules[0].nickname;//�����main�������Ĳ�����fg_nameΪ�գ����������ĵ�0������fg_name��
		if(!avi_name) avi_name = "E:\\����·\\VID_20141128_111510.avi";
	}
	//std::string videoFile = "F:\\c++\\������\\����\\4.avi";
	*/
	
	//if ( !avi_name ) avi_name = "E:\\����·\\VID_20141128_111510.avi";
	if ( !avi_name ) avi_name = "F:\\c++\\������\\����\\4.avi";
	/* create source video ����Դ��Ƶ*/
	string videoFile(avi_name);
	//׼���ṹ��
	VideoCapture capture;
	//����Ƶ
	capture.open(videoFile);
	//�����Ƶ�Ƿ�ɹ���
	if(!capture.isOpened())
		return 1;

	Controller CarTrack(capture);//�����ȡ������Ƶ��Դ
	CarTrack.Process();//���д���
}