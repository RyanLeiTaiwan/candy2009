/** File: detect.h
 ** Author: Ryan Lei
 ** Creation: 2010/01/26
 ** Update: XXXX/XX/XX
 ** Description: Header file for the car-detection program.
 **/
#include "adaboost.h"
#include "extract.h"
#define TIMER_DETECT 1  // Show running time of classifying a single image

/* Print program usage: reused a lot */
void printUsage();

/* Read the model parameters to vector<AdaStrong> H */
void readModel(ifstream &fin, vector<AdaStrong> &H);
/* Classify a single image using cascaded AdaBoost */
float classifyCascade(IplImage *img, vector<AdaStrong> &H);
					
/* Online single image mode with directory [TEST_DIR] */
void detectSingleOnline(char *TEST_DIR, vector<AdaStrong> &H);
