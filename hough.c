/** File: hough.c
 ** Author: snowm, ryanlei
 ** Creation: 2009/04/01
 ** Modification: 2009/04/30
 ** Description: Hough Transform, the Rho-Theta Version
 **/

#include "hough.h"
#define DEBUG 1

void hough( Matrix *img, char *nameIn,
	FILTER filterIn, float min_edge, float min_diag ) {
	
	float COS[ 180 ], SIN[ 180 ], COT[ 180 ], CSC[ 180 ];
	Matrix filter, edge, vote;
	int M, N, D, i, x, y;
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
	/* secretly change values 0 to values 1 */
	change_0_to_1( img );
	/* cos, sin, cot, csc tables */
	cos_sin_table( COS, SIN );
	cot_csc_table( COT, CSC, COS, SIN );

	/* filter type :
	 * Gradient: [ 0,0,0; 0,1,-1; 0,0,0 ] 
	 * Sobel: [ -1,0,1; -2,0,2; -1,0,1 ]
	 * Laplacian: [ 0,-1,0; -1,4,-1; 0,-1,0 ] 
	 */
	zeros( &filter, 3, 3, 1 );
	switch ( filterIn ) {
		case Gradient:
			filter.data[ 0 ][ 1 ][ 1 ] = 1;
			filter.data[ 0 ][ 1 ][ 2 ] = -1;
			break;
		case Sobel:
			break;
			filter.data[ 0 ][ 0 ][ 0 ] = -1;
			filter.data[ 0 ][ 0 ][ 2 ] = 1;
			filter.data[ 0 ][ 1 ][ 0 ] = -2;
			filter.data[ 0 ][ 1 ][ 2 ] = 2;
			filter.data[ 0 ][ 2 ][ 0 ] = -1;
			filter.data[ 0 ][ 2 ][ 2 ] = 1;
		case Laplace:
			filter.data[ 0 ][ 0 ][ 1 ] = -1;
			filter.data[ 0 ][ 1 ][ 0 ] = -1;
			filter.data[ 0 ][ 1 ][ 1 ] = 4;
			filter.data[ 0 ][ 1 ][ 2 ] = -1;
			filter.data[ 0 ][ 2 ][ 1 ] = -1;
			break;
	}
	
	/* [1] Obtain the "edge image" */
	cross( img, &filter, &edge );
	ABS( &edge );  /* take absolute value */
	map_0_255( &edge );
	sprintf( fileName, "%s_edge.bmp", newName );
	printf( "fileName: %s\n", fileName );
	imwrite( fileName, &edge, GRAY );

	/* [2] The voting process */
	/* -D <= rho <= D;  0' <= theta < 180' */
	zeros( &vote, 2 * D + 1, 180 );
    /* formula : rho = x * cos(theta) + y * sin(theta)
	 * right-shift RHO by D to avoid negative indices
	 */





}

#if DEBUG
int main() {
	clock_t tic, toc;
	Matrix img; /* original image */
	/* read the image */
	imread( "pics/hough/line.bmp", &img );

	/* hough transform */
	tic = clock();
	hough( &img, "pics/output/line", Laplace, 30.f, 0.5 );




	toc = clock();
	runningTime( tic, toc );

	return 0;
}
#endif
