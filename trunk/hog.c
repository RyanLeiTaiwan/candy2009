/** File: hog.c
 ** Author: Ryan Lei
 ** Creation: 2009/07/05
 ** Modification: XXXX/XX/XX
 ** Description: 
 **   feature extraction based on
 **   HOG: Histogram of Oriented Gradients.
 **   the features vectors are then fed to NTU LibSVM
 **/

#include "image.h"

/*** HOG Parameters:
 * [A] centered/uncentered gradients
 * [B] # of orientation bins
 * [C] c x c cell, 2c x 2c blocks (each block contains 2 x 2 cells)
 * [D] sigma, the Gaussian stddev, for the weighted vote
 * => feature vector dimension = (2*2*binNum) x 1
***/
void HOG( Matrix *img, char *outFileName, bool centered, int binNum, int cellSize, float sigma ) {
	Matrix RGBimg[ 3 ]; /* array of 2D matrices, each being the RGB components of img */
	/* Hor: horizontal, Ver: vertical, Mag: magnitude, Bin: orientation bin */
	Matrix gradHor[ 3 ], gradVer[ 3 ], gradMag, gradBin;
	int M = img->size1, N = img->size2;
	int i, row, col;
	int blockSize = cellSize * 2;

	/*** [0] Pre-processing ***/

	/*** [1] Compute gradients ***/
	/* Separate img into RGBimg[ 0~2 ].
	 * Calculate gradients for RGBimg[ 0~2 ] */
	for ( i = 0; i < 3; i++ ) { 
		zeros( &RGBimg[ i ], M, N, 1 );
		full_assign( img, &RGBimg[ i ], (COLOR)i, RR );
		gradient( &RGBimg[ i ], &gradHor[ i ], horizontal, true );
		gradient( &RGBimg[ i ], &gradVer[ i ], vertical, true );
	}
	/* Compute the magnitude of the gradient and compare with the largest among R,G,B.
	 * if it IS the largest, also compute the angle(bin) of the gradient */
	zeros( &gradMag, M, N, 1 );
	zeros( &gradBin, M, N, 1 );
	for ( row = 0; row < M; row++ ) {
		for ( col = 0; col < N; col++ ) {
			for ( i = 0; i < 3; i++ ) {
				float Fh = gradHor[ i ].data[ 0 ][ row ][ col ]; 
				float Fv = gradVer[ i ].data[ 0 ][ row ][ col ];
				float mag = sqrt( Fh * Fh + Fv * Fv );
				/* is largest ( automatically excludes the (0,0) case ) */
				if ( mag > gradMag.data[ 0 ][ row ][ col ] ) { 
					float ang;
					int bin, binSize = 360 / binNum;
					gradMag.data[ 0 ][ row ][ col ] = mag;
					/* compute the angle and adjust for II, III, IV quadrants */
					ang = atan( Fv / Fh ) / M_PI * 180.f; /* arc tangent */
					if ( Fh < 0 ) { /* II, III */
						ang += 180.f;
					}
					else if ( Fv < 0 ) { /* IV */
						ang += 360.f;
					}
					assert( ang >= 0 && ang < 360.f );
					/* convert angle into orientation bin */
					bin = (int)roundf( ang ) / binSize;
					gradBin.data[ 0 ][ row ][ col ] = bin;	
					/* printf( "layer%d: (%.0f,%.0f): mag = %f, ang = %.0f, bin = %d\n",
						i, Fh, Fv, mag, ang, bin ); */
				}

			}
		}
	}
	
	/*** [2] Weighted vote into spatial & orientation cells ***/
	
	/* free memory space */
	for ( i = 0; i < 3; i++ ) {
		freeMatrix( &RGBimg[ i ] );
		freeMatrix( &gradHor[ i ] );
		freeMatrix( &gradVer[ i ] );
	}
	freeMatrix( &gradMag );
	freeMatrix( &gradBin );
}

int trainEach( char *fileName, int pathLen, bool centered, int binNum, int cellSize, float sigma ) {
	FILE *ftest;
	Matrix img;
	char D1, D2, D3; /* each from '0' ~ '9' */
	int imgCount = 0;

	/* for each image: */
	for ( D1 = '0'; D1 <= '0'; D1++ ) {
		for ( D2 = '0'; D2 <= '0'; D2++ ) {
			for ( D3 = '0'; D3 <= '0'; D3++ ) {
				fileName[ pathLen + 5 ] = D1;
				fileName[ pathLen + 6 ] = D2;
				fileName[ pathLen + 7 ] = D3;
				/* termination condition */
				if ( !( ftest = fopen( fileName, "r" ) ) ) {
					return imgCount;
				}
				else {
					fclose( ftest );
					/* imread(), HOG(), freeMatrix() */
					imread( fileName, &img );
					/***** second argument should change into file pointer *****/
					HOG( &img, "pics/output/HOG_train.txt", centered, binNum, cellSize, sigma );
					imgCount++;
					printf( "%s extracted.\n", fileName );
					freeMatrix( &img );
				}
			}
		}
	}
	return imgCount;
}

int main( int argc, char *argv[] ) {
	/* Usage: ./run "XX/YY",
	 * then the program will train using all pictures in directory
	 *   XX/YY
	 * image file format: trainDDD.bmp, D from [0,9]
	 * must start from train000.bmp and have no gap in the numbers
	 */
	char fileName[ 60 ];
	int pathLen, imgCount;
	clock_t tic, toc;

	/*** HOG Parameters:
	 * [A] centered/uncentered gradients
	 * [B] # of orientation bins
	 * [C] c x c cell, 2c x 2c blocks (each block contains 2 x 2 cells)
	 * [D] sigma, the Gaussian stddev, for the weighted vote
	 * => feature vector dimension = (2*2*binNum) x 1
	 ***/
	bool centered = true;
	int binNum = 8;
	int cellSize = 8;
	float sigma = 8.f;
	assert( !( 360 % binNum ) ); // binNum must divide 360

	if ( argc != 2 ) {
		error( "Usage: ./run \"XX/YY\" (directory path)" );
	}
	strcpy( fileName, argv[ 1 ] );
	pathLen = strlen( fileName );
	/* append '/' if necessary (unix) */
	if ( fileName[ pathLen - 1 ] != '/' ) {
		fileName[ pathLen++ ] = '/';
		fileName[ pathLen ] = '\0';
	}
	sprintf( fileName, "%strain000.bmp", fileName );
	
	tic = clock();
	/* training of each image */
	imgCount = trainEach( fileName, pathLen, centered, binNum, cellSize, sigma );
	toc = clock();
	printf( "HOG: feature extraction of %d images completed.\n", imgCount );
	runningTime( tic, toc );

	return 0;
}