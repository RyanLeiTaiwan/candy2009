/** File: util.h
 ** Author: iopic, ryanlei
 ** Creation : 2009/03/23
 ** Modification: 2009/03/29
 ** Description: utility functions
 **/
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define DEBUG 0

void error( char *msg ) {
    fprintf( stderr, "%s\n", msg );
	    exit( EXIT_FAILURE );
}

void cos_sin_table( float COS[], float SIN[] ) {	
   
	int i; 
	float r = M_PI/180;
   
	for( i=0; i<180; i++ ){
    	SIN[i] = sin( r*i );
      	COS[i] = cos( r*i );      
	}  
}

#if DEBUG
int main() {
    /* generate cos, sin table */
    float COS[ 180 ], SIN[ 180 ];
    cos_sin_table( COS, SIN );

    printf( "cos( 0 ) = %f\n", COS[ 0 ] );
    printf( "sin( 30 ) = %f\n", SIN[ 30 ] );
    printf( "sin( 90 ) = %f\n", SIN[ 90 ] );
    printf( "cos( 120 ) = %f\n", COS[ 120 ] );

    return 0;
}
#endif
