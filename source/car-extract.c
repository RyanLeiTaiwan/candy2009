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

void extract_block( int Iid, int Bid, Feature ***POOL, Matrix *img, Matrix *ii, 
	int rowBeg, int colBeg, int rowEnd, int colEnd ) {
	/* [1.1] 5 rectangle features, normal version */
	(*POOL)[ Iid ][ Bid ].REC[ 0 ] = recSum( ii, rowBeg, colBeg, (rowBeg+rowEnd)/2, colEnd );
		- recSum( ii, (rowBeg+rowEnd)/2 + 1, colBeg, rowEnd, colEnd );
	printf( "image: %d, block: %d, REC[ 0 ]: %f\n", Iid, Bid, (*POOL)[ Iid ][ Bid ].REC[ 0 ] );
	
	/* [1.2] 5 ectangle features, 45'-tilted version (NOT YET implemented) */
	/* [2] 9 EOH features */
	/* [3] 1 ED feature */

}
