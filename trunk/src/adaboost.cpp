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

/* Set values of an AdaWeak */
void AdaWeak::setValue(int Bid, int Fid, char parity, float decision, float weight) {
	this->Bid = Bid;
	this->Fid = Fid;
	this->parity = parity;
	this->decision = decision;
	this->weight = weight;
}

/* Learn AdaBoost stage A[i,j] */
void learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejectTable[],
			CvMat *POS, CvMat *NEG, AdaStrong &H, float &F_current, ofstream &fout) {
	
	/* [0] Initialization */
	int selection[ N1 ];  // image selection result
	static float initialW = 1.f / (float) (N1 << 1) ;  // Initial data weight = 1 / 2N1 (# of pos == # of neg)
	float f_local = 1.f;
	float threshold = 0.f; /* threshold of the strong classifier A[i,j], should be recorded */
	CvMat *posWeight, *negWeight;
	posWeight = cvCreateMat(1, N1, CV_32FC1);  // Row vector, which is easy to print
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

	
	/* Result of H(x) classification */
	CvMat *posResult = cvCreateMat(1, N1, CV_32FC1);
	CvMat *negResult = cvCreateMat(1, N1, CV_32FC1);
	while (f_local > f_maxA) {
		/* [2] Add a weak learner h to the strong classifier A[i,j] */
		addWeak(N1, blockCount, selection, POS, NEG, posWeight, negWeight, H);
		
		/* [3.1] Use the H(x) so far to trial-classify 
		 * H(x) = sign( sum[alpha_t * h_t(x)] )
		 * Process the POS and NEG together in one loop
		 */
		classifyStrongAll(N1, blockCount, selection, POS, NEG, H, posResult, negResult);
		
		/* [4] Calculate f_local of H(x) */
		
        /* [2.2] If necessary, modify the threshold of H(x) to fulfill d_minA 
		 * If min( posResult ) < 0, then let
		 * result = result - min( posResult ) so that all POS data are
		 * determined as positive, i.e., detection rate = 100%.
		 */		
		
        /* [4] Calculate f_local of H(x) */
		
	}
	
	
	
	
}

/* Randomly select negative examples from the bootstrap */
void selectNeg(const int N1, const int N2, bool rejectTable[], int selection[]) {
	/* Selection table: indicate whether this image has been selected */
	bool *selectTable = new bool[ N2 ];
	memset(selectTable, 0, N2);
	
	for ( int i = 0; i < N1; i++ ) {
		int Iid;
		/* Randomly select an image that is neither REJECTED nor SELECTED 
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


void addWeak(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
			 CvMat *posWeight, CvMat *negWeight, AdaStrong &H) {
	
	/* The best weak classifier h */
	AdaWeak h;
	float bestError = 1.f;
	int bestBid, bestFid;
	char bestParity;
	float bestDecision;
#if GETCHAR
	float avgError = 0.f;
#endif
	/* Vector of 1's */
	CvMat *ones = cvCreateMat(1, N1, CV_32FC1);
	cvSet(ones, cvScalar(1));
	/* Temporary classification result: +1 for correct, -1 for wrong */
	CvMat *posResult, *negResult;
	posResult = cvCreateMat(1, N1, CV_32FC1);
	negResult = cvCreateMat(1, N1, CV_32FC1);
	/* The weak leaner weight multiplier vectors */
	CvMat *posMul, *negMul;
	posMul = cvCreateMat(1, N1, CV_32FC1);
	negMul = cvCreateMat(1, N1, CV_32FC1);

	
	/* For all possible blocks and features (Bid, Fid):
	 * Select a weak classifier that minimizes the error rate */
	for (int Bid = 0; Bid < blockCount; Bid++ ) {
		for (int Fid = 0; Fid < FEATURE_COUNT; Fid++ ) {
			float posMean = 0.f, negMean = 0.f;  // POS & NEG Mean values for this feature
			/* [0] Reset posResult, negResult to ones */
			cvReleaseMat(&posResult);
			posResult = cvCloneMat(ones);
			cvReleaseMat(&negResult);
			negResult = cvCloneMat(ones);
			
			/* [1] Compute the POS & NEG mean feature values */
			for (int Iid = 0; Iid < N1; Iid++) {
				posMean += cvGetReal2D(POS, Iid * blockCount + Bid, Fid);
				negMean += cvGetReal2D(NEG, selection[ Iid ] * blockCount + Bid, Fid);
			}
			posMean /= N1;
			negMean /= N1;
			
			/* Initial decision threshold: Average of POS_mean and NEG_mean */
			float decision = (posMean + negMean) / 2.f;
			/* 
			 * parity = +1: ... negMean ... | ... posMean ...
			 * parity = -1: ... posMean ... | ... negMean ...
			 */
			float parity = (posMean >= negMean) ? +1 : -1;
#if 0
			cout << "Bid: " << Bid << ", Fid: " << Fid << ", posMean = " << posMean << ", negMean = " << negMean << endl;
#endif
			
			/* [2] Classify and compute the AdaBoost weighted error rate */
			float error = 0.f;
			for (int Iid = 0; Iid < N1; Iid++) {
				/* Positive and wrong => False negative */
				if (parity * cvGetReal2D(POS, Iid * blockCount + Bid, Fid) < parity * decision) {
					error += cvGetReal2D(posWeight, 0, Iid);
					cvSetReal2D(posResult, 0, Iid, -1.f);
				}
				
				/* Nagative and wrong => False positive */
				if (parity * cvGetReal2D(NEG, Iid * blockCount + Bid, Fid) > parity * decision) {
					error += cvGetReal2D(negWeight, 0, Iid);
					cvSetReal2D(negResult, 0, Iid, -1.f);
				}
			}
#if 0
			cout << "Error rate = " << error << endl;
			getchar();
#endif
#if GETCHAR
			avgError += error;
#endif
			
			/* [3] If error < bestError, keep the values in h */
			if (error < bestError) {
				bestError = error;
				bestBid = Bid;
				bestFid = Fid;
				bestParity = parity;
				bestDecision = decision;
				/* Copy posResult and negResult to posMul, negMul */
				cvReleaseMat(&posMul);
				posMul = cvCloneMat(posResult);
				cvReleaseMat(&negMul);
				negMul = cvCloneMat(negResult);
			}
		}
	} // End of for all (Bid, Fid). Now we have the best weak learner.

	
	/* Make sure bestError < 0.5 (random guessing) */
	if (bestError >= 0.5f) {
		error("addWeak(): Best error rate is above 0.5! Please train again.");
	}
	/* Weak leaner weight: http://en.wikipedia.org/wiki/Adaboost */
	float weight = logf((1.f - bestError) / bestError) / 2.f;
	/* [4] Add h to H */
	h.setValue(bestBid, bestFid, bestParity, bestDecision, weight);
	H.h.push_back(h);
	
#if GETCHAR
	avgError /= (blockCount * FEATURE_COUNT);
	cout << "Average error rate: " << avgError << endl;
	cout << "Best error rate: " << bestError << ", Bid: " << bestBid << ", Fid: " << bestFid << endl;
	cout << "Best weak learner weight: " << weight;
	getchar();
#endif

#if 0
	printMat(posMul, "posMul");
	printMat(negMul, "negMul");
	getchar();
#endif
	
	/* 
	/* [5] Update the weight:
	 *             D_t[i] * exp(-weight_t[i] * y_t[i] * h_t(x[i])
	 * D_t+1[i] = ------------------------------------------------
	 *                         normalization factor
	 *
	 * So far, posMul and negMul contain values of y_t(i) * h_t(x(i)): +1 or -1
	 */
	cvScale(posMul, posMul, -weight);
	cvScale(negMul, negMul, -weight);
	cvExp(posMul, posMul);
	cvExp(negMul, negMul);
	float normalize = cvSum(posMul).val[0] + cvSum(negMul).val[0];
	cvScale(posMul, posMul, 1.f / normalize);
	cvScale(negMul, negMul, 1.f / normalize);

#if 0
	printMat(posMul, "posMul");
	printMat(negMul, "negMul");
	getchar();
#endif	
	
}

/* Classify all data using the strong classifier. Write the results in "results". */
void classifyStrongAll(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
					   AdaStrong &H, CvMat *posResult, CvMat *negResult) {
	
	/* H(x[i] = sign( sum[ weight_t * h_t(x[i]) ] )
	 * Process the POS and NEG together in one loop.
	 */
	for (int Iid = 0; Iid < N1; Iid++) {		
		for (vector<AdaWeak>::iterator it = H.h.begin(); it != H.h.end(); it++) {
			int Bid = it->Bid;
			int Fid = it->Fid;
			short parity = it->parity;
			float decision = it->decision;
			float weight = it->weight;
			
			/* Determine that positive is positive */
			
			/* Determine that positive is negative */
			/* Determine that negative is positive */
			/* Determine that negative is negative */
		}
	}
}

/* Learn Meta stage M[i] */
void learnM() {
	;
}
