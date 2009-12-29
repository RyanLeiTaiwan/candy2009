/** File: car_extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: XXXX/XX/XX
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
#include <dirent.h>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
using namespace std;

int main(int argc, char *argv[]) {
	DIR *dir;
	FILE *fout;
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	char POS_PATH_BASE[ MAX_PATH_LENGTH ];
	char NEG_PATH_BASE[ MAX_PATH_LENGTH ];
	
	if (argc != 4) {
		cout << "Usage: car_train [POS_DIR] [NEG_DIR] [OUTPUT]\n";
		exit(EXIT_FAILURE);
	}
	
	if (!(dir = opendir(argv[ 1 ]))) {
		cout << "car_train: [POS_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	/* Set POS_PATH_BASE to [POS_DIR] and append '/' or '\' */
	strcpy(POS_PATH_BASE, argv[ 1 ]);	
	sprintf(POS_PATH_BASE, "%s%c", POS_PATH_BASE, slash);
	closedir(dir);
	
	if (!(dir = opendir(argv[ 2 ]))) {
		cout << "car_train: [NEG_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	strcpy(NEG_PATH_BASE, argv[ 2 ]);	
	sprintf(NEG_PATH_BASE, "%s%c", NEG_PATH_BASE, slash);	
	closedir(dir);
	
	if (!(fout = fopen(argv[ 3 ], "w"))) {
		cout << "car_train: [OUTPUT] does not exist.\n";
		exit(EXIT_FAILURE);
	}

	
	/** Command is correct **/
	cout << "Command is correct.\n";
	
	return 0;
}
