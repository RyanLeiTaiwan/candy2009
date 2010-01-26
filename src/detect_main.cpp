/** File: detect_main.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/26
 ** Modification: XXXX/XX/XX
 ** Description: The main function of the car-detection program based on the paper:
 **   Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
 **/

/* Usage:
 * 1. detect -c [MODEL_FILE] : Online detection using a camera.
 * 2. detect -v [VIDEO_FILE] [MODEL_FILE] : Online detection using a video file.
 * 3. detect -s [TEST_DIR] [MODEL_FILE] : Online detection using single images in one directory.
 * 4. detect -s [POS_DIR] [NEG_DIR] [MODEL_FILE] : Offline detection using POS & NEG directories.
 */
#include "detect.h"

int main(int argc, char *argv[]) {
	
	
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
			cout << "Single image mode (TEST_DIR). Under construction." << endl;
		}
		else if (argc == 5) {
			cout << "Single image mode (POS_DIR, NEG_DIR). Under construction." << endl;
		}
		else {
			printUsage();
		}
	}
	/* Invalid argv[1] */
	else {
		printUsage();
	}

	return 0;
}
