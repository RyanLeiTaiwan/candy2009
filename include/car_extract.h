/** File: car_extract.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Update: 2010/01/09
 ** Description: Header file of car feature extraction
 **/

/* Internal parameters */
/* feature-ralated constants */
#define FEATURE_COUNT 15  
#define REC_BEGIN 0
#define EOH_BEGIN 5
#define ED_BEGIN 14

/* Function declrations */

/* Feature extraction of the entire directory */
void extractAll(char *PATH_BASE, CvMat **POOL, int *N, int *blockCount);
/* Feature extraction of a single image */
void extractImg(IplImage *img, CvMat *data);
/* Count # of blocks in the image according to the internal parameters */
int countBlocks(IplImage *img);