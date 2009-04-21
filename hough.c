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
	Matrix img, filt, edge, vote; /* the image, filter */
	int row, col;
	int dmax, min_vote;
    int min_edge = 30;
    int vote_ratio = 0.75;
    float COS[ 180 ], SIN[ 180 ];
    int THETA, RHO;

    cos_sin_table( COS, SIN );

	/* only a test */

	imread( "pics/hough/line.bmp", &img );
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

	//dump( &edge, "edge(gray) ", RR, 300, 319, 300, 319, INT );

    dmax=floorf(sqrt(((img.size1)^2)+((img.size2)^2)));
    min_vote = dmax * 0.5;

    zeros(&vote,2*dmax+1,180,1);  //voting matrix
    for( row=0; row< edge.size1; row++){
        for( col=0; col< edge.size2; col++){
            if( edge.data[col][row][1]>= min_edge){
                for( THETA=1; THETA<=180; THETA++){/////////////need modified!
                    RHO= floorf(row* COS(THETA) + col* SIN(THETA)+ (dmax + 1));
                    vote.data[RHO][THETA][1]= vote.data[RHO][THETA][1]+1;
                }
            }
        }
    }

	toc = clock();
	runningTime( tic, toc );

	return 0;
}
#endif
