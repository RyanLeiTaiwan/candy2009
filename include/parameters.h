/** File: global.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/25
 ** Update: 2009/12/26
 ** Description: Constants or Parameters used globally.
 **/

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
#define WINDOW_HEIGHT 96 // 4 : 3

