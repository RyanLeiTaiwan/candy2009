/** File: train.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2010/01/12
 ** Description: The car-training program based on the paper:
    Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
    Important techniques / concepts:
      1. Real AdaBoost -> For now, use basic binary Adaboost
      2. Cascaded classifiers -> For now, it is without meta-stages (no confidence values)
      3. Rectangle / Edge Orientation Histogram / Edge-Density features
      4. Integral image technique in computation
 **/

/* Usage: car_train [POS_DIR] [NEG_DIR] [OUTPUT] */
#define META 0  // NOT YET supporting meta stages

#include "extract.h"
#include "adaboost.h"

int main(int argc, char *argv[]) {
	
	DIR *dir;
	/* There are some problems with <fstream> and Xcode 3.2 ... Orz 
	 * So I'll just settle with FILE * and fprintf().
	 */
	ofstream fout;  
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	char POS_PATH_BASE[ MAX_PATH_LENGTH ];
	char NEG_PATH_BASE[ MAX_PATH_LENGTH ];
	
	if (argc != 4) {
		error("Usage: car_train [POS_DIR] [NEG_DIR] [OUTPUT].");
	}
	
	if (!(dir = opendir(argv[ 1 ]))) {
		error("car_train: [POS_DIR] does not exist.");
	}
	/* Set POS_PATH_BASE to [POS_DIR] and append '/' or '\' */
	strcpy(POS_PATH_BASE, argv[ 1 ]);	
	sprintf(POS_PATH_BASE, "%s%c", POS_PATH_BASE, slash);
	closedir(dir);
	
	if (!(dir = opendir(argv[ 2 ]))) {
		error("car_train: [NEG_DIR] does not exist.");
	}
	/* Set NEG_PATH_BASE to [NEG_DIR] and append '/' or '\' */
	strcpy(NEG_PATH_BASE, argv[ 2 ]);	
	sprintf(NEG_PATH_BASE, "%s%c", NEG_PATH_BASE, slash);	
	closedir(dir);
	
	fout.open(argv[ 3 ]);
	if (!(fout)) {
		error("car_train: [OUTPUT] does not exist.");
	}
	
	/** Command is correct **/
	cout << "This program provides cascaded-AdaBoost training.\n" <<
		"Please make sure that [POS_DIR] and [NEG_DIR] contain only training image data,\n" <<
		"  whose size is " << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << ".\n" <<
		"Press [Enter] to continue, or ctrl+C/D/Z to exit ...";
	getchar();
	
	CvMat *POS, *NEG;
	int N1, N2; /* number of positive / negative images */
	int blockCount = 0; /* number of blocks in an image */
	
	/* [1] Feature extraction */
	/* First, check for validity of extraction parameters */
	assert(!(360 % (BIN_NUM * 2))); // (360 / BIN_NUM / 2) should be an integer */
	assert(360 / BIN_NUM == BIN_SIZE);
	assert(BIN_SIZE >> 1 == HALF_BIN_SIZE);
	
	cout << "\nStart of feature extraction ...\n";
	extractAll(POS_PATH_BASE, POS, N1, blockCount);
	cout << "Extraction of POS data completed.\n";
	extractAll(NEG_PATH_BASE, NEG, N2, blockCount);
	cout << "Extraction of POS data completed.\n";
	assert(blockCount > 0);
	cout << "# of blocks per image: " << blockCount << ".\n";
	getchar();

	
	/* [2] Cascaded-AdaBoost training */ 
	cout << "Start of cascaded-AdaBoost training ...\n";
	srand(time(NULL));
	float F_current = 1.0;  // current overall false positive rate
	int i = 1;  // AdaBoost stage counter
	int k = 0;  // # of AdaStrong trained so far
	int rejectCount = 0;  // # of negative images rejected so far

	/* allocate an array of AdaBoost strong classifiers to keep track of */
	AdaStrong *H = new AdaStrong[ ni + 1 ];
	/* Allocate rejection table */
	/** Note: All the xxxTable[]'s are of boolean flags **/
	bool *rejectTable = new bool[ N2 ];
	
	/* Learn A[1,j] stage as an exception */
	for (int j = 1; j <= ni + 1; j++, k++) {
		cout << "\nLearning stage A[" << i << "," << j << "]...\n";
		learnA(N1, N2, blockCount, rejectCount, rejectTable, POS, NEG, H, F_current, fout);
	}
	
#if META
	/* Learn M[1] stage */
	cout << "\nLearning stage M[1]...\n";
	learnM();
	k++;
#endif
	getchar();
	i++;
	
	/* Learn other A[i,j] stages */
	while ( F_current > F_target ) {
		
		for (int j = 1; j <= ni; j++, k++ ) {
			cout << "\nLearning stage A[" << i << "," << j << "]...\n";
			learnA(N1, N2, blockCount, rejectCount, rejectTable, POS, NEG, H, F_current, fout);
		}
#if META
		cout << "\nLearning stage M[" << i << "]...\n";
		learnM();
		k++;
#endif
		getchar();
		i++;
		
	}	
	
	/* Don't forget to close the file stream */
	fout.close();
	return 0;
}
