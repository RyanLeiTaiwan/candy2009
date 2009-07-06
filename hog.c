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

/* enum: normalization scheme */
typedef enum { L2_norm, L2_Hys, L1_norm, L1_sqrt } NORMALIZE;
/*** HOG Parameters:
 * [A] centered/uncentered gradients
 * [B] # of orientation bins
 * [C] c x c cell
 * [D] b x b blocks, b must be a multiple of c. Let b = r * c
 *     => feature vector dimension = (blockNum * r * r * binNum) x 1
 *     The blocks are overlapping and blockNum is computed in HOG.
 * [E] sigma, the Gaussian stddev, for the weighted vote
 * [F] normalization scheme: L2-norm
 * [G] epsilon: a very small constant in normalization (to avoid div-by-0)
***/
void HOG( Matrix *img, char *outFileName, 
	bool centered, int binNum, int cellSize, int blockSize, float sigma, NORMALIZE scheme, float epsilon ) {

	Matrix RGBimg[ 3 ]; /* array of 2D matrices, each being the RGB components of img */
	/* Hor: horizontal, Ver: vertical, Mag: magnitude, Bin: orientation bin */
	Matrix gradHor[ 3 ], gradVer[ 3 ], gradMag, gradBin;
	Matrix Gauss, tempMag, tempVote, tempBin, tempBlock;
	int M = img->size1, N = img->size2;
	int i, j, row, col, Hcount, Vcount, HH, VV, cRow, cCol;
	int BCratio = blockSize / cellSize; /* the "r" in comment [D] */
	/* number of cells in horizontal and vertical directions:
	 * 不整除的部份忽略不做，多做的feature也不見得好 
	 */
	int cellV = M / cellSize, cellH = N / cellSize;
	/* number of blocks in horizontal and vertical directions */
	int blockV = cellV - BCratio + 1; /* overlapping blocks */
	int blockH = cellH - BCratio + 1; /* overlapping blocks */
	int blockNum = blockV * blockH;

	/*** [0] Pre-processing ***/
	/* Generate Gaussian filter */
	Gaussian( &Gauss, cellSize, sigma );

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
	zeros( &tempMag, cellSize, cellSize, 1 );
	zeros( &tempBin, cellSize, cellSize, 1 );
	zeros( &tempBlock, BCratio * BCratio, binNum, 1 );
	i = 0; // cell count
	for ( row = 0, Vcount = 0; Vcount < blockV; row += cellSize, Vcount++ ) {
		for ( col = 0, Hcount = 0; Hcount < blockH; col += cellSize, Hcount++ ) {
			/* for each (overlapping) block of BCratio x BCratio cell array: */
			j = 0; // cell count within a block
			for ( VV = 0; VV < BCratio; VV++ ) {
				for ( HH = 0; HH < BCratio; HH++ ) {
					/* for each cell in the block: */
					int rowBeg = row + VV * cellSize;
					int rowEnd = rowBeg + cellSize - 1;
					int colBeg = col + HH * cellSize;
					int colEnd = colBeg + cellSize - 1;

					/* extract a cell from gradMag, gradBin */
					part_assign( &gradMag, &tempMag, rowBeg, rowEnd, colBeg, colEnd, 0, 0,
						0, cellSize - 1, 0, cellSize - 1, 0, 0 );
					part_assign( &gradBin, &tempBin, rowBeg, rowEnd, colBeg, colEnd, 0, 0,
						0, cellSize - 1, 0, cellSize - 1, 0, 0 );
					/* ".*" the Gaussian weight */
					e_mul( &tempMag, &Gauss, &tempVote );
					/* count the vote for corresponding bin and record it */
					for ( cRow = 0; cRow < cellSize; cRow++ ) {
						for ( cCol = 0; cCol < cellSize; cCol++ ) {
							/* for each pixel in the cell */
							int bin = tempBin.data[ 0 ][ cRow ][ cCol ];
							tempBlock.data[ 0 ][ j ][ bin ] += tempVote.data[ 0 ][ cRow ][ cCol ];
						}
					}

					j++; i++;	
					/* free memory space that was alloated here */
					freeMatrix( &tempVote );
				}
			}

			full_dump( &tempBlock, "tempBlock", ALL, FLOAT );
			/* Still in the "for each block" loop */
			/*** [3] Contrast normalize over overlapping spatial blocks ***/


			/*** [4] Collect HOG's over detection window ***/

			/* clear tempBlock (restart from 0) */
			clear( &tempBlock );
		}
	}
	printf( "blockNum = %d, cellNum = %d\n", blockNum, i );
	assert( i == blockNum * BCratio * BCratio );
	

	/* free memory space */
	for ( i = 0; i < 3; i++ ) {
		freeMatrix( &RGBimg[ i ] );
		freeMatrix( &gradHor[ i ] );
		freeMatrix( &gradVer[ i ] );
	}
	freeMatrix( &gradMag ); freeMatrix( &gradBin ); freeMatrix( &tempMag ); freeMatrix( &tempBin );
	freeMatrix( &Gauss ); freeMatrix( &tempBlock );
}

int trainEach( char *fileName, int pathLen, 
	bool centered, int binNum, int cellSize, int blockSize, float sigma, NORMALIZE scheme, float epsilon ) {

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
					HOG( &img, "pics/output/HOG_train.txt", 
						centered, binNum, cellSize, blockSize, sigma, scheme, epsilon );
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
	 * [C] c x c cell
	 * [D] b x b blocks, b must be a multiple of c. Let b = r * c
	 *     => feature vector dimension = (blockNum * r * r * binNum) x 1
	 *     The blocks are overlapping and blockNum is computed in HOG.
	 * [E] sigma, the Gaussian stddev, for the weighted vote
	 * [F] normalization scheme: L2-norm
	 * [G] epsilon: a very small constant in normalization (to avoid div-by-0)
	 ***/
	bool centered = true;
	int binNum = 8;
	int cellSize = 8;
	int blockSize = 16;
	float sigma = 8.f;
	float epsilon = 1e-6;
	assert( !( 360 % binNum ) ); // binNum must divide 360
	assert( !( blockSize % cellSize ) ); // cellSize must divide blockSize

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
	imgCount = trainEach( fileName, pathLen, 
		centered, binNum, cellSize, blockSize, sigma, L2_norm, epsilon );
	toc = clock();
	printf( "HOG: feature extraction of %d images completed.\n", imgCount );
	runningTime( tic, toc );

	return 0;
}
