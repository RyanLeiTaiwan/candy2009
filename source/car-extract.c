/** File: car-train.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/16
 ** Description: The ACTUAL feature extraction of a block
    Features:
	  1. Rectangle features -- Integral image, 45'-tilted-version techniques
	  2. Edge Orientation Histograms (EOH) -- Integral histogram
	  3. Edge Density (ED) -- Integral image
 **/
#include "../include/car-train.h"

void extract_block( int Iid, int Bid, Feature ***POOL, Matrix *img_norm, Matrix *ii_norm, 
	int rowBeg, int colBeg, int rowEnd, int colEnd ) {
	/* define row and col middle, 1st quarter, 3rd quarter points */
	const int rowMid = ( rowBeg + rowEnd ) / 2;
	const int rowQ1 = ( rowBeg + rowMid ) / 2;
	const int rowQ3 = ( rowMid+1 + rowEnd ) / 2;
	const int colMid = ( colBeg + colEnd ) / 2;
	const int colQ1 = ( colBeg + colMid ) / 2;
	const int colQ3 = ( colMid+1 + colEnd ) / 2;
	/* [1.1] 5 rectangle features, normal version */
	(*POOL)[ Iid ][ Bid ].REC[ 0 ] = 
		recSum( ii_norm, rowBeg, colBeg, rowMid, colEnd ) -
		recSum( ii_norm, rowMid+1, colBeg, rowEnd, colEnd );
#if 0
	printf( "image: %d, block: %d, value: %f\n", Iid, Bid, (*POOL)[ Iid ][ Bid ].REC[ 0 ] );
#endif
	(*POOL)[ Iid ][ Bid ].REC[ 1 ] = 
		recSum( ii_norm, rowBeg, colBeg, rowEnd, colMid ) -
		recSum( ii_norm, rowBeg, colMid+1, rowEnd, colEnd );
	(*POOL)[ Iid ][ Bid ].REC[ 2 ] = 
		recSum( ii_norm, rowBeg, colBeg, rowMid, colMid ) +
		recSum( ii_norm, rowMid+1, colMid+1, rowEnd, colEnd ) -
		recSum( ii_norm, rowBeg, colMid+1, rowMid, colEnd ) -
		recSum( ii_norm, rowMid+1, colBeg, rowEnd, colMid );
	(*POOL)[ Iid ][ Bid ].REC[ 3 ] =
		recSum( ii_norm, rowBeg, colBeg, rowQ1, colEnd ) -
		recSum( ii_norm, rowQ1+1, colBeg, rowQ3, colEnd ) +
		recSum( ii_norm, rowQ3+1, colBeg, rowEnd, colEnd );
	(*POOL)[ Iid ][ Bid ].REC[ 4 ] =
		recSum( ii_norm, rowBeg, colBeg, rowEnd, colQ1 ) -
		recSum( ii_norm, rowBeg, colQ1+1, rowEnd, colQ3 ) +
		recSum( ii_norm, rowBeg, colQ3+1, rowEnd, colEnd );

	/* [1.2] 5 rectangle features, 45'-tilted version (NOT YET implemented) */

	/* [2] 9 EOH features */

	/* [3] 1 ED feature */

}
