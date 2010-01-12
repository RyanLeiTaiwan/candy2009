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

/* Learn AdaBoost stage A[i,j] */
void learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejectTable[],
			CvMat POS[], CvMat NEG[], AdaStrong H[], float &F_current, ofstream &fout) {
	
	/* [0] Initialization */
	int selection[ N1 ];  // image selection result
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
    selectNeg(N1, N2, rejectTable, selection);
	
	
	
	
	
	
}

/* Randomly select negative examples from the bootstrap */
void selectNeg(int N1, int N2, bool rejectTable[], int selection[]) {
	/* Selection table: indicate whether this image has been selected */
	bool *selectTable = new bool[ N2 ];	
	for ( int i = 0; i < N1; i++ ) {
		int Iid;
		/* Randomly select an image that is not yet REJECTED or SELECTED 
		 * by the AdaBoost learning */
		do {
			Iid = rand() % N2;
			cout << "Iid = " << Iid << endl;
		}
		while (rejectTable[ Iid ] || selectTable[ Iid ]);
		cout << "selection = " << Iid << endl;
		getchar();
		selectTable[ Iid ] = true;  // Mark as selected
		selection[ i ] = Iid;  // Record the result
	}	
	delete [] selectTable;
}

/* Learn Meta stage M[i] */
void learnM() {
	;
}
