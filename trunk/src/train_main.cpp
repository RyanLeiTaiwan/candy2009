/** File: train_main.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2010/04/04
 ** Description: The main function of the car-training program based on the paper:
      Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
    Important techniques / concepts:
      1. Real AdaBoost -> For now, use basic binary Adaboost
      2. Cascaded classifiers -> For now, it is without meta-stages (no confidence values)
      3. Rectangle / Edge Orientation Histogram / Edge-Density features
      4. Integral image technique in computation
 **/

/* Usage: train [POS_DIR] [NEG_DIR] [OUTPUT_FILE] */
#define META 0  // NOT YET supporting meta stages

#include "extract.h"
#include "adaboost.h"

int main(int argc, char *argv[]) {

	DIR *dir;
	ofstream fout;
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	char POS_PATH_BASE[MAX_PATH_LENGTH];
	char NEG_PATH_BASE[MAX_PATH_LENGTH];
	clock_t tic, toc;

	if (argc != 4) {
		error("Usage: train [POS_DIR] [NEG_DIR] [OUTPUT_FILE].");
	}

	if (!(dir = opendir(argv[1]))) {
		error("train: [POS_DIR] open failed.");
	}
	/* Set POS_PATH_BASE to [POS_DIR] and append '/' or '\' */
	strcpy(POS_PATH_BASE, argv[1]);
	sprintf(POS_PATH_BASE, "%s%c", POS_PATH_BASE, slash);
	closedir(dir);

	if (!(dir = opendir(argv[2]))) {
		error("train: [NEG_DIR] open failed.");
	}
	/* Set NEG_PATH_BASE to [NEG_DIR] and append '/' or '\' */
	strcpy(NEG_PATH_BASE, argv[2]);
	sprintf(NEG_PATH_BASE, "%s%c", NEG_PATH_BASE, slash);
	closedir(dir);

	fout.open(argv[3]);
	if (!(fout)) {
		error("train: [OUTPUT_FILE] open failed.");
	}

	/** Command is correct **/
	echoOS();
	cout << "This program provides cascaded-AdaBoost training.\n" <<
		"Please make sure that [POS_DIR] and [NEG_DIR] contain only training image data,\n" <<
		"  whose size is " << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << ".\n" <<
		"Press [Enter] to continue, or ctrl+C/D/Z to exit ...";
	getchar();

	tic = clock();
	/* Use only one large matrix for storing all POS / NEG feature data */
	CvMat *POS, *NEG;
	int N1, N2; /* number of positive / negative images */
	int blockCount = 0; /* number of blocks in an image */

	/* [1] Feature extraction */
	/* First, check for validity of extraction parameters */
	assert(!(360 % (BIN_NUM * 2))); // (360 / BIN_NUM / 2) should be an integer */
	assert(360 / BIN_NUM == BIN_SIZE);
	assert(BIN_SIZE >> 1 == HALF_BIN_SIZE);

	cout << "\nStart of feature extraction ...\n";
	/* Should pass (CvMat *&) to really create the matrices */
	extractAll(POS_PATH_BASE, POS, N1, blockCount);
	cout << "Extraction of POS data completed.\n";

#if CHECKNaN
	cout << "Check POS matrix for NaN values:" << endl;
	if (checkNaN(POS, "POS")) {
	    error("POS matrix contains NaN values!");
    }
    else {
        cout << "OK!" << endl;
    }
#endif
	
	extractAll(NEG_PATH_BASE, NEG, N2, blockCount);
	cout << "Extraction of NEG data completed.\n";

#if CHECKNaN
	cout << "Check NEG matrix for NaN values:" << endl;
	if (checkNaN(NEG, "NEG")) {
	    error("NEG matrix contains NaN values!");
    }
    else {
        cout << "OK!" << endl;
    }
#endif
	assert(blockCount > 0);
	cout << endl << "# of blocks per image: " << blockCount << ".\n";
#if GETCHAR
	getchar();
#endif

	
	/* [2] Cascaded-AdaBoost training */
	cout << "Start of cascaded-AdaBoost training ...\n";
	srand(time(NULL));
	float F_current = 1.0;  // current overall false positive rate
	int i = 1;  // AdaBoost stage counter
	int rejectCount = 0;  // # of negative images rejected so far

	/* Allocate rejection table */
	/** Note: All the xxxTable[]'s are of boolean flags **/
	bool *rejectTable = new bool[ N2 ];
	memset(rejectTable, 0, N2);

	bool stop = false;  // Stopping flag
	vector<AdaStrong> H;  // A cascade of AdaBoost strong classifiers

	/*** The training algorithm ***/
	while (F_current > F_target && !stop) {
		/* upper bound for j */
		int jEnd = (i == 1) ? (ni + 1) : ni;

		for (int j = 1; j <= jEnd; j++) {
			/* Learn an A[i,j] stage */
			cout << "\nLearning stage A[" << i << "," << j << "]...\n";
			if ((stop = learnA(N1, N2, blockCount, rejectCount, rejectTable, POS, NEG, H, F_current))) {
				break;
			}
			cout << "Stage A[" << i << "," << j << "] completed.\n";
#if GETCHAR
			getchar();
#endif
		}
#if META
		if (stop) break;
		/* Learn a M[i] stage */
		cout << "\nLearning stage M[" << i << "]...\n";
		learnM();
		cout << "Stage M[" << i << "," << j << "] completed.\n";
  #if GETCHAR
		getchar();
  #endif
#endif
		i++;

	} // End of loop while (F_current > F_target && !stop)

	cout << "The entire training process completed.\nWriting model parameters to " << argv[3] << " ... ";
	/* Write model parameters to [OUTPUT_FILE]. See docs/train_format.txt for explanation. */
	writeModel(H, fout);
	cout << "done!\n";

	/* Show running time */
	toc = clock();
	runningTime(tic, toc);

	/* Don't forget to close the file stream */
	fout.close();
	return 0;
}
