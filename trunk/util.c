/** File: util.h
 ** Author: iopic, ryanlei
 ** Creation : 2009/03/23
 ** Modification: 2009/07/05
 ** Description: utility functions
 **/
#include "util.h"

#define DEBUG 0

void error( char *msg ) {
    fprintf( stderr, "%s\n", msg );
	    exit( EXIT_FAILURE );
}

void runningTime( clock_t tic, clock_t toc ) {
	printf( "Running Time: %f seconds.\n", 
		(double) ( toc - tic ) / (double) CLOCKS_PER_SEC );
}

void cos_sin_table( float COS[], float SIN[] ) {
	int i;
	float r = M_PI / 180;
	for ( i = 0; i < 180; i++ ){
    	SIN[ i ] = sin( r * i );
      	COS[ i ] = cos( r * i );
	}
}

void cot_csc_table( float COT[], float CSC[], float COS[], float SIN[] ) {
	int i;
	for ( i = 0; i < 180; i++ ){
    	COT[ i ] = COS[ i ] / SIN[ i ];
      	CSC[ i ] = 1.f / SIN[ i ];
	}
}

void Gaussian( float ***GAUSS, float sigma, int size ) {
	/* 2D Gaussian filter with 0 mean and sigma stddev:
	 *                      1                x^2 + y^2 
	 * Gauss( x, y ) = ------------ * exp( - --------- )
	 *                 2*pi*sigma^2          2*sigma^2 
	 *
	 * may remove the denominator to make Gauss( 0, 0 ) = 1
	 */
	int x, y;
	float center = ( size - 1 ) / 2.f;
	/* malloc a 2D array and compute Gaussian values */
	*GAUSS = (float **)malloc( sizeof( float * ) * size );
	for ( x = 0; x < size; x++ ) {
		float xVal = x - center;
		(*GAUSS)[ x ] = (float *)malloc( sizeof( float ) * size );
		for ( y = 0; y < size; y++ ) {
			float yVal = y - center;
			(*GAUSS)[ x ][ y ] = exp( -( xVal*xVal + yVal*yVal ) / ( 2.f*sigma*sigma ) );
				//( 2*M_PI*sigma*sigma );
		}
	}
}

#if DEBUG
int main() {
	clock_t tic, toc;
	int i, j;
    /* generate cos, sin, cot, csc table */
    float COS[ 180 ], SIN[ 180 ];
	float COT[ 180 ], CSC[ 180 ];
	float **GAUSS;
	tic = clock();
    cos_sin_table( COS, SIN );
	cot_csc_table( COT, CSC, COS, SIN );
	/* generate 2D Gaussian function */
	Gaussian( &GAUSS, 8.f, 15 );

	toc = clock();
	runningTime( tic, toc );

    printf( "cos( 0 ) = %f\n", COS[ 0 ] );
    printf( "sin( 30 ) = %f\n", SIN[ 30 ] );
    printf( "sin( 90 ) = %f\n", SIN[ 90 ] );
    printf( "cos( 120 ) = %f\n", COS[ 120 ] );
	printf( "cot( 135 ) = %f\n", COT[ 135 ] );
	printf( "csc( 30 ) = %f\n", CSC[ 30 ] );

	printf( "15x15 Gauss( 0, 8 ):\n" );
	for ( i = 0; i < 15; i++ ) {
		for ( j = 0; j < 15; j++ ) {
			printf( "%.3f ", GAUSS[ i ][ j ] );
		}
		printf( "\n" );
	}

    return 0;
}
#endif
