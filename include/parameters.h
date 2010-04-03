/** File: global.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/25
 ** Update: 2010/04/04
 ** Description: Constants or Parameters used globally.
 **/

/** x is always horizontal, y is always vertical, but since OpenCV data is store in row-major order,
 ** for loops may have to be in [for y ... for x] order **/

/** Constants **/
#define FEATURE_COUNT 5


/** System parameters: may be edited **/
/* Uncomment either line according to your OS */
#define Mac
//#define Linux
//#define Windows

#ifdef Windows
	#define Unix 0
#else
	#define Unix 1
#endif

#ifdef Mac
	// Work around some Xcode 3.2 bug ... orz
	#define _GLIBCXX_FULLY_DYNAMIC_STRING 1
	#undef _GLIBCXX_DEBUG
	#undef _GLIBCXX_DEBUG_PEDANTIC
#endif
#define MAX_PATH_LENGTH 100
#define MAX_LINE_LENGTH 50

/** Run-time options: may be edited **/
#define GETCHAR 0  // Encounter getchar()'s in training, i.e., verbose mode.
#define CHECKNaN 1  // Check for NaN values after feature extraction
#define CVWINDOW_X 700  // x position of OpenCV's NamedWindow
#define CVWINDOW_Y 100  // y position of OpenCV's NamedWindow
#define RESIZE_WIDTH 512  // width of the resized image
#define RESIZE_HEIGHT 512  // height of the resized image
#define FONT_X 220  // x position to put text on the image (single-image detection mode)
#define FONT_Y 110  // y position to put text on the image (single-image detection mode)

/** Project parameters: may be edited **/
#define WINDOW_WIDTH 128  // detection window width
#define WINDOW_HEIGHT 128  // detection window height
#define BLOCK_MIN_SIZE 16  // minumum block size
#define BLOCK_SIZE_INC 8  // block size increment
#define BLOCK_POS_INC 16  // block position increment

#define EPSILON 1E-9  // a small positive number to avoid division by zero
#define BIN_NUM 9  // # of bins in EOH (signed orientation: 360 degrees)
#define BIN_SIZE 40  // degrees in each bin (signed orientation => should be 360 / BIN_NUM)
#define HALF_BIN_SIZE 20  // should be BIN_SIZE / 2

#define F_target 1E-30  // target overall false positive rate -> 0
#define d_minA 0.99  // minimum acceptable detection rate per AdaBoost stage (implemented as 100%)
#define f_maxA 0.50  // maximum acceptable false positive rate per AdaBoost stage
#define d_minM 1.00  // minimum acceptable detection rate per meta stage
#define ni 1  // # of AdaBoost stages before meeting a meta stage


