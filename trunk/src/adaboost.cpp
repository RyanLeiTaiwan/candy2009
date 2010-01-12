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
			CvMat *POS, CvMat *NEG, AdaStrong H[], float &F_current, ofstream &fout) {
	
	/* [0] Initialization */
	int selection[ N1 ];  // image selection result
	float initialW = 1.f / (float) (N1 << 1) ;  // Initial data weight = 1 / 2N1 (# of pos == # of neg)
	float f_local = 1.f;
	float threshold = 0.f; /* threshold of the strong classifier A[i,j], should be recorded */
	CvMat *posWeight, *negWeight;
	posWeight = cvCreateMat(N1, 1, CV_32FC1);
	cvSet(posWeight, cvScalar(initialW));
	negWeight = cvCloneMat(posWeight);
	
	/* [1] Randomly select N1 negative examples from the bootstrap set.
	 * Report an error if NEG images are not enough
	 */	
	if (N2 - rejectCount < N1) {
		fout << "Warning: Not enough NEG images.\n";
		error( "learnA(): Not enough NEG images." );
	}
    selectNeg(N1, N2, rejectTable, selection);
	
	while (f_local > f_maxA) {
		/* [2] Add a weak learner h to the strong classifier A[i,j] */
		addWeak(N1, N2, blockCount, selection, POS, NEG, posWeight, negWeight, H);
		
		
	}
	
	
	
	
}

/* Randomly select negative examples from the bootstrap */
void selectNeg(const int N1, const int N2, bool rejectTable[], int selection[]) {
	/* Selection table: indicate whether this image has been selected */
	bool *selectTable = new bool[ N2 ];
	memset(selectTable, 0, N2);
	
	for ( int i = 0; i < N1; i++ ) {
		int Iid;
		/* Randomly select an image that is not yet REJECTED or SELECTED 
		 * by the AdaBoost learning */
		do {
			Iid = rand() % N2;
			//cout << "Iid = " << Iid << endl;
		}
		while (rejectTable[ Iid ] || selectTable[ Iid ]);
		//cout << "selection = " << Iid << endl;
		selectTable[ Iid ] = true;  // Mark as selected
		selection[ i ] = Iid;  // Record the result
	}	
	delete [] selectTable;
}

void addWeak(const int N1, const int N2, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
			 CvMat *posWeight, CvMat *negWeight, AdaStrong H[]) {
	
	/* For all possible blocks and features (Bid, Fid):
	 * Select a weak classifier that minimizes the error rate */
	for (int Bid = 0; Bid < blockCount; Bid++ ) {
		for (int Fid = 0; Fid < FEATURE_COUNT; Fid++ ) {
			float posMean = 0.f, negMean = 0.f;  // POS / NEG Mean values for this feature
			
			/* [1] Compute the POS & NEG mean feature values */
			for (int Iid = 0; Iid < N1; Iid++) {
				posMean += cvGetReal2D(POS, Iid * blockCount + Bid, Fid);
				//negMean += NEG[ selectTable[ Iid ] ][ Bid ][ Fid ];
			}
			posMean /= N1;
			negMean /= N2;			

		}
	}
}


/* Learn Meta stage M[i] */
void learnM() {
	;
}
