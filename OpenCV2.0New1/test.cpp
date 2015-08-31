#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

int otsu (IplImage *image);
void HSV_Detect(IplImage* psrcimg,IplImage* pbackimg,IplImage* psrcgray,IplImage* pgrayimg);

int main()
{
	std::string videoFile = "v.avi";
	cv::VideoCapture capture;
	capture.open(videoFile);

	cv::BackgroundSubtractorMOG2 mog;
	
	cv::Mat foreground;
	cv::Mat background;
	int i,j;
	cv::Mat frame;
	/*capture.read(frame);*/
	cv::Mat pFrame;
	cv::Mat psrcimg;
	cv::Mat pbackimg;
	long frameNo = 0;
	
	while (capture.read(frame))
	{
		mog(frame, foreground, 0.01); 
		mog.getBackgroundImage(background);
		IplImage img=(IplImage)frame;
		IplImage* srcimg=cvCreateImage(cvGetSize(&img), 8,3);
		IplImage* backimg=cvCreateImage(cvGetSize(&img), 8,3);
		IplImage* dstimg=cvCreateImage(cvGetSize(&img), 8,3);
		IplImage* src_grayimg=cvCreateImage(cvGetSize(&img), 8,1);
		IplImage* back_grayimg=cvCreateImage(cvGetSize(&img), 8,1);
		IplImage* grayimg=cvCreateImage(cvGetSize(&img), 8,1);
		IplImage* grayimg1=cvCreateImage(cvGetSize(&img), 8,1);
		IplImage* binaryimg=cvCreateImage(cvGetSize(&img), 8,1);
		
		srcimg=&(IplImage)frame;
		backimg=&(IplImage)background;
		cvCvtColor(srcimg,src_grayimg,CV_BGR2GRAY);
		cvCvtColor(backimg,back_grayimg,CV_BGR2GRAY);
		cvCvtColor(backimg,grayimg,CV_BGR2GRAY);
		cvAbsDiff(src_grayimg,back_grayimg,grayimg);
		cvCopy(grayimg,grayimg1);
		//int threshold=otsu (grayimg1);
		cvThreshold(grayimg,binaryimg,33,255.0,CV_THRESH_BINARY);

		cvErode(binaryimg,binaryimg);
		cvDilate(binaryimg,binaryimg);

		

		HSV_Detect(srcimg,backimg,binaryimg,dstimg);

		cvShowImage("srcimg",srcimg);
		cvShowImage("backimg",backimg);
		cvShowImage("grayimg",grayimg);
		cvShowImage("binaryimg",binaryimg);
		cvShowImage("dstimg",dstimg);
		if (cv::waitKey(25) > 0)
		{
			break;
		}
	}
		return 0;
}

//大津法求阈值
int otsu (IplImage *image)
{
int w = image->width;
int h = image->height;
    
    unsigned char*np; // 图像指针
    unsigned char pixel;
int thresholdValue=1; // 阈值
int ihist[256]; // 图像直方图，256个点
    
int i, j, k; // various counters
int n, n1, n2, gmin, gmax;
double m1, m2, sum, csum, fmax, sb;
    
// 对直方图置零...
    memset(ihist, 0, sizeof(ihist));
    
    gmin=255; gmax=0;
// 生成直方图
for (i =0; i < h; i++) 
    {
        np = (unsigned char*)(image->imageData + image->widthStep*i);
for (j =0; j < w; j++) 
        {
            pixel = np[j];
            ihist[ pixel]++;
if(pixel > gmax) gmax= pixel;
if(pixel < gmin) gmin= pixel;
        }
    }
    
// set up everything
    sum = csum =0.0;
    n =0;
    
for (k =0; k <=255; k++) 
    {
        sum += k * ihist[k]; /* x*f(x) 质量矩*/
        n += ihist[k]; /* f(x) 质量 */
    }
    
if (!n) 
    {
// if n has no value, there is problems...
//fprintf (stderr, "NOT NORMAL thresholdValue = 160\n");
        thresholdValue =160;
goto L;
    }
    
// do the otsu global thresholding method
    fmax =-1.0;
    n1 =0;
for (k =0; k <255; k++) 
    {
        n1 += ihist[k];
if (!n1) { continue; }
        n2 = n - n1;
if (n2 ==0) { break; }
        csum += k *ihist[k];
        m1 = csum / n1;
        m2 = (sum - csum) / n2;
        sb = n1 * n2 *(m1 - m2) * (m1 - m2);
/* bbg: note: can be optimized. */
if (sb > fmax)
        {
            fmax = sb;
            thresholdValue = k;
        }
    }
    
L:
for (i =0; i < h; i++) 
    {
        np = (unsigned char*)(image->imageData + image->widthStep*i);
for (j =0; j < w; j++) 
        {
if(np[j] >= thresholdValue)
                np[j] =255;
else np[j] =0;
        }
    }

//cout<<"The Threshold of this Image in Otsu is:"<<thresholdValue<<endl;
return(thresholdValue);
}

//核心算法
void HSV_Detect(IplImage* psrcimg,IplImage* pbackimg,IplImage* psrcgray,IplImage* pgrayimg)
{
	IplImage* psrcHSVimg=cvCreateImage(cvGetSize(psrcimg), 8,3);
	IplImage* pbackHSVimg=cvCreateImage(cvGetSize(psrcimg),8,3);
	IplImage* ptemp=cvCreateImage(cvGetSize(psrcimg), 8,3);
	IplImage* pmask=cvCreateImage(cvGetSize(psrcimg), 8,3);
	cvCvtColor(psrcimg,psrcHSVimg,CV_BGR2HSV);
	cvCvtColor(pbackimg,pbackHSVimg,CV_BGR2HSV);
	CvScalar srcscal;
	CvScalar backscal;
	CvScalar src_scal;
	CvScalar back_scal;
	CvScalar dst_scal;

	srcscal=cvAvg(psrcHSVimg,psrcgray);
	backscal=cvAvg(pbackHSVimg,psrcgray);

	//if(backscal.val[2]<120)
	//{
	//	cvReleaseImage(&psrcHSVimg);
	//	cvReleaseImage(&pbackHSVimg);
	//}
	double hightrate=(srcscal.val[2]+0.5*(backscal.val[2]-srcscal.val[2]))/backscal.val[2];//最高阈值

	double lowrate=srcscal.val[2]/backscal.val[2];//最低阈值
	double Vrate=0;
	for(int i=0;i<psrcimg->height;i++)
	{
		for(int j=0;j<psrcimg->width;j++)
		{
			src_scal=cvGet2D(psrcHSVimg,i,j);
			back_scal=cvGet2D(pbackHSVimg,i,j);
			Vrate=src_scal.val[2]/backscal.val[2];
			if((Vrate<lowrate)||Vrate>hightrate)
			{
				dst_scal.val[0]=255;dst_scal.val[1]=255;dst_scal.val[2]=255;
				cvSet2D(ptemp,i,j,dst_scal);
			}
			else
			{
				dst_scal.val[0]=0;dst_scal.val[1]=0;dst_scal.val[2]=0;
				cvSet2D(ptemp,i,j,dst_scal);
			}
		}
	}
	cvCopy(ptemp,pgrayimg,psrcgray);
}    

#if 0
int main(int argc, char** argv)
{
	std::string videoFile = "F:\\OPENCV项目\\陈智敏\\程序\\车流量统计程序\\Debug\\6.avi";
	cv::VideoCapture capture;
	capture.open(videoFile);
	if (!capture.isOpened())
	{
		//std::cout<<"read video failure"<<std::endl;
		return -1;
	}
	
	cv::BackgroundSubtractorMOG2 mog;
	
	cv::Mat foreground;
	cv::Mat background;
	int i,j;
	cv::Mat frame;
	/*capture.read(frame);*/
	/*cv::Mat pFrame(frame.rows,frame.cols,CV_8UC1,cv::Scalar(0));*/
	long frameNo = 0;
	float num;
	float num1,num2;

	uchar* p,*q,v;
	while (capture.read(frame))
	{
		cv::Mat pFrame(frame.rows,frame.cols,CV_8UC1,cv::Scalar(0));
		++frameNo;
		//std::cout<<frameNo<<std::endl;
		
		// 运动前景检测，并更新背景
		mog(frame, foreground, 0.01); 
		mog.getBackgroundImage(background); 
		for(j=0;j<frame.rows;j++)
		{
			//q=pFrame.ptr(j);
			for(i=0;i<frame.cols;i++)
			{
				//pFrame.data=p;
				//q=;
				//if(p[i]!=0)pFrame.at<uchar>(i,j)=p[i];
				/*else q[i]=0;*/
				/*if((int)v==0)*(pFrame.ptr(j)+i)=0;
				else *(pFrame.ptr(j)+i)=255;*/
				if(foreground.at<uchar>(j,i)!=0)
				{
						/*pFrame.at<cv::Vec3b>(j,i)[0]=frame.at<cv::Vec3b>(j,i)[0];
						pFrame.at<cv::Vec3b>(j,i)[1]=frame.at<cv::Vec3b>(j,i)[1];
						pFrame.at<cv::Vec3b>(j,i)[2]=frame.at<cv::Vec3b>(j,i)[2];*/
					num1=(int)(background.at<cv::Vec3b>(j,i)[1]+background.at<cv::Vec3b>(j,i)[2]);
					num2=(int)(frame.at<cv::Vec3b>(j,i)[1]+frame.at<cv::Vec3b>(j,i)[2]);
					num=num1/num2;
					if(num<0.88||frame.at<cv::Vec3b>(j,i)[1]>background.at<cv::Vec3b>(j,i)[1]||frame.at<cv::Vec3b>(j,i)[2]>background.at<cv::Vec3b>(j,i)[2]||frame.at<cv::Vec3b>(j,i)[0]>background.at<cv::Vec3b>(j,i)[0])
					{
						/*pFrame.at<cv::Vec3b>(j,i)[0]=frame.at<cv::Vec3b>(j,i)[0];
						pFrame.at<cv::Vec3b>(j,i)[1]=frame.at<cv::Vec3b>(j,i)[1];
						pFrame.at<cv::Vec3b>(j,i)[2]=frame.at<cv::Vec3b>(j,i)[2];*/
						/*pFrame.at<cv::Vec3b>(j,i)[0]=255;
						pFrame.at<cv::Vec3b>(j,i)[1]=255;
						pFrame.at<cv::Vec3b>(j,i)[2]=255;*/
						pFrame.at<uchar>(j,i)=255;
						
						
					}
					else
					{
						/*foreground.at<uchar>(j,i)=255;*/
						
					}
				}
				/*else
				{
					pFrame.at<cv::Vec3b>(j,i)[0]=0;
					pFrame.at<cv::Vec3b>(j,i)[1]=0;
					pFrame.at<cv::Vec3b>(j,i)[2]=0;
				}*/
				
			}
		}


		cv::erode(pFrame, pFrame, cv::Mat());
		cv::dilate(pFrame, pFrame, cv::Mat());
		// 腐蚀
		cv::erode(foreground, foreground, cv::Mat());
		
		// 膨胀
		cv::dilate(foreground, foreground, cv::Mat());
		
		/*mog.getBackgroundImage(background); */  // 返回当前背景图像

		//cv::absdiff(frame,background,pFrame);
		
		cv::imshow("frame",frame);
		cv::imshow("video", foreground);
		cv::imshow("background", background);
		cv::imshow("back", pFrame);

		if (cv::waitKey(25) > 0)
		{
			break;
		}
	}
	


	return 0;
}
#endif