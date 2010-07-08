/** File: util.h
 ** Author: Ryan Lei
 ** Creation: 2010/01/09
 ** Update: 2010/07/09
 ** Description: Header file for utility functions
 **/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <dirent.h>
#include <cstring>
#include <ctime>
#include <cmath>
#include <vector>
//#include <cxcore.h>
//#include <cv.h>
//#include <highgui.h>
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <opencv2/highgui/highgui_c.h>
#include "parameters.h"
using namespace std;

/* Print an error message in cerr */
void error(const char *msg);
/* Echo the operating system set by #define */
void echoOS();
/* (Not precisely) count # of image files in a directory */
int countImages(const char *PATH);
/* Print a 2D, single-channel matrix, with / without a specified range */
void printMat(CvMat *matrix, const char *name, int rowBeg, int rowEnd, int colBeg, int colEnd);
void printMat(CvMat *matrix, const char *name);
/* The overloaded version for IplImage: Print a single-channel image */
void printMat(IplImage *img, const char *name, int rowBeg, int rowEnd, int colBeg, int colEnd);
void printMat(IplImage *img, const char *name);
/* Timer function */
void runningTime(clock_t tic, clock_t toc);
void avgRunningTime(clock_t tic, clock_t toc, int N);
/* Mean and variance normalization given the "sum", the sum of squares "sqSum", and # of pixels N */
void meanVarNorm(IplImage *normImg, double sum, double sqSum, int N);
/* Upright rectangular sum given two corner points (x1,y1), (x2,y2) */
float recSumRight(IplImage *ii, int x1, int y1, int x2, int y2);
/* Tilted rectangular sum given (x, y, w, h) */
float recSumTilt(IplImage *ii, int x, int y, int w, int h);
/* Check for NaN values of a matrix: returns true if it contains NaN values */
bool checkNaN(CvMat *mat, const char *name);

