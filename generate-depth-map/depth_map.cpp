#include "cvaux.h"
#include "highgui.h"
#include <limits.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <cv.h>
#include <cxcore.h>
#include <cvblob.h>

IplImage* vis_disp = NULL;


CvMat* img1r ;
CvMat* img2r ;

CvMat* mx1;
		
CvMat* mx2;
		
CvMat* my1;
		
CvMat* my2;

int maxdisp = 20;

void calc_dense_disparity(IplImage* image1, IplImage* image2 , CvMat Q , CvStereoBMState *BMState);



#define SOURCE_AVI 1
#define SOURCE_CAM 2

#ifndef FLTK
int main( int argc, char** argv )
{
    int source = SOURCE_CAM; 
	//int source = SOURCE_AVI; 

    CvCapture* captureL; //left 
    CvCapture* captureR; //right

    IplImage* left_gray = 0;
     IplImage* right_gray = 0;
	IplImage* tmp_frameL;
	IplImage* tmp_frameR;



    if( source == SOURCE_CAM ) //input from cameras
    {
       captureL = cvCaptureFromCAM( 0 ); //first camera
        captureR = cvCaptureFromCAM( 1 ); //second camera
    }
    else
    {
		////captureL = cvCaptureFromAVI( "D:\\vids\\L.avi" ); //first avi file
		//captureR = cvCaptureFromAVI( "D:\\vids\\R.avi" ); //second avi file

		//captureL = cvCaptureFromAVI( "K:\\New folder\\L1.avi" ); //first avi file
		//captureR = cvCaptureFromAVI( "K:\\New folder\\R1.avi" ); //second avi file 
    }   
    
    if( !captureL || !captureR ) 
    {  printf("can't run camera/avi\n");  return 1; }
        
    cvvNamedWindow( "source left", 1 );
    cvvNamedWindow( "source right", 1 );
    
	
	CvMat* _Q = (CvMat*)cvLoad( "Q.xml" );
	CvMat Q = *_Q;

	mx1 = (CvMat*)cvLoad( "mx1.xml" );
		
	mx2 = (CvMat*)cvLoad( "mx2.xml" );
		
	my1 = (CvMat*)cvLoad( "my1.xml" );
		
	my2 = (CvMat*)cvLoad( "my2.xml" );

	IplImage* imageL = cvQueryFrame(captureL);
	IplImage* imageR = cvQueryFrame(captureR);

	img1r = cvCreateMat( imageL->height,imageL->width, CV_8U ); 
	img2r = cvCreateMat( imageL->height,imageL->width, CV_8U );


	CvStereoBMState *BMState = cvCreateStereoBMState(); 
        assert(BMState != 0); 
        BMState->preFilterSize=41; 
        BMState->preFilterCap=31; 
        BMState->SADWindowSize=41; 
        BMState->minDisparity=0; 
        BMState->numberOfDisparities=64; 
        BMState->textureThreshold=10;
        BMState->uniquenessRatio=15; 
		//BMState->trySmallerWindows;
    for(;;)
    {
        int cmd = cvvWaitKeyEx( 0, 1 );
        if( cmd == '\x1b' )
            break;

        //frame processing
        //cvGrabFrame( captureL );
        //int time2 = clock();
        //cvGrabFrame( captureR );
        //int time1 = clock();
        //printf("time diff = %d", time1 - time2);
        IplImage* frameL = cvQueryFrame(captureL);
        IplImage* frameR = cvQueryFrame(captureR);

        if( !left_gray && !right_gray )
        {
            left_gray = cvCreateImage(cvSize( frameL->width,frameL->height), IPL_DEPTH_8U, 1 );
            right_gray = cvCreateImage(cvSize( frameL->width,frameL->height), IPL_DEPTH_8U, 1 );   
            left_gray->origin = right_gray->origin = frameL->origin;
        }
    
        cvCvtColor( frameL, left_gray, CV_BGR2GRAY );
		cvCvtColor( frameR, right_gray, CV_BGR2GRAY );
        
        calc_dense_disparity( left_gray, right_gray ,Q , BMState); 

        cvvShowImage( "source left", frameL );
        cvvShowImage( "source right", frameR );


    }
   

    cvReleaseCapture( &captureL );
    cvReleaseCapture( &captureR );

    cvReleaseImage( &left_gray );
    cvReleaseImage( &right_gray );

    cvReleaseImage(&vis_disp);

    
    return 0;
}

#endif

void calc_dense_disparity(IplImage* image1, IplImage* image2 , CvMat Q , CvStereoBMState *BMState)
{


        cvNamedWindow("disparity", 1);
        cvCreateTrackbar( "maxdisp", "disparity", &maxdisp, 255, 0 );
        vis_disp = cvCreateImage(cvSize(image1->width, image1->height), IPL_DEPTH_8U, 1);

	//cvRemap( image1, img1r, mx1, my1 ); 
    //cvRemap( image2, img2r, mx2, my2 );

    vis_disp->origin = image1->origin;
    cvFindStereoCorrespondence( image1, image2,CV_DISPARITY_BIRCHFIELD,vis_disp,maxdisp );
    cvConvertScale(vis_disp, vis_disp, 255.f/maxdisp);

	cvSmooth(vis_disp,vis_disp,CV_BLUR);
	/*vis_disp = cvCreateImage(cvSize(img1r->width, img2r->height), IPL_DEPTH_16S , 1);
	IplImage* vdisp = cvCreateImage(cvSize(image1->width, image1->height), CV_8U , 1);
	cvFindStereoCorrespondenceBM( img1r, img2r, vis_disp , BMState); 
	cvConvertScale(vis_disp, vis_disp, 255.f/maxdisp);
	cvNormalize( vis_disp, vdisp, 0, 256, CV_MINMAX ); */

    cvvShowImage("disparity", vis_disp);

	//IplImage *im32 = cvCreateImage(cvSize(image1->width, image1->height), IPL_DEPTH_16S, 1);
	//cvConvertScale(vis_disp, im32, 255.f/maxdisp);
	//CvMat* depth = cvCreateMat( image1->height, image1->width, CV_32FC3 ); 
	//cvReprojectImageTo3D( im32 , depth,&Q); 
	//
	//cvNamedWindow( "depth" ); 
	//cvShowImage( "depth", depth ); 
}

