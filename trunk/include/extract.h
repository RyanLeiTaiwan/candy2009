/** File: extract.h
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Update: 2010/01/09
 ** Description: Header file of car feature extraction
 **/

#include "util.h"

/* Feature extraction of the entire directory */
void extractAll(char *PATH_BASE, CvMat *POOL, int &N, int &blockCount);
/* Feature extraction of a single image */
void extractImg(IplImage *img, float *&ptr);
/* Feature extraction of a single block */
void extractBlock(int Bid, float *&data, IplImage *normSum, int x, int W, int y, int H);
/* Count # of blocks in the image according to the internal parameters */
int countBlocks(IplImage *img);

