//-----------------------------------【头文件包含部分】---------------------------------------  
//  描述：包含程序所依赖的头文件  
//----------------------------------------------------------------------------------------------------
#include <iostream>
#include <string>
#include <vector>

//#include <opencv2/opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/highgui/highgui.hpp"  
//-----------------------------------【命名空间声明部分】---------------------------------------  
//  描述：包含程序所使用的命名空间  
//------------------------------------------------------------------------------------------------------     
using namespace std;
using namespace cv;

/* 选择正确的函数进行不区分大小写的字符串比对 */
#ifdef WIN32
#define MY_STRNICMP strnicmp
#define MY_STRICMP stricmp
#else
#define MY_STRNICMP strncasecmp
#define MY_STRICMP strcasecmp
#endif
#include "Controller.h"
////前景处理头文件
//#include "SimpleFGDetector.h"
///* 前景检测模块列表 */
//static FGDetector* cvCreateSimpleFGDetector(){return (new SimpleFGDetector());}
//
//typedef struct DefModule_FGDetector
//{
//	FGDetector* (*create)();//指向函数的指针。使用这种写法，可以使c语言具有面向对象的特性。
//	char* nickname;//这个FGDETECTOR的别名。Nickname相当于小名
//	char* description;//这个是fgDetector的简单描述。
//} DefModule_FGDetector;
//
//DefModule_FGDetector FGDetector_Modules[] =//这是个数组。
//{
//	{cvCreateSimpleFGDetector, "FG_SIMPLE","运用最前400帧和ostu优化和Surdra的算法"},
//	{NULL,NULL,NULL},//全部为空，这里有的null有很大的作用，来指示这是这个数组的最后一个，增加其他的算法。在main函数使用这个数组时会使用到这一点。
//};
///* 前景检测模块列表 */

int main(int argc, char* argv[])
{
	char* avi_name = NULL;
	/*
	if(argc!=2)
	{
	cout<<"usage:display_image imagetoloadanddisplay"<<endl;
	return -1;
	}

	//打开视频文件的位置
	
	char* fg_name=NULL;
	char* avi_name=NULL;

	DefModule_FGDetector*           pFGModule = NULL;

	for(int i=1;i<argc;++i)
	{
		int bParsed = 0;
		//size_t 类型定义在cstddef头文件中，该文件是C标准库的头文件stddef.h的C++版。
		//它是一个与机器相关的unsigned类型，其大小足以保证存储内存中对象的大小。
		size_t len = strlen(argv[i]);
		//将参数中如"fg="、"fgavi="等多余的字符去掉,利用的是指针的位移
		//着重分析这个宏，如果argv[i]等于"fg="这个字符串。
		//我们就认为是给fg_name这个变量赋值。以下宏相同。
#define RO(_n1,_n2) if(MY_STRNICMP(argv[i],_n1,strlen(_n1))==0) {_n2 = argv[i]+strlen(_n1);bParsed=1;cout<<_n1<<_n2<<endl;};
		RO("fg=",fg_name);


		//下面将提取视频的文件名
#undef RO
		{
			char* ext = argv[i] + len-4;
			//strrchr()函数的作用是：
			//查找一个字符串在另一个字符串中 末次 出现的位置，并返回从字符串中的这个位置起，一直到字符串结束的所有字符；
			//如果未能找到指定字符，那么函数将返回False。
			if( strrchr(argv[i],'=') == NULL &&
				!bParsed &&
				(len>3 && (MY_STRICMP(ext,".avi") == 0 )))
			{
				avi_name = argv[i];//满足上面的if条件就给avi_name赋值，就是要打开的视频文件名称。
				break;
			}
		}// next argument 下一个参数
	}

	for(pFGModule=FGDetector_Modules;pFGModule->nickname;++pFGModule)
		//给pFGModule赋值。注意到文件最开头的几个数组的最后都是null结尾的。
		if( fg_name && MY_STRICMP(fg_name,pFGModule->nickname)==0 ) break;//null是结束的标志。

	{//set default parameters for one processing 为一个处理设置默认的参数
		if(!fg_name) fg_name = FGDetector_Modules[0].nickname;//如果从main传过来的参数的fg_name为空，则将这个数组的第0个附给fg_name。
		if(!avi_name) avi_name = "E:\\科韵路\\VID_20141128_111510.avi";
	}
	//std::string videoFile = "F:\\c++\\陈智敏\\程序\\4.avi";
	*/
	
	//if ( !avi_name ) avi_name = "E:\\科韵路\\VID_20141128_111510.avi";
	if ( !avi_name ) avi_name = "F:\\c++\\陈智敏\\程序\\4.avi";
	/* create source video 创建源视频*/
	string videoFile(avi_name);
	//准备结构体
	VideoCapture capture;
	//打开视频
	capture.open(videoFile);
	//检查视频是否成功打开
	if(!capture.isOpened())
		return 1;

	Controller CarTrack(capture);//传入读取到的视频资源
	CarTrack.Process();//运行处理
}