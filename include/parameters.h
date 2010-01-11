/** File: global.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/25
 ** Update: 2010/01/09
 ** Description: Constants or Parameters used globally.
 **/

/** x is always horizontal, y is always vertical, but since OpenCV data is store in row-major order,
 ** for loops may have to be in [for y ... for x] order **/

/** Constants **/


/** System parameters: may be edited **/
/* Uncomment either line according to your OS */
#define Mac 1
//#define Linux 1
//#define Windows 1
#ifdef Mac || Linux
	#define Unix 1
#else
	#define Unix 0
#endif
#define MAX_PATH_LENGTH 200

/** Project parameters: may be edited **/
#define WINDOW_WIDTH 128
#define WINDOW_HEIGHT 128
#define BLOCK_MIN_SIZE 16  // minumum block size
#define BLOCK_SIZE_INC 8  // block size increment
#define BLOCK_POS_INC 16  // block position increment

#define BIN_NUM 9  // # of bins in EOH (signed orientation: 360 degrees)
#define BIN_SIZE 40  // degrees in each bin (signed orientation => should be 360 / BIN_NUM)
#define HALF_BIN_SIZE 20  // should be BIN_SIZE / 2

#define F_target 0.001  // target overall false positive rate
#define d_minA 0.999  // minimum acceptable detection rate per AdaBoost stage
#define f_maxA 0.50  // maximum acceptable false positive rate per AdaBoost stage
#define d_minM 1.00  // minimum acceptable detection rate per meta stage
#define ni 1  // # of AdaBoost stages before meeting a meta stage
