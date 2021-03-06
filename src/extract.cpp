/** File: extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: 2010/04/03
 ** Description: The implementations of car feature extraction
    Features:
      1. Rectangle features
      2. Edge Orientation Histograms (EOH)
      3. Edge Density (ED)
    Notice the use of integral histogram in computation.
 **/

#include "extract.h"

void extractAll(char *PATH_BASE, CvMat *&POOL, int &N, int &blockCount) {
	DIR *dir;
	struct dirent *dp;
	char PATH[MAX_PATH_LENGTH];
	IplImage *img;
	int n = 0; // The actual # of images

	/* Roughly count # of images */
	N = countImages(PATH_BASE);
	cout << "\nRough image count of " << PATH_BASE << ": " << N << endl;
#if GETCHAR
	getchar();
#endif

	try {
		if (!(dir = opendir(PATH_BASE))) {
			throw "Directory open exception";
		}

		int Iid = 0; /* Image ID */
		float *ptr;  /* Pointer to some "float" matrix element */
		while ((dp = readdir(dir))) {
			/* The full path is PATH_BASE + FILENAME */
			sprintf(PATH, "%s%s", PATH_BASE, dp->d_name);

			/* If it is an image file (read it in grayscale) */
			if ((img = cvLoadImage(PATH, 0))) {
				n++;  // Count actual # of images
				cout << PATH << " ... ";

				/* check image sizes */
				if ((img->width != WINDOW_WIDTH) || (img->height != WINDOW_HEIGHT)) {
					char *msg = NULL;
					sprintf(msg, "Image size should be %d x %d.\n", WINDOW_WIDTH, WINDOW_HEIGHT);
					error(msg);
				}
				/* For the first image: also count # of blocks and create the large matrix */
				if (Iid == 0) {
					blockCount = countBlocks(img);
					POOL = cvCreateMat(blockCount * N, FEATURE_COUNT, CV_32FC1);
					ptr = (float *) POOL->data.ptr;
				}

				/** Extract features for this image **/
				extractImg(img, ptr);
				cvReleaseImage(&img);
				cout << "extracted.\n";
				Iid++;
			}
		}
		closedir(dir);
	}
	catch (const char * e) {
		cerr << "extractAll(): " << e << endl;
	}

	if (n != N) {
		cout << "extractAll(): Warning: " << PATH_BASE << " contains non-image files.\n";
		cout << "Rough count: " << N << ". Actual count: " << n << endl;
		N = n;  // Reset N to the actual # of images counted by n
		/* In fact, this situation won't ruin the training process and is acceptable. */
		getchar();
	}

//	printMat(POOL, "POOL in extractAll()");
//	getchar();
}

int countBlocks(IplImage *img) {
	int ret = 0;
	/* try all possible sizes and positions within the image */
	for (int bHeight = BLOCK_MIN_SIZE; bHeight <= WINDOW_HEIGHT; bHeight += BLOCK_SIZE_INC) {
		for (int bWidth = BLOCK_MIN_SIZE; bWidth <= WINDOW_WIDTH; bWidth += BLOCK_SIZE_INC) {
			//int subTotal = 0;
			for (int y = 0; y + bHeight - 1 < WINDOW_HEIGHT; y += BLOCK_POS_INC) {
				for (int x = 0; x + bWidth - 1 < WINDOW_WIDTH; x += BLOCK_POS_INC) {

					ret++;
					//subTotal++;
					//cout << bHeight << " x " << bWidth << ": (" << yBeg << "," << xBeg <<
					//") to (" << yEnd << "," << xEnd << ")\n";
				}
			}
			//cout << "Subtotal: " << subTotal << endl;
			//getchar();
		}
	}
	//cout << "Total: " << ret << endl;
	return ret;
}

/* Extract features from a single image */
/** Be careful with the PERFORMANCE in this function and its subroutines **/
void extractImg(IplImage *img, float *&ptr) {
	/* data is the pointer to a single matrix: POOL[ Iid ] */
	int Bid = 0; // block ID

#if TIMER_EXTRACT
	clock_t tic, toc;
	tic = clock();
#endif

	/* [1.1] For rectangle features: first, perform Mean and variance normalization */
	int W = img->width, H = img->height, N = W * H;
	/* "IPL_DEPTH_32F" converted image, square image, and normalized image */
	IplImage *cvtImg, *sqImg, *normImg;

	cvtImg = cvCreateImage(cvSize(W, H), IPL_DEPTH_32F, 1);
	cvConvert(img, cvtImg);
	normImg = cvCloneImage(cvtImg);
	sqImg = cvCloneImage(cvtImg);
	cvPow(sqImg, sqImg, 2);
	/* Mean and variance normalization */
	meanVarNorm(normImg, cvSum(img).val[ 0 ], cvSum(sqImg).val[ 0 ], N);
	cvReleaseImage(&sqImg);

	/* [1.2] Use cvIntegral() to compute the integral image of normImg */
	IplImage *normSum;  // in "IPL_DEPTH_64F"
	normSum = cvCreateImage(cvSize(W + 1, H + 1), IPL_DEPTH_64F, 1);
	cvIntegral(normImg, normSum);
//	printMat(normImg, "normImg", 0, 14, 0, 14);
//	printMat(normSum, "normSum", 1, 15, 1, 15);
	cvReleaseImage(&normImg);


	/* [2] For EOH features */

	/* [3] For ED features */



	/* Release cvtImg */
	cvReleaseImage(&cvtImg);

	/* [4] Extract these features from each block */
	/* try all possible sizes and positions within the image */
	for (int bHeight = BLOCK_MIN_SIZE; bHeight <= WINDOW_HEIGHT; bHeight += BLOCK_SIZE_INC) {
		for (int bWidth = BLOCK_MIN_SIZE; bWidth <= WINDOW_WIDTH; bWidth += BLOCK_SIZE_INC) {
			for (int y = 0; y + bHeight - 1 < WINDOW_HEIGHT; y += BLOCK_POS_INC) {
				for (int x = 0; x + bWidth - 1 < WINDOW_WIDTH; x += BLOCK_POS_INC) {
					/** Consider the dummy 1st row and 1st column produced by cvIntegral() **/
					extractBlock(Bid++, ptr, normSum, x + 1, bWidth, y + 1, bHeight);
				}
			}
		}
	}


	/* Release remaining images */
	cvReleaseImage(&normSum);

#if TIMER_EXTRACT
	toc = clock();
	runningTime(tic, toc);
	getchar();
#endif

}

/* Feature extraction of a single block */
/** Be careful with the PERFORMANCE in this function and its subroutines **/
void extractBlock(int Bid, float *&ptr, IplImage *normSum, int x, int W, int y, int H) {
	/* define the 1st quarter, middle, 3rd quarter, and endpoint coordinates */
	const int xEnd = x + W - 1;
	const int xMid = (x + xEnd) >> 1;
	const int xQ1 = (x + xMid) >> 1;
	const int xQ3 = (xMid+1 + xEnd) >> 1;
	const int yEnd = y + H - 1;
	const int yMid = (y + yEnd) >> 1;
	const int yQ1 = (y + yMid ) >> 1;
	const int yQ3 = (yMid+1 + yEnd) >> 1;
	//const int area = (xEnd - x + 1) * (yEnd - y + 1);

	/* [1.1] 5 upright rectangle features */
	*ptr++ = recSumRight(normSum, x, y, xEnd, yMid) - recSumRight(normSum, x, yMid + 1, xEnd, yEnd);
	*ptr++ = recSumRight(normSum, x, y, xMid, yEnd) - recSumRight(normSum, xMid + 1, y, xEnd, yEnd);
	*ptr++ = recSumRight(normSum, x, y, xMid, yMid) - recSumRight(normSum, xMid + 1, y, xEnd, yMid) -
		recSumRight(normSum, x, yMid + 1, xMid, yEnd) + recSumRight(normSum, xMid + 1, yMid + 1, xEnd, yEnd);
	*ptr++ = recSumRight(normSum, x, y, xEnd, yQ1) - recSumRight(normSum, x, yQ1 + 1, xEnd, yQ3) +
		recSumRight(normSum, x, yQ3 + 1, xEnd, yEnd);
	*ptr++ = recSumRight(normSum, x, y, xQ1, yEnd) - recSumRight(normSum, xQ1 + 1, y, xQ3, yEnd) +
		recSumRight(normSum, xQ3 + 1, y, xEnd, yEnd);

	/* [1.2] 5 tilted rectangle features (NOT YET implemented) */

	/* [2] 9 EOH features */

	/* [3] 1 ED feature */

}

