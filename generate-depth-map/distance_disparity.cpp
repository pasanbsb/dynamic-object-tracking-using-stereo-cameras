//#include "stdafx.h"


#include "stdlib.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <math.h>
#include "cv.h"
#include "cvaux.h"
#include "highgui.h"
// OpenCV includes.
#include "cv.h"
#include "highgui.h"
//#pragma comment(lib,"cv.lib")
//#pragma comment(lib,"cxcore.lib")
//#pragma comment(lib,"highgui.lib")

using namespace std;

IplImage* greyImage;
CvPoint pt1, pt2,c;
CvFont font;
char wow[65];
CvRect bndRect;
CvSize imgSize;
CvBGStatModel* bgs(IplImage* image);
float findContoursL( IplImage* image , CvBGStatModel* bg_model , CvPoint &center);
float findContoursR( IplImage* image , CvBGStatModel* bg_model , CvPoint &center);
float pi = 3.14;
float findDistance(float angleL,float angleR);

int main(int argc, char* argv[])
{

	//cvNamedWindow("image", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("bgs", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("L", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("greyImageL", CV_WINDOW_AUTOSIZE);

	cvNamedWindow("R", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("greyImageR", CV_WINDOW_AUTOSIZE);

	CvCapture *inputL;
	CvCapture *inputR;

	//inputL = cvCaptureFromCAM(0);
	//inputR = cvCaptureFromCAM(1);
	inputL= cvCaptureFromAVI("D:\\vids\\L.avi");
	inputR= cvCaptureFromAVI("D:\\vids\\R.avi");

	CvPoint centerL = cvPoint(0,0);
	CvPoint centerR = cvPoint(0,0); 
	
	IplImage* frameL = cvQueryFrame(inputL);
	IplImage* frameR = cvQueryFrame(inputR);
	IplImage* colourImageL;
	IplImage* colourImageR;
	
	bndRect = cvRect(0,0,0,0);

	imgSize = cvGetSize(frameL); 
	CvBGStatModel* bg_modelL = bgs(frameL);
	CvBGStatModel* bg_modelR = bgs(frameR);

	for(;;)
	{
		colourImageL = cvQueryFrame(inputL);
		colourImageR = cvQueryFrame(inputR);

		float angleL = findContoursL(colourImageL,bg_modelL,centerL);
		float angleR = findContoursR(colourImageR,bg_modelR,centerR);

		//float disparity = (float)(centerL.x - centerR.x);
		//float d = (0.6f * 0.0025f) / (disparity * 640.0f);


		float disparity = (float)((320 + centerL.x) - (320 - centerR.x));
		//float d = tan((pi / 2.0f) - atan(disparity/2.5)) * 60; 

		float d = 0.008 * (0.6 * disparity);
		printf("%f\n",d);
	/*	char buffer[128];
		sprintf(buffer, "(%d,%d)", center.x,center.y);
		cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3, 0, 1);
      	cvPutText(colourImageL, buffer, center , &font, cvScalar(0, 0, 300));*/

		//cvShowImage("image", colourImageL);

		cvWaitKey(10);

	}

	cvReleaseImage(&greyImage);
	//cvDestroyWindow("L");
	//cvDestroyWindow("greyImageL");
	//cvDestroyWindow("R");
	//cvDestroyWindow("greyImageR");
	cvDestroyWindow("image");

	cvReleaseCapture(&inputL);
	cvReleaseCapture(&inputR);



return 0;

}

CvBGStatModel* bgs(IplImage* image){

 
	cvThreshold(image, image, 70, 255, CV_THRESH_BINARY);
	CvBGStatModel* bg_model = cvCreateGaussianBGModel( image );
	return bg_model;

}

float findContoursL( IplImage* colourImage , CvBGStatModel* bg_model , CvPoint &center){

		float angle = 0.0f;
		greyImage = cvCreateImage( imgSize, IPL_DEPTH_8U, 1);

		cvUpdateBGStatModel( colourImage, bg_model );
		greyImage = cvCloneImage(bg_model->foreground);
		cvSmooth(greyImage,greyImage,CV_BLUR);
		cvThreshold(greyImage, greyImage, 70, 255, CV_THRESH_BINARY);
		
		cvDilate(greyImage, greyImage, NULL, 18);
		cvErode(greyImage, greyImage, 0, 10);


		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvFindContours( greyImage, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL,CV_LINK_RUNS );

		for( ; contour != 0; contour = contour->h_next )
		{
			bndRect = cvBoundingRect(contour, 0);

			pt1.x = bndRect.x;
			pt1.y = bndRect.y;
			pt2.x = bndRect.x + bndRect.width;
			pt2.y = bndRect.y + bndRect.height;

			c.x = bndRect.x + bndRect.width/2;
			c.y = bndRect.y + bndRect.height/2;
			cvRectangle(colourImage, pt1, pt2, CV_RGB(255,0,0), 2);
			cvCircle(colourImage,c,1,CV_RGB(0,255,0),2);

			center = c;
			//printf("%d\n",center.y);
			char buffer[128];
			float diff = pt2.x - pt1.x;
			angle = ((45.0f/2.0f)/320.0f)*(diff-320.0f);

			sprintf(buffer, "(%d,%d)", c.x,c.y);
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3, 0, 1);
      		cvPutText(colourImage, buffer, c , &font, cvScalar(0, 0, 300));
			
		
		}

		cvShowImage("L", colourImage);
		//cvShowImage("bgs", greyImage);
		return angle;

}

float findContoursR( IplImage* colourImage , CvBGStatModel* bg_model , CvPoint &center){

		greyImage = cvCreateImage( imgSize, IPL_DEPTH_8U, 1);
		float angle = 0.0f;

		cvUpdateBGStatModel( colourImage, bg_model );
		greyImage = cvCloneImage(bg_model->foreground);
		cvSmooth(greyImage,greyImage,CV_BLUR);
		cvThreshold(greyImage, greyImage, 70, 255, CV_THRESH_BINARY);
		
		cvDilate(greyImage, greyImage, NULL, 18);
		cvErode(greyImage, greyImage, 0, 10);

		

		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvFindContours( greyImage, storage, &contour, sizeof(CvContour), CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE );

		for( ; contour != 0; contour = contour->h_next )
		{
			bndRect = cvBoundingRect(contour, 0);

			pt1.x = bndRect.x;
			pt1.y = bndRect.y;
			pt2.x = bndRect.x + bndRect.width;
			pt2.y = bndRect.y + bndRect.height;

			c.x = bndRect.x + bndRect.width/2;
			c.y = bndRect.y + bndRect.height/2;
			cvRectangle(colourImage, pt1, pt2, CV_RGB(255,0,0), 2);
			cvCircle(colourImage,c,1,CV_RGB(0,255,0),2);

			center = c;

			char buffer[128];
			float diff = pt2.x - pt1.x;
			angle = ((45.0f/2.0f)/320.0f)*(diff-320.0f);

			sprintf(buffer, "(%d,%d) - angle = %.2f", c.x,c.y,angle);
			cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.3, 0.3, 0, 1);
     		cvPutText(colourImage, buffer, c , &font, cvScalar(0, 0, 300));

			
		}

		cvShowImage("R", colourImage);
		//cvShowImage("greyImageR", greyImage);

		return angle;

}

float findDistance(float angleL,float angleR){

	float distance = (tan(pi/2 - angleL) * tan(pi/2 - angleR) * 6) / (tan(pi/2 - angleL) + tan(pi/2 - angleR));
	//printf("%f\n",distance);
	return distance;
} 