/** File: adaboost.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2009/01/11
 ** Description: The implementations of cascaded AdaBoost.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include "adaboost.h"
using namespace std;

/* Learn AdaBoost stage A[i,j] */
void learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejectTable[],
			CvMat POS[], CvMat NEG[], AdaStrong H[], float &F_current) {
	
	/* [0] Initialization */
	int selectTable[ N2 ];  // image selection table
	int N = N1 << 1;  // Total image count, with "# of pos = # of neg" in the process
	float initialW = 1.f / (float) N;  // Initial data weight
	
	
	/* [1] Randomly select N1 negative examples from the bootstrap set 
	 * Report an error if NEG images are not enough
	 */
	//fout << "Test in learnA\n";
	
	exit(EXIT_SUCCESS);
	if (N2 - rejectCount < N1) {
		//fout << "Warning: Not enough NEG images.\n";
		error( "learnA(): Not enough NEG images." );
	}
    selectNeg( N1, N2, rejectTable, selectTable );
	
	
	
	
	
	
}

/* Randomly select negative examples from the bootstrap */
void selectNeg(int N1, int N2, bool rejected[], int selectTable[]) {
	;
	
}

/* Learn Meta stage M[i] */
void learnM() {
	;
}
