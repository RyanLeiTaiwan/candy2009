/** File: gen_neg.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/26
 ** Modification: 2009/12/27
 ** Description: Generate multiple negative images chopped
 **              from large ones.
 **/

/* Usage: gen_neg [INPUT_DIR] [OUTPUT_DIR] */ 
#include <iostream>
#include <cstring>
#include <cxcore.h>
#include <cv.h>
#include <highgui.h> // for testing
#include <dirent.h>
#include "global.h"
using namespace std;

int main(int argc, char *argv[]) {
	DIR *dirin, *dirout;
	struct dirent *dp;
	char INPUT_PATH_BASE[ MAX_PATH_LENGTH ];
	char INPUT_PATH[ MAX_PATH_LENGTH ];
	char OUTPUT_PATH_BASE[ MAX_PATH_LENGTH ];
	char OUTPUT_PATH[ MAX_PATH_LENGTH ];
	
	if (argc != 3) {
		cout << "Usage: gen_neg [INPUT_DIR] [OUTPUT_DIR]" << endl;
		exit(EXIT_FAILURE);
	}
	
	if (!(dirin = opendir(argv[ 1 ]))) {
		cout << "gen_neg(): [INPUT_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	/* Set INPUT_PATH_BASE to [INPUT_DIR] and append '/' or '\' */
	strcpy(INPUT_PATH_BASE, argv[ 1 ]);
	char slash = Unix ? '/' : '\\'; // It is '/' or '\' depending on OS
	sprintf(INPUT_PATH_BASE, "%s%c", INPUT_PATH_BASE, slash);
	
	if (!(dirout = opendir(argv[ 2 ]))) {
		cout << "gen_neg(): [OUTPUT_DIR] does not exist.\n";
		exit(EXIT_FAILURE);
	}
	closedir(dirout); // not needed for output
	/* Set OUTPUT_PATH_BASE to [OUTPUT_DIR] and append '/' or '\' */
	strcpy(OUTPUT_PATH_BASE, argv[ 2 ]);
	sprintf(OUTPUT_PATH_BASE, "%s%c", OUTPUT_PATH_BASE, slash);
	
	/** Command is correct **/
	cout << "This program will take in large-sized images in [INPUT_DIR],\n" <<
		"ASSUMING THE ASPECT RATIO is the same as " << LARGE_WIDTH << " : " << LARGE_HEIGHT << 
	", then:\n 1. Resize each image to exactly " << LARGE_WIDTH << " x "<< LARGE_HEIGHT << ".\n" <<
	" 2. Discard the 4 sides of the image by window size: " << WINDOW_WIDTH << " x " << WINDOW_HEIGHT << ".\n" <<
	" 3. Scan using the window, and write the new images into [OUTPUT_DIR]/genCOUNT.jpg,\n" <<
	"    where COUNT is a digit counting from 1.\n\n" <<
	"Press [Enter] to continue, or ctrl+C/D/Z to exit ...";
	getchar();
	
	int COUNT = 1;
	IplImage *img;
	/* For each image file in [INPUT_DIR] */
	while (dp = readdir(dirin)) {		
		/* The full input path is [INPUT_DIR] + "/" + FILENAME */
		strcpy(INPUT_PATH, INPUT_PATH_BASE);
		strcat(INPUT_PATH, dp->d_name);
		cout << INPUT_PATH << "   ......   ";

		/* Read the image */		
		if (img = cvLoadImage(INPUT_PATH, -1)) {
			/* Success on image read */
			/* Resize it to WINDOW_WIDTH x WINDOW_HEIGHT */
			////////cvResize();
			/* For different ROI's (Regions of Interest) */
			cvSetImageROI(img, cvRect(200, 200, 640, 480));
			/* The full output path is [OUTPUT_DIR] + "/" + genCOUNT.jpg */
			strcpy(OUTPUT_PATH, OUTPUT_PATH_BASE);
			sprintf(OUTPUT_PATH, "%s%d.jpg", OUTPUT_PATH, COUNT);
			//cout << "OUTPUT_PATH: " << OUTPUT_PATH << endl;
			if (cvSaveImage(OUTPUT_PATH, img)) {
				cout << "done\n";
			}
			else {
				cout << "failure\n";
				exit(EXIT_FAILURE);
			}

			
			COUNT++;
			
			
			cvReleaseImage(&img);
		}
		else {
			/* Failure on image read: skip this file */
			cout << "not an image\n";
		}

	}
	
	
	
	return 0;
}
