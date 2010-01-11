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
#include <cmath>
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
			for (int y = 0; y + bHeight - 1 < WINDOW_HEIGHT; y += BLOCK_POS_INC) {
				for (int x = 0; x + bWidth - 1 < WINDOW_WIDTH; x += BLOCK_POS_INC) {
					
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
	
	/* Timer */
	clock_t tic, toc;
	tic = clock();
	
	/* [1.1] For rectangle features: first, perform Mean and variance normalization */
	int W = img->width, H = img->height, N = W * H;
	IplImage *sqImg, *normImg; // square image and normalized image
	
	normImg = cvCreateImage(cvSize(W, H), IPL_DEPTH_32F, 1);
	cvConvert(img, normImg);
	sqImg = cvCloneImage(normImg);
	cvPow(sqImg, sqImg, 2);	
	meanVarNorm(normImg, cvSum(img).val[ 0 ], cvSum(sqImg).val[ 0 ], N);
	cvReleaseImage(&sqImg);

	/* [1.2] Use cvIntegral() to compute the integral image of normImg */
	IplImage *normSum;  // in "IPL_DEPTH_64F"
	normSum = cvCreateImage(cvSize(W + 1, H + 1), IPL_DEPTH_64F, 1);
	cvIntegral(normImg, normSum);
	
	printMat(normImg, "normImg", 0, 14, 0, 14);
	printMat(normSum, "normSum", 1, 15, 1, 15);
	
	
	toc = clock();
	runningTime(tic, toc);
	getchar();

}

/* Mean and variance normalization given the "sum", the sum of squares "sqSum", and # of pixels N */
/* Both sum and sqSum are double values, not IplImages */
/* "normImg" is both the source and destination */
void meanVarNorm(IplImage *normImg, double sum, double sqSum, int N) {
	double mean, stdev;
	mean = sum / (double)N;
	stdev = sqrt((sqSum - N * mean * mean) / N);
	cout << "mean = " << mean << ", stdev = " << stdev << ".\n";
	
	/* normalized */
	cvSubS(normImg, cvScalar(mean), normImg);
	cvScale(normImg, normImg, 1 / stdev);
}

/* Feature extraction of a single block */
void extractBlock(int Bid, CvMat *data, IplImage *normSum, int x, int W, int y, int H) {
	
	;
}