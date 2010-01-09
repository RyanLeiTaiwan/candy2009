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
	cout << "\nImage count of " << PATH_BASE << ": " << *N << endl;
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
				(*POOL)[ Iid ] = cvMat(*blockCount, FEATURE_COUNT, CV_32FC1);
				extractImg(img, &((*POOL)[ Iid++ ]));
				cout << " extracted.\n";
				cvReleaseImage(&img);
			}
		}
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
			for (int yBeg = 0, yEnd = yBeg + bHeight - 1; yEnd < WINDOW_HEIGHT;
				 yBeg += BLOCK_POS_INC, yEnd += BLOCK_POS_INC) {
				for (int xBeg = 0, xEnd = xBeg + bWidth - 1; xEnd < WINDOW_WIDTH;
					xBeg += BLOCK_POS_INC, xEnd += BLOCK_POS_INC) {
					assert(yEnd <= WINDOW_HEIGHT && xEnd <= WINDOW_WIDTH);
			
					ret++;
					//cout << bHeight << " x " << bWidth << ": (" << yBeg << "," << xBeg <<
					//") to (" << yEnd << "," << xEnd << ")\n";
				}
			}
		}
	}
	//cout << "Total: " << ret << endl;
	return ret;
}

void extractImg(IplImage *img, CvMat *DATA) {
	/* DATA is a pointer to a single CvMat */
	float val1[] = { 1.1, 1.2, 1.3, 1.4, 2.1, 2.2, 2.3, 2.4, 3.1, 3.2, 3.3, 3.4 };
	CvMat matrix1 = cvMat(3, 4, CV_32FC1, val1);
	printMat(&matrix1, "matrix1");
	int val2[] = { 1, -2, 3, -4, 5, -6, 7, -8, 9, -10, 11, -12 };
	CvMat matrix2 = cvMat(3, 4, CV_32SC1, val2);
	printMat(&matrix2, "matrix2");

}

