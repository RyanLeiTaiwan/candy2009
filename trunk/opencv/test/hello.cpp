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

	/* Example 2-2 */
	cvNamedWindow( "Example2", CV_WINDOW_AUTOSIZE );
	CvCapture *capture = cvCreateFileCapture( argv[2] );
	IplImage *frame;
	while ( true ) {
		frame = cvQueryFrame( capture );
		if ( !frame ) break;
		cvShowImage( "Example2", frame );
		char c = cvWaitKey( 33 );
		if ( c == 32 ) break; /* Space bar */
	}

	/* Example 2-9: Input from a "camera" */
	cvNamedWindow( "USB Camera", CV_WINDOW_AUTOSIZE );
	cvWaitKey( 1000 );
	CvCapture *camera = cvCaptureFromCAM( 0 );
	assert( camera != NULL );
	while ( true ) {
		frame = cvQueryFrame( camera );
		cvShowImage( "USB Camera", frame );
		if ( cvWaitKey( 0 ) == 32 ) break;
	}
	

	/* End of program */
	cvReleaseCapture( &camera );
	cvReleaseCapture( &capture );
	cvDestroyWindow( "USB Camera" );
	cvDestroyWindow( "Example2" );
	cvReleaseImage( &img ); 
	cvDestroyWindow( "Example1" );

	return 0;
}


