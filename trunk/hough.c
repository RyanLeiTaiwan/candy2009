/** File: hough.c
 ** Author: snowm, ryanlei
 ** Creation: 2009/04/01
 ** Modification: 2009/04/23
 ** Description: Hough Transform, the Rho-Theta Version
 **/

#include "hough.h"
#include "image.h"
#define DEBUG 0


#if DEBUG
int main() {
	clock_t tic, toc;
	Matrix img, filt, edge, vote, Sob,Lap; /* the image, filter */
	int row, col;
	int dmax, min_vote;
    int min_edge = 30;
    //float vote_ratio = 0.75;
    float COS[ 180 ], SIN[ 180 ];
   // int Laplacian [3][3] = {0,-1,0, -1,4,-1, 0,-1,0};
    int THETA, RHO;

    cos_sin_table( COS, SIN );

	/* only a test */

	imread( "pics/hough/line.bmp", &img );

	color2Gray( &img );  /* ryanlei: 寫好color2Gray當然要用阿 */

	ones( &filt, 5, 5, 1 );
    s_mul( &filt, 1/25.f );
    zeros( &Sob, 3, 3, 1 );
    zeros( &Lap, 3, 3, 1 );
    /*sobel*/
    Sob.data[0][0][0]=-1;
    Sob.data[0][0][2]=1;
    Sob.data[0][1][0]=-2;
    Sob.data[0][1][2]=2;
    Sob.data[0][2][0]=-1;
    Sob.data[0][2][2]=1;
    /*gradient*/
    Lap.data[0][0][1]=-1;
    Lap.data[0][1][0]=-1;
    Lap.data[0][1][1]=4;
    Lap.data[0][1][2]=-1;
    Lap.data[0][2][1]=-1;

	dump(&Lap,"n",RR,0,2,0,2,INT);
	tic = clock();
	//cross( &img, &Sob, &edge );
    cross( &img, &Lap, &edge );
    //dump(&edge, "eg", RR, 0, edge.size1-1, 0, edge.size2-1,INT);
	//dump( &edge, "edge(gray) ", RR, 300, 319, 300, 319, INT );
    imwrite("lapedge.bmp", &edge,GRAY);
    dmax=floorf(sqrt(((img.size1)^2)+((img.size2)^2)));
    min_vote = dmax * 0.5;
    printf("dma=%d",dmax);
    zeros(&vote,2*dmax+3,180,1);  //voting matrix

    for( row=0; row< edge.size1; row++){
        for( col=0; col< edge.size2; col++){
            //printf("t:%d ",edge.data[0][row][col]);
            if( edge.data[0][row][col]>= min_edge){

                for( THETA=0; THETA<180; THETA++){

                    RHO= floorf(row* COS[THETA] + col* SIN[THETA] + dmax+1);
                    //printf("ro=%d ",RHO);
                    /*for(ang=0;ang<RHO;ang++){/////////////need modified!
                        vote.data[0][ang][THETA]= vote.data[0][ang][THETA]+1;
                    }*/
                }
            }
        }
    }

	toc = clock();
	runningTime( tic, toc );

	return 0;
}
#endif
