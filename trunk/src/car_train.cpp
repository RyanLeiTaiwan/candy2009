/** File: car_extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2010/01/09
 ** Description: The car-training program based on the paper:
    Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
    Important techniques / concepts:
      1. Real AdaBoost -> For now, use basic binary Adaboost
      2. Cascaded classifiers -> For now, it is without meta-stages (no confidence values)
      3. Rectangle / Edge Orientation Histogram / Edge-Density features
      4. Integral image technique in computation
 **/

/* Usage: car_train [POS_DIR] [NEG_DIR] [OUTPUT] */
#include <iostream>
#include <cstring>
#include <cassert>
#include <dirent.h>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
#include "car_extract.h"
using namespace std;

int main(int argc, char *argv[]) {
	DIR *dir;
	FILE *fout;
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	char POS_PATH_BASE[ MAX_PATH_LENGTH ];
	char NEG_PATH_BASE[ MAX_PATH_LENGTH ];
	
	if (argc != 4) {
		cerr << "Usage: car_train [POS_DIR] [NEG_DIR] [OUTPUT]\n";
		exit(EXIT_FAILURE);
	}
	
	if (!(dir = opendir(argv[ 1 ]))) {
		cerr << "car_train: [POS_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	/* Set POS_PATH_BASE to [POS_DIR] and append '/' or '\' */
	strcpy(POS_PATH_BASE, argv[ 1 ]);	
	sprintf(POS_PATH_BASE, "%s%c", POS_PATH_BASE, slash);
	closedir(dir);
	
	if (!(dir = opendir(argv[ 2 ]))) {
		cerr << "car_train: [NEG_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	/* Set NEG_PATH_BASE to [NEG_DIR] and append '/' or '\' */
	strcpy(NEG_PATH_BASE, argv[ 2 ]);	
	sprintf(NEG_PATH_BASE, "%s%c", NEG_PATH_BASE, slash);	
	closedir(dir);
	
	if (!(fout = fopen(argv[ 3 ], "w"))) {
		cerr << "car_train: [OUTPUT] does not exist.\n";
		exit(EXIT_FAILURE);
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
	
	/* [0] Feature extraction */
	extractAll(POS_PATH_BASE, &POS, &N1, &blockCount);
	cout << "Extraction of POS data completed.\n";
	extractAll(NEG_PATH_BASE, &NEG, &N2, &blockCount);
	cout << "Extraction of POS data completed.\n";
	assert(blockCount > 0);
	cout << "# of blocks per image: " << blockCount << ".\n";
	getchar();
	
	
	return 0;
}
