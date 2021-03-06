/** File: util.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/09
 ** Modification: 2010/04/04
 ** Description: Utility functions
 **/


#include "util.h"

/* Print an error message in cerr */
void error(const char *msg) {
	cerr << msg << endl;
	exit(EXIT_FAILURE);
}

/* Echo the operating system set by #define */
void echoOS() {
	cout << "====  OS set in \"parameters.h\": ";
#ifdef Mac
	cout << "Mac  ====\n\n";
	return;
#endif
#ifdef Linux
	cout << "Linux  ====\n\n";
	return;
#endif
#ifdef Windows
	cout << "Windows  ====\n\n";
	return;
#endif
	
	/* None of the above */
	cout << "Unknown  ====\n\n";
	error("echoOS(): Operating system not set. Please check include/parameters.h");
	
}

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
			/* On unix-like systems, hidden file names start with '.' */
			while ((dp = readdir(dir))) {
				if (dp->d_name[ 0 ] != '.') {
					ret++;
				}
			}
		}
		else {
			/* On Windows system, ignore ".*", "desktop.ini", "Thumbs.db", and ".DS_Store" */
			const char *str1 = "desktop.ini";
			const char *str2 = "Thumbs.db";
			const char *str3 = ".DS_Store";
			while ((dp = readdir(dir))) {
				if (dp->d_name[ 0 ] != '.' && strcmp(dp->d_name, str1) && strcmp(dp->d_name, str2) && strcmp(dp->d_name, str3)) {
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

/* Print a single-channel 2D matrix with a specified range */
/* The idea is taken from http://blog.weisu.org/2007/11/opencv-print-matrix.html */
void printMat(CvMat *A, const char *name, int rowBeg, int rowEnd, int colBeg, int colEnd) {
	/* Check for validity of the privided (row, col) range */
	if (rowBeg < 0 || rowEnd >= A->rows || rowBeg > rowEnd || colBeg < 0 || colEnd >= A->cols || colBeg > colEnd) {
		error("printMat(): Invalid range of (rowBeg, rowEnd, colBeg, colEnd)." );
	}

	cout << endl << name << "(" << rowBeg << ":" << rowEnd << ", " << colBeg << ":" << colEnd << ") =\n\n";
	for (int i = rowBeg; i <= rowEnd; i++) {
		switch (CV_MAT_DEPTH(A->type)) {
			case CV_32F:
			case CV_64F:
				for (int j = colBeg; j <= colEnd; j++)
					/* Use C's printf() for convenience :p */
					printf("%6.2f ", (float)cvGetReal2D(A, i, j));
				break;
			case CV_8U:
			case CV_8S:
			case CV_16U:
			case CV_16S:
			case CV_32S:
				for (int j = colBeg; j <= colEnd; j++)
					/* Use C's printf() for convenience :p */
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

/* Print a single-channel 2D matrix without a specified range */
void printMat(CvMat *A, const char *name) {
	int rowBeg = 0, rowEnd = A->rows - 1, colBeg = 0, colEnd = A->cols - 1;

	cout << endl << name << "(" << rowBeg << ":" << rowEnd << ", " << colBeg << ":" << colEnd << ") =\n\n";
	for (int i = rowBeg; i <= rowEnd; i++) {
		switch (CV_MAT_DEPTH(A->type)) {
			case CV_32F:
			case CV_64F:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%6.2f ", (float)cvGetReal2D(A, i, j));
				break;
			case CV_8U:
			case CV_8S:
			case CV_16U:
			case CV_16S:
			case CV_32S:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
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

/* Print a single-channel image with a spcified range */
/* This is the overloaded version for IplImage */
void printMat(IplImage *A, const char *name, int rowBeg, int rowEnd, int colBeg, int colEnd) {
	/* If the (row, col) range is all in default => Use the full range */
	if (rowBeg == -1 && rowEnd == -1 && colBeg == -1 && colEnd == -1) {
		rowBeg = 0; rowEnd = A->height - 1; colBeg = 0; colEnd = A->width - 1;
	}

	/* Check for validity of the privided (row, col) range */
	if (rowBeg < 0 || rowEnd >= A->height || rowBeg > rowEnd || colBeg < 0 || colEnd >= A->width || colBeg > colEnd) {
		error("printMat(): Invalid range of (rowBeg, rowEnd, colBeg, colEnd)." );
	}

	cout << endl << name << "(" << rowBeg << ":" << rowEnd << ", " << colBeg << ":" << colEnd << ") =\n\n";
	for (int i = rowBeg; i <= rowEnd; i++) {
		switch (A->depth) {
			case IPL_DEPTH_32F:
			case IPL_DEPTH_64F:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%6.2f ", (float)cvGetReal2D(A, i, j));
				break;
			case IPL_DEPTH_8U:
			case IPL_DEPTH_8S:
			case IPL_DEPTH_16U:
			case IPL_DEPTH_16S:
			case IPL_DEPTH_32S:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%4d ", (int)cvGetReal2D(A, i, j));
				break;
			case IPL_DEPTH_1U:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%1d ", (int)cvGetReal2D(A, i, j));
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

/* Print a single-channel image without a spcified range */
/* This is the overloaded version for IplImage */
void printMat(IplImage *A, const char *name) {
	int	rowBeg = 0, rowEnd = A->height - 1, colBeg = 0, colEnd = A->width - 1;

	cout << endl << name << "(" << rowBeg << ":" << rowEnd << ", " << colBeg << ":" << colEnd << ") =\n\n";
	for (int i = rowBeg; i <= rowEnd; i++) {
		switch (A->depth) {
			case IPL_DEPTH_32F:
			case IPL_DEPTH_64F:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%6.2f ", (float)cvGetReal2D(A, i, j));
				break;
			case IPL_DEPTH_8U:
			case IPL_DEPTH_8S:
			case IPL_DEPTH_16U:
			case IPL_DEPTH_16S:
			case IPL_DEPTH_32S:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%4d ", (int)cvGetReal2D(A, i, j));
				break;
			case IPL_DEPTH_1U:
				for (int j = colBeg; j <= colEnd; j++)
				/* Use C's printf() for convenience :p */
					printf("%1d ", (int)cvGetReal2D(A, i, j));
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

/* Timer function */
void runningTime(clock_t tic, clock_t toc) {
	printf( "Running Time: %lf seconds.\n",
		   (double)(toc - tic) / (double)CLOCKS_PER_SEC );
}

void avgRunningTime(clock_t tic, clock_t toc, int N) {
	printf( "Average Running Time: %lf seconds.\n",
		   (double)(toc - tic) / (double)CLOCKS_PER_SEC / (double)N );
}

/* Mean and variance normalization given the "sum", the sum of squares "sqSum", and # of pixels N */
/* Both sum and sqSum are double values, not IplImages */
/* "normImg" is both the source and destination */
void meanVarNorm(IplImage *normImg, double sum, double sqSum, int N) {
	double mean, stdev;
	mean = sum / (double)N;
	stdev = sqrt((sqSum - N * mean * mean) / N);
	//cout << "mean = " << mean << ", stdev = " << stdev << ".\n";

	/* normalized */
	cvSubS(normImg, cvScalar(mean), normImg);
	/* EPSILON: Avoid stdev = 0 */
	cvScale(normImg, normImg, 1 / (stdev + EPSILON));
}

/* Upright rectangular sum given two corner points (x1,y1), (x2,y2) */
/* For the sake of performance, don't check for validity, and don't use cvGetReal2D().
 * In addition, ii is in IPL_DEPTH_64F.
 * Make sure that (x1,y1) lies to the upper-left of (x2,y2) */
float recSumRight(IplImage *ii, int x1, int y1, int x2, int y2) {
	double ret =
		*(double *) (ii->imageData + y2 * ii->widthStep + x2 * sizeof(double)) +
		*(double *) (ii->imageData + (y1 - 1) * ii->widthStep + (x1 - 1) * sizeof(double)) -
		*(double *) (ii->imageData + (y1 - 1) * ii->widthStep + x2 * sizeof(double)) -
		*(double *) (ii->imageData + y2 * ii->widthStep + (x1 - 1) * sizeof(double));
	assert(!isnan(ret));
	return (float)ret;
}

/* Tilted rectangular sum given (x, y, w, h) */
/* For the sake of performance, don't check for validity, and don't use cvGetReal2D().
 * In addition, ii is in IPL_DEPTH_64F.
 * Make sure the parameters agree with those in the paper:
 * Lienhart & Maydt, "An extended set of Haar-like features for rapid object detection", [2002] */
float recSumTilt(IplImage *ii, int x, int y, int w, int h) {
	return
		*(double *) (ii->imageData + (y + w) * ii->widthStep + (x + w) * sizeof(double)) +
		*(double *) (ii->imageData + (y + h) * ii->widthStep + (x - h) * sizeof(double)) -
		*(double *) (ii->imageData + y * ii->widthStep + x * sizeof(double)) -
		*(double *) (ii->imageData + (y + w + h) * ii->widthStep + (x + w - h) * sizeof(double));
}

/* Check for NaN values of a matrix: returns true if it contains NaN values */
bool checkNaN(CvMat *mat, const char *name) {
	int rows = mat->rows;
	int cols = mat->cols;
	bool NaN = false;
	
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (isnan(cvGetReal2D(mat, i, j))) {
				NaN = true;
				cout << name << "[" << i << "][" << j << "]" << endl;
			}
		}
	}
	return NaN;
}
