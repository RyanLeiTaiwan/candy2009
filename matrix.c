/** File: matrix.c
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/03/22
 ** Description: matrix data structure 
 **/
#include "matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DEBUG 1

/***** matrix basics *****/
/* 顯示矩陣內容( 矩陣、名稱、第三維、INT或DOUBLE )：
 * color: {0,1,2,3} == {R,G,B,ALL}
 * 2D矩陣只有R有值，可以傳R或ALL
 */

void dump( Matrix *source, char *name, COLOR color, TYPE type ) {
	int row, col, layer, layer_start, layer_end;
	/* set layer range */
	switch ( color ) {
		case R: layer_start = 0; layer_end = 0; break;
		case G: layer_start = 1; layer_end = 1; break;
		case B: layer_start = 2; layer_end = 2; break;
		case ALL: layer_start = 0; layer_end = source->size3 - 1 ; break;
		default: error( "dump(): parameter COLOR error." );
	}
	for ( layer = layer_start; layer <= layer_end; layer++ ) {
		printf( "%s(:,:,%d) =\n\n", name, layer );
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				if ( type == INT )
					printf( "%4d", (int) source->data[ layer ][ row ][ col ] );
				else if ( type == DOUBLE )
					printf( "%7.2lf", source->data[ layer ][ row ][ col ] );
				else
					error( "dump(): parameter TYPE error." );
			}
			printf( "\n" );
		}
		printf( "\n" );
	}
	printf( "\n" );
}

void zeros( Matrix *source, int size1, int size2, int size3 ) {
	int row, col, layer;
	/* set dimensions */
	source->size1 = size1;
	source->size2 = size2;
	source->size3 = size3;
	/* memory allocation */
	source->data = (double ***) malloc( size3 * sizeof( double ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (double **) malloc( size1 * sizeof( double *) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (double *) malloc( size2 * sizeof( double ) );
			/* initialization */
			for ( col = 0; col < size2; col++ ) {
				source->data[ layer ][ row ][ col ] = 0.f;
			}
		}
	}
}

void ones( Matrix *source, int size1, int size2, int size3 ) {
	int row, col, layer;
	/* set dimensions */
	source->size1 = size1;
	source->size2 = size2;
	source->size3 = size3;
	/* memory allocation */
	source->data = (double ***) malloc( size3 * sizeof( double ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (double **) malloc( size1 * sizeof( double *) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (double *) malloc( size2 * sizeof( double ) );
			/* initialization */
			for ( col = 0; col < size2; col++ ) {
				source->data[ layer ][ row ][ col ] = 1.f;
			}
		}
	}
}

void eye( Matrix *source, int size ) {
	int row;
	/* set dimensions */
	source->size1 = size;
	source->size2 = size;
	source->size3 = 1;  /* Identity matrix 一律是2D方陣 */
	/* memory allocation */
	source->data = (double ***) malloc( 1 * sizeof( double ** ) );
	source->data[ 0 ] = (double **) malloc( size * sizeof( double * ) );
	for ( row = 0; row < size; row++ ) {
		source->data[ 0 ][ row ] = (double *) malloc( size * sizeof( double ) );
		/* initialization */
		source->data[ 0 ][ row ][ row ] = 1.f;
	}
}

void RAND( Matrix *source, int size1, int size2, int size3, int lower, int upper ) {
	int row, col, layer;
	/* set dimensions */
	source->size1 = size1;
	source->size2 = size2;
	source->size3 = size3;
	/* memory allocation */
	source->data = (double ***) malloc( size3 * sizeof( double ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (double **) malloc( size1 * sizeof( double * ) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (double *) malloc( size2 * sizeof( double ) );
			/* initialization */
			for ( col = 0; col < size2; col++ ) {
				/* random number between [ lower, upper ] */
				source->data[ layer ][ row ][ col ] = 
					(double) ( rand() % (upper-lower+1) + lower );
			}
		}
	}
}

/***** scalar operations *****/
void s_add( Matrix *source, double number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				source->data[ layer ][ row ][ col ] += number;
			}
		}
	}
}

void s_mul( Matrix *source, double number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				source->data[ layer ][ row ][ col ] *= number;
			}
		}
	}
}

void s_pow( Matrix *source, double number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				double value = source->data[ layer ][ row ][ col ];
				source->data[ layer ][ row ][ col ] = pow( value, number );
			}
		}
	}
}

void freeMatrix( Matrix *source ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			free( source->data[ layer ][ row ] );
		}
		free( source->data[ layer ] );
	}
	free( source->data );
}


/***** matrix operations *****/

void m_add( Matrix *source1, Matrix *source2, Matrix *dest ) {
	int row, col, layer;
	int size1 = source1->size1;
	int size2 = source1->size2;
	int size3 = source1->size3;
	/* check dimensions */
	if ( size1 != source2->size1 || size1 != source2->size2 || size3 != source2->size3 ) 
		error( "m_add(): Dimensions disagree." );	
	/* set dimensions */
	dest->size1 = size1;
	dest->size2 = size2;
	dest->size3 = size3;
	/* memory allocation */
	dest->data = (double ***) malloc( size3 * sizeof( double ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		dest->data[ layer ] = (double **) malloc( size1 * sizeof( double * ) );
		for ( row = 0; row < size1; row++ ) {
			dest->data[ layer ][ row ] = malloc( size2 * sizeof( double ) );
			/* addition */
			for ( col = 0; col < size2; col++ ) {
				dest->data[ layer ][ row ][ col ] = 
					source1->data[ layer ][ row ][ col ] + source2->data[ layer ][ row ][ col ];
			}
		}
	}
}

void e_mul( Matrix *source1, Matrix *source2, Matrix *dest ) {
	int row, col, layer;
	int size1 = source1->size1;
	int size2 = source1->size2;
	int size3 = source1->size3;
	/* check dimensions */
	if ( size1 != source2->size1 || size1 != source2->size2 || size3 != source2->size3 ) 
		error( "e_mul(): Dimensions disagree." );	
	/* set dimensions */
	dest->size1 = size1;
	dest->size2 = size2;
	dest->size3 = size3;
	/* memory allocation */
	dest->data = (double ***) malloc( size3 * sizeof( double ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		dest->data[ layer ] = (double **) malloc( size1 * sizeof( double * ) );
		for ( row = 0; row < size1; row++ ) {
			dest->data[ layer ][ row ] = malloc( size2 * sizeof( double ) );
			/* addition */
			for ( col = 0; col < size2; col++ ) {
				dest->data[ layer ][ row ][ col ] = 
					source1->data[ layer ][ row ][ col ] * source2->data[ layer ][ row ][ col ];
			}
		}
	}
}

/* 2D矩陣相乘，目前用最trivial的演算法：O^( n^3 )，
 * 將來有必要時可以combine Strassen's algorithm：O( n^2.807 )
 */
void m_mul( Matrix *source1, Matrix *source2, COLOR color, Matrix *dest ) {
	int count, row, col, layer, layer_start, layer_end;
	int size11 = source1->size1, size21 = source2->size1;
	int size12 = source1->size2, size22 = source2->size2;
	int size13 = source1->size3, size23 = source2->size3;
	/* check dimensions */
	if ( size12 != size21 || size13 != size23 ) 
		error( "m_mul(): Dimensions disagree." );	
	/* set dimensions */
	dest->size1 = size11;
	dest->size2 = size22;

	/* branch on COLOR value */
	if ( color == ALL ) {
		dest->size3 = size13;
		/* memory allocation */
		dest->data = (double ***) malloc( size13 * sizeof( double ** ) );
		for ( layer = 0; layer < size13; layer++ ) {
			dest->data[ layer ] = (double **) malloc( size11 * sizeof( double * ) );
			for ( row = 0; row < size11; row++ ) {
				dest->data[ layer ][ row ] = malloc( size22 * sizeof( double ) );
				for ( col = 0; col < size22; col++ ) {
					/* initialization */
					dest->data[ layer ][ row ][ col ] = 0.f;
					/* multiplication */
					for ( count = 0; count < size12; count++ ) {
						/* the [ layer ][ count ][ col ] part has BAD LOCALITY */
						dest->data[ layer ][ row ][ col ] += 
						source1->data[ layer ][ row ][ count ] * 
						source2->data[ layer ][ count ][ col ];
					}
				}
			}
		}
	}
	else {
		/* only one color component:
		 * now color = {R,G,B} */
		dest->size3 = 1;
		/* memory allocation */
		dest->data = (double ***) malloc( 1 * sizeof( double ** ) );
		dest->data[ 0 ] = (double **) malloc( size11 * sizeof( double * ) );
		for ( row = 0; row < size11; row++ ) {
			dest->data[ 0 ][ row ] = malloc( size22 * sizeof( double ) );
			for ( col = 0; col < size22; col++ ) {
				/* initialization */
				dest->data[ 0 ][ row ][ col ] = 0.f;
				/* multiplication */
				for ( count = 0; count < size12; count++ ) {
						/* the [ layer ][ count ][ col ] part has BAD LOCALITY */
		 				/* now color = {R,G,B} */
						dest->data[ 0 ][ row ][ col ] += 
						source1->data[ color ][ row ][ count ] * 
						source2->data[ color ][ count ][ col ];
				}
			}
		}
	}
}

/* to be added in util.h */
void error( char *msg ) {
	fprintf( stderr, "%s\n", msg );
	exit( EXIT_FAILURE );
}

#if DEBUG
int main() {
	srand( time( NULL ) );
	Matrix A, B, C, D, E, Ra, Rb;
	ones( &A, 10, 10, 1 );
	eye( &B, 6 );
	RAND( &C, 10, 10, 1, 20, 29 );
	dump( &C, "C", R, INT );
	s_pow( &C, 0.5 );
	dump( &C, "(C^0.5)", R, DOUBLE );
	RAND( &Ra, 2, 2, 3, 0, 9 );
	RAND( &Rb, 2, 2, 3, 0, 9 );
	//dump( &Ra, "Ra", ALL, INT );
	//dump( &Rb, "Rb", ALL, INT );
	m_mul( &Ra, &Rb, ALL, &E );
	//dump( &E, "(Ra * Rb)", ALL, INT );

	/* free memory space */
	freeMatrix( &A ); freeMatrix( &B ); freeMatrix( &C ); freeMatrix( &D ); freeMatrix( &E );
	freeMatrix( &Ra ); freeMatrix( &Rb );

}

#endif
