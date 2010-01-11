/** File: adaboost.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2009/01/11
 ** Description: The implementations of cascaded AdaBoost.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
#include "adaboost.h"
using namespace std;

/* Learn AdaBoost stage A[i,j] */
void learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejected[],
			CvMat POS[], CvMat NEG[], AdaStrong H[], float &F_current, FILE *&fout) {
	
	
	
	
	
	
}

/* Randomly select negative examples from the bootstrap */
void selectNeg(int N1, int N2, bool rejected[], int selectTable[]) {
	;
	
}

/* Learn Meta stage M[i] */
void learnM() {
	;
}
