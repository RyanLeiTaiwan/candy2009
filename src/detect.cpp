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
	
	cout << "Model read successfully.\n# of AdaBoost stages = " << H.size() << endl;
}

/* Classify a single image using cascaded AdaBoost */
float classifyCascade(IplImage *img, vector<AdaStrong> &H) {
#if TIMER_DETECT
	clock_t tic, toc;
	tic = clock();
#endif
	float score = 0.f;  // Sum of all the positive H(x)
	
	assert(img->width == WINDOW_WIDTH && img->height == WINDOW_HEIGHT);
	/* countBlocks() just this once */
	static int blockCount = countBlocks(img);
	
	/* Feature extraction */
	CvMat *POOL = cvCreateMat(blockCount, FEATURE_COUNT, CV_32FC1);
	float *ptr = (float *) POOL->data.ptr;  // Pointer to a CvMat element */
	extractImg(img, ptr);

#if 0
	//Pointer access Example:
	cout << "cvGetReal2D(10, 3) = " << cvGetReal2D(POOL, 10, 3) << endl;
	cout << "ptr(10, 3) = " << *(float *) (POOL->data.ptr + 10 * POOL->step + 3 * sizeof(float)) << endl;
#endif
	
	/* Vector iterators */
	vector<AdaStrong>::iterator itStrong;
	vector<AdaWeak>::iterator itWeak;
	for (itStrong = H.begin(); itStrong != H.end(); itStrong++) {
		/* H(x) = sign( sum[ weight_t * h_t(x) ] - threshold ) */
		float result = 0.f;
		float threshold = itStrong->threshold;
		
		for (itWeak = itStrong->h.begin(); itWeak != itStrong->h.end(); itWeak++) {
			int Bid = itWeak->Bid;
			int Fid = itWeak->Fid;
			short parity = itWeak->parity;
			float decision = itWeak->decision;
			float weight = itWeak->weight;
			
			float value = *(float *) (POOL->data.ptr + Bid * POOL->step + Fid * sizeof(float));
			/* AdaWeak determines it is positive */
			if (parity * value >= parity * decision) {
				result += weight;
			}
			/* AdaWeak determines it is positive */
			else {
				result -= weight;
			}
		} // End of loop itWeak
		
		/* Adjust result by AdaStrong's threshold */
		result -= threshold;
		
		/* If AdaStrong determines it is negative, return -1 immediately */
		if (result < 0.f) {
			cout << "[-]: Rejected at stage " << itStrong - H.begin() + 1 << endl;
#if TIMER_DETECT
			toc = clock();
			runningTime(tic, toc);
#endif
			return -1.f;
		}
		/* If AdaStrong determines it is positive, add result to score */
		else {
			score += result;
		}

	} // End of loop itStrong
	
	/* positive */
	cout << "[+]: Score = " << score << endl;
#if TIMER_DETECT
	toc = clock();
	runningTime(tic, toc);
#endif
	
	return score;
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
	
	srand(time(NULL));
	
	
	
	cout << "\nPress any key for the next image. Press [ESC] to exit." << endl;
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

				/* Classify the image, then put text 'O' or 'X' onto colorImg based on the result */
				if (classifyCascade(cvtImg, H) >= 0) {
					cvPutText(colorImg, "O", cvPoint(FONT_X, FONT_Y), font, GREEN);
				}
				else {
					cvPutText(colorImg, "X", cvPoint(FONT_X, FONT_Y), font, RED);
				}

				/* Show the (possibly) color image */
				cvResizeWindow("Detection in single image mode", CVWINDOW_WIDTH, CVWINDOW_HEIGHT);
				cvShowImage("Detection in single image mode", colorImg);
				cvReleaseImage(&colorImg);

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