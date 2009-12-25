/** File: car-detect.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/27
 ** Modification: 2009/09/28
 ** Description: The car detection process given the training result of parameters.
 **/
#include "../include/car-detect.h"

void detect_all_images( short label, char *directory, FILE *in, FILE *fout ) {

	char fileName[ 80 ];
	int pathLen;
	FILE *ftest;
	char D1, D2, D3; /* max # of images: 1000 */
	int Bid, blockCount; /* block id, # of blocks in an image */
	float ***POOL;

	/* only support labels of +1 and -1 */
	if ( label != +1 && label != -1 ) {
		error( "detect_all_images(): Label shoold be +1 or -1." );
	}

	strcpy( fileName, directory );
	pathLen = strlen( fileName );
    /* append '/' if necessary (unix) */
    if ( fileName[ pathLen - 1 ] != '/' ) {
        fileName[ pathLen++ ] = '/';
        fileName[ pathLen ] = '\0';
    }
	sprintf( fileName, "%stest000.bmp", fileName );

	/* Count the number of blocks per image, 
	 * and allocate the feature pool memory (3-D array) for "one image".
	 * So the first dimension of the pool is 1 */
	blockCount = count_blocks();
	POOL = (float ***) malloc( 1 * sizeof( float ** ) );
	POOL[ 0 ] = (float **) malloc( blockCount * sizeof( float * ) );
	for ( Bid = 0; Bid < blockCount; Bid++ ) {
		POOL[ 0 ][ Bid ] = (float *) malloc( FEATURE_COUNT * sizeof( float ) );
	}

	/* Scan each image */
	for ( D1 = '0'; D1 <= '9'; D1++ ) {
		for ( D2 = '0'; D2 <= '9'; D2++ ) {
			for ( D3 = '0'; D3 <= '9'; D3++ ) {
				fileName[ pathLen + 4 ] = D1;
				fileName[ pathLen + 5 ] = D2;
				fileName[ pathLen + 6 ] = D3;
				/* termination condition */
				if ( !( ftest = fopen( fileName, "r" ) ) ) {
					return;
				}
				else {
					fclose( ftest );
					extract_image( fileName, 0, POOL );
				}
			}
		}
	}


}

/********************************     start of main     **********************************/
int main( int argc, char *argv[] ) {
	time_t tic, toc;
	FILE *fin, *fout;

	/* Usage: car-detect POS NEG input output
	 * POS: directory of positive test set
	 * NEG: directory of negative test set
	 * input: the model file created by car-train
	 * output: output file of the detection result
	 * The images in POS and NEG should have file names: trainDDD.bmp, D form [0,9],
	 * and starts with train000.bmp without discontinuities.
	 */
	if ( argc != 5 ) {
		error( "Usage: car-detect POS NEG Inout Output.\n"
		"Example: ./car-detect pics/INRIA/test_car pics/INRIA/test_nocar train.txt output/detect.txt" );
	}
	/* check file open */
	if ( !( fin = fopen( argv[ 3 ], "w" ) ) ) {
		error( "car-detect(): Output file open error." );
	}
	if ( !( fout = fopen( argv[ 4 ], "w" ) ) ) {
		error( "car-detect(): Output file open error." );
	}
	/* start timer */
	tic = clock();
	
	/*** the car detection process ***/
	detect_all_images( +1, argv[ 1 ], fin, fout );
	detect_all_images( -1, argv[ 2 ], fin, fout );
		
    /* stop timer */
    toc = clock();
    runningTime( tic, toc );
	fclose( fin );
	fclose( fout );

	return 0;
}

