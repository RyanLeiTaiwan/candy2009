/** File: car_extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2010/01/19
 ** Description: The implementations of car feature extraction
    Features:
      1. Rectangle features
      2. Edge Orientation Histograms (EOH)
      3. Edge Density (ED)
    Notice the use of integral histogram in computation.
 **/

#include <iostream>
#include <cstring>
#include <cassert>
#include <dirent.h>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h>
#include "parameters.h"
#include "car_extract.h"
#include "util.h"
using namespace std;

void extractAll(char *PATH_BASE, CvMat **POOL, int *N, int *blockCount) {
	DIR *dir;
	struct dirent *dp;
	char PATH[ MAX_PATH_LENGTH ];
	IplImage *img;
	
	/* count # of images */
	*N = countImages(PATH_BASE);
	cout << "Image count of " << PATH_BASE << ": " << *N << endl;
	/* allocate memory */
	*POOL = new CvMat[ *N ];
	
	try {
		if (!(dir = opendir(PATH_BASE))) {
			throw "Directory open exception";
		}
		
		int Iid = 0; /* Image ID */
		while (dp = readdir(dir)) {
			/* The full path is PATH_BASE + FILENAME */
			sprintf(PATH, "%s%s", PATH_BASE, dp->d_name);
			
			/* If it is an image file (read it in grayscale) */
			if (img = cvLoadImage(PATH, 0)) {
				cout << PATH;
				
				/* check image sizes */
				if (img->width != WINDOW_WIDTH || img->height != WINDOW_HEIGHT) {
					cerr << ": Image size should be " << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << ".\n";
					exit(EXIT_FAILURE);
				}
				/* For the first image: also count # of blocks */
				if (Iid == 0) {
					*blockCount = countBlocks(img);
				}
				/* Create data matrix for this image */
				CvMat *ptr = (*POOL) + Iid; // pointer to (*POOL)[ Iid ]
				ptr = cvCreateMat(*blockCount, FEATURE_COUNT, CV_32FC1);
				cvmSet(ptr, 2, 2, 50);
				Iid++;
				
				/* Extract features for this image */
				extractImg(img, ptr);
				cout << " extracted.\n";
				cvReleaseImage(&img);
			}
		}
		closedir(dir);
	}
	catch (const char * e) {
		cerr << "extractAll(): " << e << endl;
	}
	
}

int countBlocks(IplImage *img) {
	int ret = 0;
	/* try all possible sizes and positions within the image */
	for (int bHeight = BLOCK_MIN_SIZE; bHeight <= WINDOW_HEIGHT; bHeight += BLOCK_SIZE_INC) {
		for (int bWidth = BLOCK_MIN_SIZE; bWidth <= WINDOW_WIDTH; bWidth += BLOCK_SIZE_INC) {
			//int subTotal = 0;
			for (int yBeg = 0, yEnd = yBeg + bHeight - 1; yEnd < WINDOW_HEIGHT;
				 yBeg += BLOCK_POS_INC, yEnd += BLOCK_POS_INC) {
				for (int xBeg = 0, xEnd = xBeg + bWidth - 1; xEnd < WINDOW_WIDTH;
					xBeg += BLOCK_POS_INC, xEnd += BLOCK_POS_INC) {
					
					assert(yEnd <= WINDOW_HEIGHT && xEnd <= WINDOW_WIDTH);
					ret++;
					//subTotal++;
					//cout << bHeight << " x " << bWidth << ": (" << yBeg << "," << xBeg <<
					//") to (" << yEnd << "," << xEnd << ")\n";
				}
			}
			//cout << "Subtotal: " << subTotal << endl;
			//getchar();
		}
	}
	//cout << "Total: " << ret << endl;
	return ret;
}

/* Extract features from a single image */
/** Be careful with the PERFORMANCE in this function **/
void extractImg(IplImage *img, CvMat *data) {
	/* data is the pointer to a single matrix: POOL[ Iid ] */
	int Bid = 0; // block ID
	
//	/* CV_8UC1 image, CV_32FC1 image, integral image, image after normalization.
//	 * "2" means the "squared" version */
//	CvMat *img8U, *img, *img2, ii, ii2, *imgNorm, iiNorm;
//	int rows, cols;
//	
//	/* Convert IplImage *imgIn into CvMat *img */
//	CvSize size = cvGetSize(imgIn);
//	img8U = cvCreateMatHeader(size.height, size.width, CV_8UC1);
//	cvGetMat(imgIn, img8U);
//	/* make it CV_32FC1 */
//	rows = img8U->rows;
//	cols = img8U->cols;
//	img = cvCreateMat(rows, cols, CV_32FC1);
//	cvConvert(img8U, img);
//
//	/* Copy img for later use */
//	img2 = cvCloneMat(img);
//	cvPow(img, img2, 2);
	
	/* [1] For rectangle features, use cvIntegral() to compute:
	 * sum: integral image of img
	 * sqSum: integral imgage for squared pixel values
	 * tiltedSum: integral for the 45' rotated image
	 */
	IplImage *sum, *sqSum, *tiltedSum; // all are in CV_64F
	int W = imgIn->width, H = imgIn->height;
	sum = cvCreateImage(cvSize(W+1, H+1), IPL_DEPTH_64F, 1);
	sqSum = cvCreateImage(cvSize(W+1, H+1), IPL_DEPTH_64F, 1);
	tiltedSum = cvCreateImage(cvSize(W+1, H+1), IPL_DEPTH_64F, 1);
	cvIntegral(imgIn, sum, sqSum, tiltedSum);
	
	
	printMat(imgIn, "imgIn", 0, 9, 0, 9);
	printMat(sum, "sum", 0, 9, 0, 9);
	printMat(sqSum, "sqSum", 0, 9, 0, 9);	
	printMat(tiltedSum, "tiltedSum", 0, 9, 0, 9);	

	
	
	
	
	getchar();

}

