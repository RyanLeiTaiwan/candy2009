/** File: extract.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Update: 2010/04/04
 ** Description: Header file of car feature extraction
 **/

#include "util.h"
#define TIMER_EXTRACT 0  // Count the time for extracting one image

/* Feature extraction of the entire directory */
void extractAll(char *PATH_BASE, CvMat *&POOL, int &N, int &blockCount);
/* Feature extraction of a single image */
void extractImg(IplImage *img, CvMat *POOL, int Iid, int blockCount);
/* Feature extraction of a single block */
void extractBlock(int Iid, int blockCount, CvMat *POOL, int Bid, float *&data, IplImage *normSum, int x, int W, int y, int H);
/* Count # of blocks in the image according to the internal parameters */
int countBlocks(IplImage *img);

