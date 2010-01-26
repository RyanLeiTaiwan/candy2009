/** File: detect.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/26
 ** Modification: XXXX/XX/XX
 ** Description: The implementations of the car-detection program.
 **/

#include "detect.h"

/* Print program usage: reused a lot */
void printUsage() {
	cout << "Usage:" << endl;
	cout << "1. detect -c [MODEL_FILE] : Online detection using a camera." << endl;
	cout << "2. detect -v [VIDEO_FILE] [MODEL_FILE] : Online detection using a video file." << endl;
	cout << "3. detect -s [TEST_DIR] [MODEL_FILE] : Online detection using single images in one directory." << endl;
	cout << "4. detect -s [POS_DIR] [NEG_DIR] [MODEL_FILE] : Offline detection using POS & NEG directories." << endl;	
	exit(EXIT_FAILURE);
}

/* Read the model parameters to vector<AdaStrong> H */
void readModel(ifstream &fin, vector<AdaStrong> &H) {
	char buffer[MAX_LINE_LENGTH];
	int stage;
	int weakNum;
	float threshold;
	int Bid, Fid;
	short parity;
	float decision, weight;
	
	/* Check leading model identifier */
	fin.getline(buffer, MAX_LINE_LENGTH);
	if(strcmp(buffer, "[Candy2009]")) {
		error("readModel(): Model identifier error. Should be [Candy2009].");
	}
	/* # of AdaBoost stages */
	fin >> stage;
	
	/* For each stage (AdaStrong) */
	for (int k = 0; k < stage; k++) {
		H.push_back(AdaStrong());
		/* # of weak classifiers, threshold */
		fin >> weakNum >> threshold;
		H.back().threshold = threshold;
		
		/* For each AdaWeak in this AdaStrong */
		for (int t = 0; t < weakNum; t++) {
			H.back().h.push_back(AdaWeak());
			/* Bid, Fid, parity, decision, weight */
			fin >> Bid >> Fid >> parity >> decision >> weight;
			H.back().h.back().setValue(Bid, Fid, parity, decision, weight);
		}
	}
	
	/* Get rid of an "\n" */
	fin.get();
	/* Check trailing model identifier */
	fin.getline(buffer, MAX_LINE_LENGTH);
	if(strcmp(buffer, "[Candy2009]")) {
		error("readModel(): Model identifier error. Should be [Candy2009].");
	}	
	
	cout << "Model read successfully." << endl;
}

/* Classify a single image using cascaded AdaBoost */
int classifyCascade(IplImage *img, vector<AdaStrong> &H) {
	;
	return 100;
}

/* Online single image mode with directory [TEST_DIR] */
void detectSingleOnline(char *TEST_DIR, vector<AdaStrong> &H) {
	DIR *dir;
	struct dirent *dp;
	char PATH[MAX_PATH_LENGTH];
	
	IplImage *colorImg, *grayImg, *cvtImg;
	CvFont *font = new CvFont();
	cvInitFont(font, CV_FONT_HERSHEY_TRIPLEX, 1, 1, 0, 1, 8);
	CvScalar RED = cvScalar(0, 0, 255);
	CvScalar GREEN = cvScalar(0, 255, 0);
	
	/* [1] Read the model parameters */
	
	
	
	cout << "\nPress [Space] for the next image. Press [ESC] to exit." << endl;
	try {
		if (!(dir = opendir(TEST_DIR))) {
			throw "Directory open exception";
		}
		
		/* Create an OpenCV NamedWindow */
		cvNamedWindow("Detection in single image mode");
		cvMoveWindow("Detection in single image mode", CVWINDOW_X, CVWINDOW_Y);
		/* [2] For all image files in [TEST_DIR] */
		while (dp = readdir(dir)) {
			/* The full path is TEST_DIR + FILENAME */
			sprintf(PATH, "%s%s", TEST_DIR, dp->d_name);

			/* If it is an image file */
			if (colorImg = cvLoadImage(PATH, -1)) {
				cout << PATH << " ... ";
				/* Convert into 8-bit grayscale (required to be 8-bit) */
				grayImg = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_8U, 1);
				cvConvertImage(colorImg, grayImg);
				/* Convert into 32-bit float */
				cvtImg = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_32F, 1);
				cvConvert(grayImg, cvtImg);
				cvReleaseImage(&grayImg);
				
				/* Put text in colorImg */
				cvPutText(colorImg, "X", cvPoint(FONT_X, FONT_Y), font, RED);
				cvPutText(colorImg, "O", cvPoint(FONT_X, FONT_Y), font, GREEN);

				/* Show the (possibly) color image */
				cvResizeWindow("Detection in single image mode", CVWINDOW_WIDTH, CVWINDOW_HEIGHT);
				cvShowImage("Detection in single image mode", colorImg);
				cvReleaseImage(&colorImg);

				cout << "done." << endl;				
				/* Press [ESC] to exit */
				if (cvWaitKey(0) == 27) {
					exit(EXIT_SUCCESS);
				}
				
			}
		}
		cvDestroyWindow("Detection in single image mode");
		
	}
	catch (const char *e) {
		cerr << "extractAll(): " << e << endl;
	}
}