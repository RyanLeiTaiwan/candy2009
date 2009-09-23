/** File: car-train.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/23
 ** Description: The ACTUAL feature extraction of a block
    Features:
	  1. Rectangle features -- Integral image, 45'-tilted-version techniques
	  2. Edge Orientation Histograms (EOH) -- Integral histogram
	  3. Edge Density (ED) -- Integral image
 **/
#include "../include/car-train.h"

void extract_block( int Iid, int Bid, float ***POOL, Matrix *ii_norm, Matrix *ii_EOH,
	Matrix *ii_ED, int rowBeg, int colBeg, int rowEnd, int colEnd ) {
	/* define row and col middle, 1st quarter, 3rd quarter points */
	const int rowMid = ( rowBeg + rowEnd ) / 2;
	const int rowQ1 = ( rowBeg + rowMid ) / 2;
	const int rowQ3 = ( rowMid+1 + rowEnd ) / 2;
	const int colMid = ( colBeg + colEnd ) / 2;
	const int colQ1 = ( colBeg + colMid ) / 2;
	const int colQ3 = ( colMid+1 + colEnd ) / 2;
	const int area = ( rowEnd - rowBeg + 1 ) * ( colEnd - colBeg + 1 );
	const int binNum = ii_EOH->size3;
	int k;
	float all; /* for debug */
	float EOH_total;
	float EOH_bin[ binNum ];
	/* [1.1] 5 rectangle features, normal version */
	POOL[ Iid ][ Bid ][ REC_BEGIN ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowMid, colEnd ) -
		recSum( ii_norm, 0, rowMid+1, colBeg, rowEnd, colEnd );
#if 0
	printf( "image: %d, block: %d, value: %f\n", Iid, Bid, POOL[ Iid ][ Bid ][ REC_BEGIN ] );
#endif
	POOL[ Iid ][ Bid ][ REC_BEGIN + 1 ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowEnd, colMid ) -
		recSum( ii_norm, 0, rowBeg, colMid+1, rowEnd, colEnd );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 2 ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowMid, colMid ) +
		recSum( ii_norm, 0, rowMid+1, colMid+1, rowEnd, colEnd ) -
		recSum( ii_norm, 0, rowBeg, colMid+1, rowMid, colEnd ) -
		recSum( ii_norm, 0, rowMid+1, colBeg, rowEnd, colMid );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 3 ] =
		recSum( ii_norm, 0, rowBeg, colBeg, rowQ1, colEnd ) -
		recSum( ii_norm, 0, rowQ1+1, colBeg, rowQ3, colEnd ) +
		recSum( ii_norm, 0, rowQ3+1, colBeg, rowEnd, colEnd );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 4 ] =
		recSum( ii_norm, 0, rowBeg, colBeg, rowEnd, colQ1 ) -
		recSum( ii_norm, 0, rowBeg, colQ1+1, rowEnd, colQ3 ) +
		recSum( ii_norm, 0, rowBeg, colQ3+1, rowEnd, colEnd );

	/* [1.2] 5 rectangle features, 45'-tilted version (NOT YET implemented) */

	/* [2] 9 EOH features */
	/* compute the bin EOH values and total EOH values */
	EOH_total = 0.f;
	for ( k = 0; k < binNum; k++ ) {
		float sum = recSum( ii_EOH, k, rowBeg, colBeg, rowEnd, colEnd );
		EOH_bin[ k ] = sum;
		EOH_total += sum;
	}
	
	/* record the EOH features */
	all = 0.f; /* for debug */
	for ( k = 0; k < binNum; k++ ) {
		float ratio = EOH_bin[ k ] / ( EOH_total + EPSILON );
		POOL[ Iid ][ Bid ][ EOH_BEGIN + k ] = ratio;
		all += ratio;
	}
#if 0
	printf( "image: %d, block: %d\nb0: %.3f", Iid, Bid, POOL[ Iid ][ Bid ][ EOH_BEGIN ] );
	for ( k = 1; k < binNum; k++ ) {
		printf( ", b%d: %.3f", k, POOL[ Iid ][ Bid ][ EOH_BEGIN + k ] );
	}
	printf( ", all: %.2f\n", all );
#endif

	/* [3] 1 ED feature */
	POOL[ Iid ][ Bid ][ ED_BEGIN ] =
		recSum( ii_ED, 0, rowBeg, colBeg, rowEnd, colEnd ) / (float)area;
}
