/** File: adaboost.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2009/01/26
 ** Description: The implementations of cascaded AdaBoost.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include "adaboost.h"

/* Set values of an AdaWeak */
void AdaWeak::setValue(int Bid, int Fid, short parity, float decision, float weight) {
	this->Bid = Bid;
	this->Fid = Fid;
	this->parity = parity;
	this->decision = decision;
	this->weight = weight;
}

/* Learn AdaBoost stage A[i,j]: Returns true if running out of NEG images. */
bool learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejectTable[],
			CvMat *POS, CvMat *NEG, vector<AdaStrong> &H, float &F_current) {
	
	/* [0] Initialization */
	int selection[ N1 ];  // image selection result
	static float initialW = 1.f / (float) (N1 << 1) ;  // Initial data weight = 1 / 2N1 (# of pos == # of neg)
	float d_local = 0.f, f_local = 1.f;
	float threshold = 0.f; /* threshold of the strong classifier A[i,j], should be recorded */
	CvMat *posWeight, *negWeight;
	posWeight = cvCreateMat(1, N1, CV_32FC1);  // Row vector, which is easy to print
	cvSet(posWeight, cvScalar(initialW));
	negWeight = cvCloneMat(posWeight);
	
	/* [1] Randomly select N1 negative examples from the bootstrap set.
	 * Stop the training if negative images are not enough.
	 */	
	if (N2 - rejectCount < N1) {
		cout << "learnA(): Run out of negative images.\n";
		return true;
	}
    selectNeg(N1, N2, rejectTable, selection);

	/* Results of H(x) classification */
	CvMat *posResult = cvCreateMat(1, N1, CV_32FC1);
	CvMat *negResult = cvCreateMat(1, N1, CV_32FC1);
	
	/* Create an AdaStrong */
	H.push_back(AdaStrong());
	int weakUsed = 0;
	while (f_local > f_maxA) {
		/* [2] Add a weak learner h to the strong classifier A[i,j] */
		weakUsed++;
#if GETCHAR
		cout << "* Weak learner " << weakUsed << ":\n";
#endif		
		addWeak(N1, blockCount, selection, POS, NEG, posWeight, negWeight, H);
		
		/* [3.1] Use the H(x) so far to classify:
		 * H(x) = sign( sum[alpha_t * h_t(x)] )
		 * Process the POS and NEG together in one loop
		 */
		float minPosResult = classifyStrongAll(N1, blockCount, selection, POS, NEG, H, posResult, negResult);
#if 0
		printMat(posResult, "posResult");
		printMat(negResult, "negResult");
		getchar();
#endif

		int detect = 0, fp = 0;  // Counters for detections and false-positives
#if GETCHAR
		/* May be skipped: Calculate d_local and f_local of H(x) */
		for (int Iid = 0; Iid < N1; Iid++) {
			if (cvGetReal2D(posResult, 0, Iid) >= 0.f) {
				detect++;
			}
			if (cvGetReal2D(negResult, 0, Iid) >= 0.f) {
				fp++;
			}
		}
		d_local = (float)detect / (float)N1;
		f_local = (float)fp / (float)N1;

		cout << "  Before modifying the threshold:\n    Detection rate = " << d_local << " (" << detect << "/" << N1 << ")\n";
		cout << "    False positive rate = " << f_local << " (" << fp << "/" << N1 << ")\n";
#endif
		
        /* [3.2] If necessary, modify the threshold of H(x) to fulfill d_minA 
		 * If minPosResult < 0, then let
		 * result = result - minPosResult so that all POS data are
		 * determined as positive, i.e., detection rate = 100%.
		 */		
		if (minPosResult < 0.f) {
			threshold = minPosResult;
			cvSubS(posResult, cvScalar(threshold), posResult);
			cvSubS(negResult, cvScalar(threshold), negResult);
		}
#if 0
		printMat(posResult, "posResult");
		printMat(negResult, "negResult");
		getchar();
#endif
		
        /* [4] Calculate d_local and f_local of H(x) */
		detect = 0, fp = 0;  // Counters for detections and false-positives
		for (int Iid = 0; Iid < N1; Iid++) {
			if (cvGetReal2D(posResult, 0, Iid) >= 0.f) {
				detect++;
			}
			if (cvGetReal2D(negResult, 0, Iid) >= 0.f) {
				fp++;
			}
		}
		d_local = (float)detect / (float)N1;
		f_local = (float)fp / (float)N1;
#if GETCHAR
		cout << "  After modifying the threshold:\n    Detection rate = " << d_local << " (" << detect << "/" << N1 << ")\n";
		cout << "    False positive rate = " << f_local << " (" << fp << "/" << N1 << ")\n";
		getchar();
#endif		
		assert(d_local >= d_minA); 
		
		
	} // End of loop "while (f_local > f_maxA). Now the learning of strong classifier H is complete."
	
	/* [5] Write the threshold to the AdaStrong */
	H.back().threshold = threshold;
	
	/* [6] Update the overall false positive rate */
	F_current *= f_local;
	cout << "Weak learners used: " << weakUsed << endl;
	cout << "Overall false positive rate: " << F_current << endl;
	
	/* [7] Reject the true negative images */
#if GETCHAR
	int rejectLocal = 0;
#endif
	for (int Iid = 0; Iid < N1; Iid++) {
		if (cvGetReal2D(negResult, 0, Iid) < 0.f) {
			rejectTable[selection[Iid]] = true;
			rejectCount++;
#if GETCHAR
			rejectLocal++;
#endif
		}
	}
#if GETCHAR
	cout << "Rejected " << rejectLocal << " images for this AdaBoost stage.\n";
#endif
	cout << "Rejected " << rejectCount << " images in total.\n";
	
	return false;
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
#if 0
		cout << "selection = " << Iid << endl;
#endif
		selectTable[ Iid ] = true;  // Mark as selected
		selection[ i ] = Iid;  // Record the result
	}	
	delete [] selectTable;
}


void addWeak(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
			 CvMat *posWeight, CvMat *negWeight, vector<AdaStrong> &H) {
	
	/* The best weak classifier h */
	AdaWeak h;
	float bestError = 1.f;
	int bestBid, bestFid;
	short bestParity;
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
			short parity = (posMean >= negMean) ? +1 : -1;
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
				if (parity * cvGetReal2D(NEG, Iid * blockCount + Bid, Fid) >= parity * decision) {
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
	H.back().h.push_back(h);
	
#if GETCHAR
	avgError /= (blockCount * FEATURE_COUNT);
	cout << "Average error rate: " << avgError << endl;
	cout << "Best error rate: " << bestError << ", Bid: " << bestBid << ", Fid: " << bestFid << endl;
	cout << "Best weak learner weight: " << weight << endl;
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
	cvMul(posWeight, posMul, posWeight);
	cvMul(negWeight, negMul, negWeight);
	float normalize = cvSum(posWeight).val[0] + cvSum(negWeight).val[0];
	cvScale(posWeight, posWeight, 1.f / normalize);
	cvScale(negWeight, negWeight, 1.f / normalize);

#if 0
	printMat(posWeight, "posWeight");
	printMat(negWeight, "negWeight");
	getchar();
#endif
	
}

/* Classify all data using the strong classifier. Write the results in "results".
 * Returns the minumum posAnswer. */
float classifyStrongAll(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
					    vector<AdaStrong> &H, CvMat *posResult, CvMat *negResult) {
	
	float minPosAnswer = 0.f; // Record posAnswer less than zero
	/* H(x[i]) = sign( sum[ weight_t * h_t(x[i]) ] )
	 * Process the POS and NEG together in one loop.
	 */
	for (int Iid = 0; Iid < N1; Iid++) {
		float posAnswer = 0.f;
		float negAnswer = 0.f;
		for (vector<AdaWeak>::iterator it = H.back().h.begin(); it != H.back().h.end(); it++) {
			int Bid = it->Bid;
			int Fid = it->Fid;
			short parity = it->parity;
			float decision = it->decision;
			float weight = it->weight;
			
			/* Determine that positive is positive */
			if (parity * cvGetReal2D(POS, Iid * blockCount + Bid, Fid) >= parity * decision) {
				posAnswer += weight;
			}
			/* Determine that positive is negative */
			else {
				posAnswer -= weight;
			}
			/* Determine that negative is positive */
			if (parity * cvGetReal2D(NEG, Iid * blockCount + Bid, Fid) >= parity * decision) {
				negAnswer += weight;
			}
			/* Determine that negative is negative */
			else {
				negAnswer -= weight;
			}	
		}
		
		/* After classification of one image: Record the result */
		cvSetReal2D(posResult, 0, Iid, posAnswer);
		cvSetReal2D(negResult, 0, Iid, negAnswer);
		
		/* Remember the minimum posAnswer:
		 * Since d_minA is implemented as 100%, we must shift the threshold to the minimum posAnswer if it's below zero.
		 */
		if (posAnswer < minPosAnswer) {
			minPosAnswer = posAnswer;
		}
	} // End of loop "Iid"
	
	return minPosAnswer;
}

/* Write model parameters to [OUTPUT]. See docs/train_format.txt for output format. */
void writeModel(vector<AdaStrong> &H, ofstream &fout) {
	/* Model identifier */
	fout << "[Candy2009]" << endl;
	/* # of Strong classifiers */
	fout << H.size() << endl;
	
	// AdaStrong iterator: itStrong. AdaWeak iterator: itWeak
	for (vector<AdaStrong>::iterator itStrong = H.begin(); itStrong != H.end(); itStrong++) {
		/* # of H_k' weak classifiers, threshold of H_k */
		fout << itStrong->h.size() << " " << itStrong->threshold << endl;
		for (vector<AdaWeak>::iterator itWeak = itStrong->h.begin(); itWeak != itStrong->h.end(); itWeak++) {
			/* hk1's Bid, Fid, parity, decision, weight */
			fout << itWeak->Bid << " " << itWeak->Fid << " " << itWeak->parity << " " <<
				itWeak->decision << " " << itWeak->weight << endl;
		}
	}
	/* Model identifier */
	fout << "[Candy2009]" << endl;
}

/* Learn Meta stage M[i] */
void learnM() {
	;
}
