/** File: hough.c
 ** Author: snowm, ryanlei
 ** Creation: 2009/04/01
 ** Modification: 2009/04/03
 ** Description: Hough Transform, the Rho-Theta Version
 **/

#include "hough.h"
#include "image.h"
#define DEBUG 1


#if DEBUG
int main() {
	clock_t tic, toc;
	Matrix img, filt, edge; /* the image, filter */
	int row, col;

	/* only a test */

	imread( "pics/huge.bmp", &img );
	/* manual color2Gray ...囧rz */
	for ( row = 0; row < img.size1; row++ ) {
		for ( col = 0; col < img.size2; col++ ) {
			img.data[ 0 ][ row ][ col ] = ( img.data[ 0 ][ row ][ col ]
				+ img.data[ 1 ][ row ][ col ] + img.data[ 2 ][ row ][ col ]
				) / 3.f;
		}
	}
	ones( &filt, 5, 5, 1 );
	s_mul( &filt, 1/25.f );
	tic = clock();
	cross( &img, &filt, &edge );
	dump( &edge, "edge(gray) ", RR, 300, 319, 300, 319, INT );

	
	toc = clock();
	runningTime( tic, toc );

	return 0;
}
#endif
