/** File: hough.c
 ** Author: snowm, ryanlei
 ** Creation: 2009/04/01
 ** Modification: 2009/07/04
 ** Description: Hough Transform, the Rho-Theta Version
 **/

#include "image.h"
#define DEBUG 1

/* Hough Transform with several parameters :
 * [1], [2], [3] 代表步驟
 * [A], [B], [C] 代表參數
 */
void hough( Matrix *img, char *nameIn,
	FILTER filterIn, float min_edge, float min_diag ) {
	
	float COS[ 180 ], SIN[ 180 ], COT[ 180 ], CSC[ 180 ];
	Matrix filter, edge, vote;
	//Matrix voteOut;
	int M, N, D, i, x, y, rho, theta;
	float min_vote;
	int lineCount = 0;
	char newName[ 30 ];  /* copied from nameIn */
	char fileName[ 30 ]; /* the final output filename */
	/* copy file name */
	for ( i = 0; nameIn[ i ] != '\0'; i++ ) {
		newName[ i ] = nameIn[ i ];
	}
	newName[ i ] = '\0';
	
	/* [0] Pre-processing */
	/* convert into grayscale and get image sizes */
	color2Gray( img );
	M = img->size1;
	N = img->size2;
	D = lroundf( sqrt( M * M + N * N ) );  /* D: diagonal length */
	/* cos, sin, cot, csc tables */
	cos_sin_table( COS, SIN );
	cot_csc_table( COT, CSC, COS, SIN );

	/* [A] filter type :
	 * Gradient: [ 0,0,0; 0,1,-1; 0,0,0 ] 
	 * Sobel: [ -1,0,1; -2,0,2; -1,0,1 ]
	 * Laplacian: [ 0,-1,0; -1,4,-1; 0,-1,0 ] 
	 */
	zeros( &filter, 3, 3, 1 );
	switch ( filterIn ) {
		case Gradient:
			/*** use the function gradient() to speed up ***/
			gradient( img, &edge, horizontal, false );
			break;
		case Sobel:
			filter.data[ 0 ][ 0 ][ 0 ] = -1;
			filter.data[ 0 ][ 0 ][ 2 ] = 1;
			filter.data[ 0 ][ 1 ][ 0 ] = -2;
			filter.data[ 0 ][ 1 ][ 2 ] = 2;
			filter.data[ 0 ][ 2 ][ 0 ] = -1;
			filter.data[ 0 ][ 2 ][ 2 ] = 1;
			break;
		case Laplace:
			filter.data[ 0 ][ 0 ][ 1 ] = -1;
			filter.data[ 0 ][ 1 ][ 0 ] = -1;
			filter.data[ 0 ][ 1 ][ 1 ] = 4;
			filter.data[ 0 ][ 1 ][ 2 ] = -1;
			filter.data[ 0 ][ 2 ][ 1 ] = -1;
			break;
	}
	
	/* [1] Obtain the "edge image" */
	if ( filterIn != Gradient ) {
		cross( img, &filter, &edge );
	}
	freeMatrix( &filter );
	ABS( &edge );  /* take absolute value */
	map_0_255( &edge );
	sprintf( fileName, "%s_edge.bmp", newName );
	imwrite( fileName, &edge, GRAY );

	/* [2] The voting process */
	/* -D <= rho <= D;  0' <= theta < 180' */
	zeros( &vote, 2 * D + 1, 180, 1 );
    /* formula : rho = x * cos(theta) + y * sin(theta)
	 * right-shift RHO by D to avoid negative indices
	 */
	for ( x = 0; x < M; x++ ) {
		for ( y = 0; y < N; y++ ) {
			/* [B] threshold value for edge image */
			if ( edge.data[ 0 ][ x ][ y ] >= min_edge ) {
				for ( theta = 0; theta < 180; theta++ ) {
					rho = lroundf( x * COS[ theta ] + y * SIN[ theta ] );
					vote.data[ 0 ][ rho + D ][ theta ]++;
				}
			}
		}
	}
	/* OPTIONAL: output the voting result */
	/*
	zeros( &voteOut, 2 * D + 1, 180, 1 );
	full_assign( &vote, &voteOut, ALL, ALL );
	map_0_255( &voteOut );
	sprintf( fileName, "%s_vote.bmp", newName );
	imwrite( fileName, &voteOut, GRAY );
	freeMatrix( &voteOut );
	*/

	/** secretly change values 0 to values 1 **/
	change_0_to_1( &edge );

	/* [3] Plot the lines based on the voted rho's and theta's */
	/* Formula: rho = x * cos(theta) + y * sin(theta)
	 * =>  y = -cot( theta ) + rho * csc( theta );
	 */
	/* [C] threshold value for # of votes recieved */
	min_vote = D * min_diag;
	for ( rho = 0; rho <= 2 * D; rho++ ) {
		/* EXCEPTION: vertical line for which theta is VERY SMALL */
		for ( theta = 0; theta <= 2; theta++ ) {
			if ( vote.data[ 0 ][ rho ][ theta ] >= min_vote ) {
				lineCount++;
				slopeIntPlot( &edge, 100.f, 0, rho - D );
			}
		}
		/* normal theta's */
		for ( theta = 3; theta < 180; theta++ ) {
			if ( vote.data[ 0 ][ rho ][ theta ] >= min_vote ) {
				lineCount++;
				/* the slope and y_intercept */
				float slope = -COT[ theta ];
				float y_int = ( rho - D ) * CSC[ theta ]; /* left-shift back by D */
				slopeIntPlot( &edge, slope, y_int, rho - D );
			}
		}
	}
	printf( "hough(): Line count = %d.\n", lineCount );
	sprintf( fileName, "%s_hough.bmp", newName );
	imwrite( fileName, &edge, RED );  /* use the RED write mode !! */

	freeMatrix( &edge );
	freeMatrix( &vote );
}

#if DEBUG
int main() {
	clock_t tic, toc;
	Matrix img; /* original image */
	/* test case begins */
	imread( "pics/hough/line.bmp", &img );
	tic = clock();
	hough( &img, "pics/output/line", Laplace, 30.f, 0.5 );
	toc = clock();
	printf( "line.bmp: " );
	runningTime( tic, toc );
	/* next test case */
	imread( "pics/hough/paint.bmp", &img );
	tic = clock();
	hough( &img, "pics/output/paint", Sobel, 30.f, 0.2 );
	toc = clock();
	printf( "paint.bmp: " );
	runningTime( tic, toc );
	/* next test case */
	imread( "pics/cars/car00.bmp", &img );
	tic = clock();
	hough( &img, "pics/output/car", Gradient, 20.f, 0.28 );
	toc = clock();
	printf( "car.bmp: " );
	runningTime( tic, toc );
	/* next test case */
	imread( "pics/road1.bmp", &img );
	tic = clock();
	hough( &img, "pics/output/road1", Gradient, 20.f, 0.3 );
	toc = clock();
	printf( "road1.bmp: " );
	runningTime( tic, toc );
	/* next test case */
	imread( "pics/road2.bmp", &img );
	tic = clock();
	hough( &img, "pics/output/road2", Sobel, 18, 0.4 );
	toc = clock();
	printf( "road2.bmp: " );
	runningTime( tic, toc );

	return 0;
}
#endif
