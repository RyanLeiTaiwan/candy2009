#include <iostream>
#include <highgui.h>
#include <cxcore.h>
#include <cv.h>
using namespace std;


int main( int argc, char *argv[] ) {
	if ( argc != 3 ) {
		cout << "Usage: hello ImageFile VideoFile\n";
		return -1;
	}
	/* Example 2-1 */
	IplImage *img = cvLoadImage( argv[1] ); 
	cvNamedWindow( "Example1", CV_WINDOW_AUTOSIZE ); 
	cvShowImage( "Example1", img ); 
	cvWaitKey( 1000 ); 
	cvReleaseImage( &img ); 
	
	/* Example 2-2 */
	IplImage *frame;
	cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );
	CvCapture *capture = cvCreateFileCapture( argv[2] );
	while ( true ) {
		frame = cvQueryFrame( capture );
		if ( !frame ) break;
		cvShowImage( "Example2", frame );
		char c = cvWaitKey( 33 );
		if ( c == 32 ) break; /* Space bar */
		
	}

	IplImage *resized;
	/* Example 2-9: Input from a "camera" */
	cvNamedWindow("USB Camera");
	CvCapture *camera = cvCaptureFromCAM( 0 );
	assert( camera != NULL );
	while ( true ) {
		frame = cvQueryFrame( camera );
		
		resized = cvCreateImage(cvSize(800, 600), IPL_DEPTH_8U, 3);
		cvResize(frame, resized);
		cvShowImage("USB Camera", resized);
		if ( cvWaitKey( 10 ) == 32 ) break;

		cvReleaseImage(&resized);
	}
	
	
	/* End of program */
	cvReleaseCapture( &camera );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "USB Camera" );
	cvDestroyWindow( "Example2" );

	cvDestroyWindow( "Example1" );
	
	return 0;
}
