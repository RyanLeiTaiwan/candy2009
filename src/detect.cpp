/** File: detect.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/26
 ** Modification: 2010/04/03
 ** Description: The implementations of the car-detection program.
 **/

#include "detect.h"

/* Print program usage: reused a lot */
void printUsage() {
	cout << "Usage:" << endl;
	cout << "1. detect -c [MODEL_FILE] : Online detection using a camera." << endl;
	cout << "2. detect -v [VIDEO_FILE] [MODEL_FILE] : Online detection using a video file." << endl;
	cout << "3. detect -s [TEST_DIR] [MODEL_FILE] : Online detection using single images in one directory." << endl;
	cout << "4. detect -s [POS_DIR] [NEG_DIR] [MODEL_FILE] : Offline batch detection using POS & NEG directories." << endl;
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

		/* If AdaStrong determines it is negative, return "-stages" immediately */
		if (result < 0.f) {
			return -(itStrong - H.begin() + 1);
		}
		/* If AdaStrong determines it is positive, add result to score */
		else {
			score += result;
		}

	} // End of loop itStrong

	/* positive */
	return score;
}

/* Online single image mode with directory [TEST_DIR] */
void detectSingleOnline(char *TEST_DIR, vector<AdaStrong> &H) {
	DIR *dir;
	struct dirent *dp;
	char PATH[MAX_PATH_LENGTH];
#if TIMER_SINGLE_ONNLINE
	clock_t tic, toc;
#endif

	IplImage *colorImg, *resizedImg;
	IplImage *grayImg, *cvtImg;
	CvFont *font = new CvFont();
	cvInitFont(font, CV_FONT_HERSHEY_TRIPLEX, 4, 4, 0, 4, 8);
	CvScalar RED = cvScalar(0, 0, 255);
	CvScalar GREEN = cvScalar(0, 255, 0);
	//int

	cout << "\nPress any key for the next image. Press [ESC] to exit." << endl;
	try {
		if (!(dir = opendir(TEST_DIR))) {
			throw "Directory open exception";
		}

		/* Create an OpenCV NamedWindow */
		cvNamedWindow("Detection in single image mode", CV_WINDOW_AUTOSIZE);
		cvMoveWindow("Detection in single image mode", CVWINDOW_X, CVWINDOW_Y);

		/* For all image files in [TEST_DIR] */
		while ((dp = readdir(dir))) {
			/* The full path is TEST_DIR + FILENAME */
			sprintf(PATH, "%s%s", TEST_DIR, dp->d_name);

			/* If it is an image file */
			if ((colorImg = cvLoadImage(PATH, CV_LOAD_IMAGE_UNCHANGED))) {
				/* Resize image by myself since cvResizeWindow() does not work with QTKit or Cocoa on Mac */
				resizedImg = cvCreateImage(cvSize(RESIZE_WIDTH, RESIZE_HEIGHT), IPL_DEPTH_8U, 3);
				cvResize(colorImg, resizedImg);
				cvReleaseImage(&colorImg);
				cout << PATH << " ... ";

				/* cvLoadImage again because cvConvertImage() is somehow buggy */
				grayImg = cvLoadImage(PATH, CV_LOAD_IMAGE_GRAYSCALE);

				/* Convert into 32-bit float */
				cvtImg = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_32F, 1);
				cvConvert(grayImg, cvtImg);
				cvReleaseImage(&grayImg);

				/* Classify the image, show the text result (and running time),
				 * then put text 'O' or 'X' onto colorImg based on the result
				 */
#if TIMER_SINGLE_ONNLINE
				tic = clock();
#endif
				float score = classifyCascade(cvtImg, H);
				cvReleaseImage(&cvtImg);
#if TIMER_SINGLE_ONNLINE
				toc = clock();
#endif

				if (score >= 0) {
					cout << "[+]: Score = " << score << endl;
#if TIMER_SINGLE_ONNLINE
					runningTime(tic, toc);
#endif
					cvPutText(resizedImg, "O", cvPoint(FONT_X, FONT_Y), font, GREEN);
				}
				else {
					cout << "[-]: Rejected at stage " << (int)-score << endl;
#if TIMER_SINGLE_ONNLINE
					runningTime(tic, toc);
#endif
					cvPutText(resizedImg, "X", cvPoint(FONT_X, FONT_Y), font, RED);
				}

				/* Show the resized (possibly) color image */
				cvShowImage("Detection in single image mode", resizedImg);
				cvReleaseImage(&resizedImg);

				/* Press [ESC] to exit */
				if (cvWaitKey(0) == 27) {
					exit(EXIT_SUCCESS);
				}
			}
		}
		cvDestroyWindow("Detection in single image mode");

	}
	catch (const char *e) {
		cerr << "detectSingleOnline(): " << e << endl;
	}
}

/* Offline batch single image mode with directories [POS_DIR], [NEG_DIR] */
void detectSingleOffline(char *POS_DIR, char *NEG_DIR, vector<AdaStrong> &H) {
	DIR *dir;
	struct dirent *dp;
	char PATH[MAX_PATH_LENGTH];
	clock_t tic, toc;
	tic = clock();
	IplImage *grayImg, *cvtImg;
	int N1 = 0, N2 = 0; /* positive / negative image count */

	/*** Negative detection ***/
	int error = 0; /* error count */
	try {
		if (!(dir = opendir(NEG_DIR))) {
			throw "Directory open exception";
		}
		/* For all image files in [NEG_DIR] */
		while ((dp = readdir(dir))) {
			/* The full path is NEG_DIR + FILENAME */
			sprintf(PATH, "%s%s", NEG_DIR, dp->d_name);

			/* If it is an image file (read it in grayscale) */
			if ((grayImg = cvLoadImage(PATH, 0))) {
				/* Convert into 32-bit float */
				cvtImg = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_32F, 1);
				cvConvert(grayImg, cvtImg);
				cvReleaseImage(&grayImg);

				/* Classify the image, then sum the error if misclassified */
				N1++;
				float score = classifyCascade(cvtImg, H);
				if (score >= 0) {
					error++;
				}
				cvReleaseImage(&cvtImg);
			}
		}

	}
	catch (const char *e) {
		cerr << "detectSingleOffline(): " << e << endl;
	}
	/* Show the batch detection result */
	cout << "\nfalse positive:  " << fixed << setprecision(1) << error / (double)N1 * 100.0 << "% ("
		<< error << " / " << N1 << ") " << endl;


	/*** Positive detection ***/
	error = 0;
	try {
		if (!(dir = opendir(POS_DIR))) {
			throw "Directory open exception";
		}
		/* For all image files in [POS_DIR] */
		while ((dp = readdir(dir))) {
			/* The full path is POS_DIR + FILENAME */
			sprintf(PATH, "%s%s", POS_DIR, dp->d_name);

			/* If it is an image file (read it in grayscale) */
			if ((grayImg = cvLoadImage(PATH, 0))) {
				/* Convert into 32-bit float */
				cvtImg = cvCreateImage(cvSize(WINDOW_WIDTH, WINDOW_HEIGHT), IPL_DEPTH_32F, 1);
				cvConvert(grayImg, cvtImg);
				cvReleaseImage(&grayImg);

				/* Classify the image, then sum the error if misclassified */
				N2++;
				float score = classifyCascade(cvtImg, H);
				if (score < 0) {
					error++;
				}
				cvReleaseImage(&cvtImg);
			}
		}

	}
	catch (const char *e) {
		cerr << "detectSingleOffline(): " << e << endl;
	}
	/* Show the batch detection result */
	toc = clock();
	cout << "false negative:  " << fixed << setprecision(1) << error / (double)N2 * 100.0 << "% ("
	<< error << " / " << N2 << ") " << endl;

	avgRunningTime(tic, toc, N1 + N2);

}
