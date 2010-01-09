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
#define BLOCK_MIN_SIZE 8 /* minumum block size */
#define BLOCK_SIZE_INC 8 /* block size increment */
#define BLOCK_POS_INC 8 /* block position increment */

