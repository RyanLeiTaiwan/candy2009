/** File: adaboost.h
 ** Author: Ryan Lei
 ** Creation: 2010/01/11
 ** Update: 2010/01/26
 ** Description: Header file for AdaBoost learning.
 **/

#include "util.h"

/* An AdaBoost weak classifier */
class AdaWeak {
public:
	int Bid;
	int Fid;
	short parity; /* -1 or +1 */
	float decision; /* 1-D classification threshold */
	float weight; /* the weight of this weak classifier */
	void setValue(int Bid, int Fid, short parity, float decision, float weight);
};

/* An AdaBoost strong classifier */
class AdaStrong {
public:
	vector<AdaWeak> h;
	float threshold;
};

/* Learn AdaBoost stage A[i,j]: Returns true if running out of NEG images. */
bool learnA(const int N1, const int N2, const int blockCount, int &rejectCount, bool rejectTable[], 
			CvMat *POS, CvMat *NEG, vector<AdaStrong> &H, float &F_current);
/* Learn Meta stage M[i] */
void learnM();
/* Randomly select negative examples from the bootstrap */
void selectNeg(const int N1, const int N2, bool rejected[], int selection[]);
/* Add a weak learner h to the strong classifier A[i,j] */
void addWeak(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
			 CvMat *posWeight, CvMat *negWeight, vector<AdaStrong> &H);
/* Classify all data using the strong classifier. Write the results in "results".
 * Returns the minumum posAnswer.
 */
float classifyStrongAll(const int N1, const int blockCount, int selection[], CvMat *POS, CvMat *NEG,
					   vector<AdaStrong> &H, CvMat *posResult, CvMat *negResult);
/* Write model parameters to [OUTPUT]. See docs/train_format.txt for output format. */
void writeModel(vector<AdaStrong> &H, ofstream &fout);

