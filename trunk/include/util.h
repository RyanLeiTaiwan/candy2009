/** File: util.h
 ** Author: Ryan Lei
 ** Creation: 2010/01/09
 ** Update: 2010/01/10
 ** Description: Header file for utility functions
 **/

/* Print an error message in cerr */
void error(const char *msg);
/* (Not precisely) count # of image files in a directory */
int countImages(const char *PATH);
/* Print a 2D, single-channel matrix, with default range being the whole matrix */
void printMat(CvMat *matrix, const char *name, int rowBeg = -1, int rowEnd = -1, int colBeg = -1, int colEnd = -1);
/* The overloaded version for IplImage: Print a single-channel image */
void printMat(IplImage *img, const char *name, int rowBeg = -1, int rowEnd = -1, int colBeg = -1, int colEnd = -1);
/* Timer function */
void runningTime( clock_t tic, clock_t toc );