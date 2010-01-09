/** File: util.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/09
 ** Modification: XXXX/XX/XX
 ** Description: Utility functions
 **/

#include <iostream>
#include <cstring>
#include <dirent.h>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
#include "util.h"
using namespace std;


/* Count the number of images in a directory */
int	countImages(const char *PATH_BASE) {
	int ret = 0;
	DIR *dir;
	struct dirent *dp;
	
	try {
		if (!(dir = opendir(PATH_BASE))) {
			throw "Directory open exception.";
		}
		if (Unix) {
			/* In unix-like systems, hidden file names start with '.' */
			while (dp = readdir(dir)) {
				if (dp->d_name[ 0 ] != '.') {
					ret++;
				}
			}
		}
		else {
			/* In Windows system, ignore the "desktop.ini" */
			while (dp = readdir(dir)) {
				if (!strcmp(dp->d_name, "desktop.ini")) {
					ret++;
				}
			}
		}

	}
	catch (const char *e) {
		cerr << "countImages(): " << e << endl;
	}

	return ret;
}

/* Print a matrix */
/* The idea is taken from http://blog.weisu.org/2007/11/opencv-print-matrix.html */
/** For single-channel 2D arrays only **/
void printMat(CvMat *A, const char *name) {
	/* Only the default, not necessarily the case */
	int rowBeg = 0, rowEnd = A->rows - 1, colBeg = 0, colEnd = A->cols - 1;
	cout << endl << name << "(" << rowBeg << ":" << rowEnd << ", " << colBeg << ":" << colEnd << ") =\n\n";
	for (int i = rowBeg; i <= rowEnd; i++) {
		switch (CV_MAT_DEPTH(A->type)) {
			case CV_32F:
			case CV_64F:
				for (int j = colBeg; j <= colEnd; j++)
					printf("%7.3f ", (float)cvGetReal2D(A, i, j));
				break;
			case CV_8U:
			case CV_8S:
			case CV_16U:
			case CV_16S:
			case CV_32S:
				for (int j = colBeg; j <= colEnd; j++)
					printf("%4d ", (int)cvGetReal2D(A, i, j));
				break;
			default:
				cerr << "printMat(): Matrix type not supported.\n";
				exit(EXIT_FAILURE);
				break;
		}
		cout << endl;
	}
	cout << endl;
}
