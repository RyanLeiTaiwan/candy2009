/** File: detect_main.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/26
 ** Modification: 2010/03/27
 ** Description: The main function of the car-detection program based on the paper:
 **   Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
 **/

/* Usage:
 * 1. detect -c [MODEL_FILE] : Online detection using a camera.
 * 2. detect -v [VIDEO_FILE] [MODEL_FILE] : Online detection using a video file.
 * 3. detect -s [TEST_DIR] [MODEL_FILE] : Online detection using single images in one directory.
 * 4. detect -s [POS_DIR] [NEG_DIR] [MODEL_FILE] : Offline batch detection using POS & NEG directories.
 */
#include "detect.h"

int main(int argc, char *argv[]) {
	DIR *dir1, *dir2;
	ifstream fin;
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	char PATH1_BASE[MAX_PATH_LENGTH];
	char PATH2_BASE[MAX_PATH_LENGTH];
	vector<AdaStrong> H;
	
	/* Process user command */
	if (argc < 3 || argc > 5) {
		printUsage();
	}
	
	/* Option: -c */
	if (!strcmp(argv[1], "-c")) {
		if (argc == 3) {
			cout << "Camera mode. Under construction." << endl;
		}
		else {
			printUsage();
		}
	}
	/* Option: -v */
	else if (!strcmp(argv[1], "-v")) {
		if (argc == 4) {
			cout << "Video mode. Under construction." << endl;
		}
		else {
			printUsage();
		}		
	}
	/* Option: -s */
	else if (!strcmp(argv[1], "-s")) {
		if (argc == 4) {
			cout << "Online single image mode (TEST_DIR)." << endl;
			if (!(dir1 = opendir(argv[2]))) {
				error("detect: [TEST_DIR] open failed.");
			}
			fin.open(argv[3]);
			if (!fin) {
				error("detect: [MODEL_FILE] open failed.");
			}
			/*** Online single image mode with [TEST_DIR] ***/
			/* Set PATH1_BASE to [TEST_DIR] and append slash */
			strcpy(PATH1_BASE, argv[2]);	
			sprintf(PATH1_BASE, "%s%c", PATH1_BASE, slash);
			/* Read model parameters into AdaStrong vector */
			readModel(fin, H);
			detectSingleOnline(PATH1_BASE, H);
		}
		else if (argc == 5) {
			cout << "Batch single image mode (POS_DIR, NEG_DIR)." << endl;
			if (!(dir1 = opendir(argv[2]))) {
				error("detect: [POS_DIR] open failed.");
			}
			if (!(dir2 = opendir(argv[3]))) {
				error("detect: [NEG_DIR] open failed.");
			}
			fin.open(argv[4]);
			if (!fin) {
				error("detect: [MODEL_FILE] open failed.");
			}
			/*** Batch single image mode with [POS_DIR], [NEG_DIR] ***/
			/* Set PATH1_BASE to [POS_DIR] and append slash */
			strcpy(PATH1_BASE, argv[2]);	
			sprintf(PATH1_BASE, "%s%c", PATH1_BASE, slash);
			/* Set PATH2_BASE to [NEG_DIR] and append slash */
			strcpy(PATH2_BASE, argv[3]);	
			sprintf(PATH2_BASE, "%s%c", PATH2_BASE, slash);
			/* Read model parameters into AdaStrong vector */
			readModel(fin, H);
			detectSingleOffline(PATH1_BASE, PATH2_BASE, H);			
		}
		else {
			printUsage();
		}
	}
	/* Invalid argv[1] */
	else {
		printUsage();
	}

	fin.close();
	return 0;
}
