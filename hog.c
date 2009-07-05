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
#define DEBUG 1

/* [1], [2], [3], 代表步驟
 * [A], [B], [C], 代表參數 */
void HOG( Matrix *img, char *outFileName ) {
	Matrix RGBimg[ 3 ]; /* array of 2D matrices, each being the RGB components of img */
	/* Hor: horizontal, Ver: vertical, Mag: magnitude, Ang: angle */
	Matrix gradHor[ 3 ], gradVer[ 3 ], gradMag[ 3 ], gradAng[ 3 ], gradFinal;
	int M = img->size1, N = img->size2;
	int i, row, col;

	/*** [0] Pre-processing ***/
	

	/*** [1] Compute gradients ***/
	/* Separate img into RGBimg[ 0~2 ].
	 * Calculate gradients for RGBimg[ 0~2 ] */
	for ( i = 0; i < 3; i++ ) { 
		zeros( &RGBimg[ i ], M, N, 1 );
		full_assign( img, &RGBimg[ i ], (COLOR)i, ALL );
		gradient( &RGBimg[ i ], &gradHor[ i ], horizontal, true );
		gradient( &RGBimg[ i ], &gradVer[ i ], vertical, true );
		for ( row = 0; row < M; row++ ) {
			for ( col = 0; col < N; col++ ) {
				/* Compute the magnitude of the gradient and compare with the largest.
				 * if it IS the largest, also compute the angle of the gradient*/

			}
		}
	}
	
}

#if DEBUG
int main( int argc, char *argv[] ) {
	/* Usage: ./run "XX/YY",
	 * then the program will train using all pictures in directory
	 *   XX/YY
	 * image file format: trainDDD.bmp, D from [0,9]
	 * must start from train000.bmp and have no gap in the numbers
	 */
	Matrix img;
	char D1, D2, D3; /* each from '0' ~ '9' */
	char fileName[ 60 ];
	int pathLen, imgCount = 0;
	clock_t tic, toc;
	FILE *ftest;

	if ( argc != 2 ) {
		error( "Usage: ./run \"XX/YY\" (directory path)" );
	}
	strcpy( fileName, argv[ 1 ] );
	pathLen = strlen( fileName );
	/* append '/' if necessary */
	if ( fileName[ pathLen - 1 ] != '/' ) {
		fileName[ pathLen++ ] = '/';
		fileName[ pathLen ] = '\0';
	}
	sprintf( fileName, "%strain000.bmp", fileName );
	
	tic = clock();
	/* for each image: */
	for ( D1 = '0'; D1 <= '0'; D1++ ) {
		for ( D2 = '0'; D2 <= '0'; D2++ ) {
			for ( D3 = '0'; D3 <= '0'; D3++ ) {
				fileName[ pathLen + 5 ] = D1;
				fileName[ pathLen + 6 ] = D2;
				fileName[ pathLen + 7 ] = D3;
				/* termination condition */
				if ( !( ftest = fopen( fileName, "r" ) ) ) {
					break;
				}
				else {
					imgCount++;
					fclose( ftest );
					/* imread(), HOG(), freeMatrix() */
					imread( fileName, &img );
					HOG( &img, "pics/output/HOG_train.txt" );
					freeMatrix( &img );
				}
			}
		}
	}

	toc = clock();
	printf( "HOG: training of %d images completed.\n", imgCount );
	runningTime( tic, toc );

	return 0;
}
#endif
