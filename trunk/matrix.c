/** File: matrix.c
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/04/01
 ** Description: matrix data structure
 **/
#include "matrix.h"
#define DEBUG 0

/***** matrix basics *****/
/* 顯示矩陣內容( 矩陣、名稱、第三維、INT或DOUBLE )：
 * color: {0,1,2,3} == {RR,GG,BB,ALL}
 * 2D矩陣只有R有值，可以傳R或ALL
 */
void dump( Matrix *source, char *name, COLOR color, int rowBegin, int rowEnd, int colBegin, int colEnd, TYPE type ) {
	int row, col, layer, layer_start, layer_end;
	/* set layer range */
	switch ( color ) {
		case RR: layer_start = 0; layer_end = 0; break;
		case GG: layer_start = 1; layer_end = 1; break;
		case BB: layer_start = 2; layer_end = 2; break;
		case ALL: layer_start = 0; layer_end = source->size3 - 1 ; break;
		default: error( "dump(): Parameter COLOR error." );
	}
	/* check row, col range */
	if ( rowBegin < 0 || rowEnd >= source->size1 || colBegin < 0 || colEnd >= source->size2 ) {
		error( "dump(): Parameters row or col out of range." );
	}
	/* check fake layers */
	if ( source->size3 == 1 && ( color == GG || color == BB ) ) {
		error( "dump(): source has only one layer." );
	}


	for ( layer = layer_start; layer <= layer_end; layer++ ) {
		printf( "%s( %d:%d, %d:%d, %d ) =\n\n", name, rowBegin, rowEnd, colBegin, colEnd, layer );
		for ( row = rowBegin; row <= rowEnd; row++ ) {
			for ( col = colBegin; col <= colEnd; col++ ) {
				if ( type == INT )
					printf( "%4d", (int) source->data[ layer ][ row ][ col ] );
				else if ( type == DOUBLE )
					printf( "%7.2f", source->data[ layer ][ row ][ col ] );
				else
					error( "dump(): Parameter TYPE error." );
			}
			printf( "\n" );
		}
		printf( "\n" );
	}
}

void zeros( Matrix *source, int size1, int size2, int size3 ) {
	int row, col, layer;
	/* set dimensions */
	source->size1 = size1;
	source->size2 = size2;
	source->size3 = size3;
	/* memory allocation */
	source->data = (float ***) malloc( size3 * sizeof( float ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (float **) malloc( size1 * sizeof( float *) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (float *) malloc( size2 * sizeof( float ) );
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
	source->data = (float ***) malloc( size3 * sizeof( float ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (float **) malloc( size1 * sizeof( float *) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (float *) malloc( size2 * sizeof( float ) );
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
	source->data = (float ***) malloc( 1 * sizeof( float ** ) );
	source->data[ 0 ] = (float **) malloc( size * sizeof( float * ) );
	for ( row = 0; row < size; row++ ) {
		source->data[ 0 ][ row ] = (float *) malloc( size * sizeof( float ) );
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
	source->data = (float ***) malloc( size3 * sizeof( float ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		source->data[ layer ] = (float **) malloc( size1 * sizeof( float * ) );
		for ( row = 0; row < size1; row++ ) {
			source->data[ layer ][ row ] = (float *) malloc( size2 * sizeof( float ) );
			/* initialization */
			for ( col = 0; col < size2; col++ ) {
				/* random number between [ lower, upper ] */
				source->data[ layer ][ row ][ col ] =
					(float) ( rand() % (upper-lower+1) + lower );
			}
		}
	}
}

/***** scalar operations *****/
void s_add( Matrix *source, float number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				source->data[ layer ][ row ][ col ] += number;
			}
		}
	}
}

void s_mul( Matrix *source, float number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				source->data[ layer ][ row ][ col ] *= number;
			}
		}
	}
}

void s_pow( Matrix *source, float number ) {
	int row, col, layer;
	for ( layer = 0; layer < source->size3; layer++ ) {
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				float value = source->data[ layer ][ row ][ col ];
				source->data[ layer ][ row ][ col ] = pow( value, number );
			}
		}
	}
}

void freeMatrix( Matrix *source ) {
	int row, layer;
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
	dest->data = (float ***) malloc( size3 * sizeof( float ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		dest->data[ layer ] = (float **) malloc( size1 * sizeof( float * ) );
		for ( row = 0; row < size1; row++ ) {
			dest->data[ layer ][ row ] = malloc( size2 * sizeof( float ) );
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
	dest->data = (float ***) malloc( size3 * sizeof( float ** ) );
	for ( layer = 0; layer < size3; layer++ ) {
		dest->data[ layer ] = (float **) malloc( size1 * sizeof( float * ) );
		for ( row = 0; row < size1; row++ ) {
			dest->data[ layer ][ row ] = malloc( size2 * sizeof( float ) );
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
void m_mul( Matrix *source1, Matrix *source2, COLOR color1, COLOR color2, Matrix *dest ) {
	int count, row, col, layer;
	int size11 = source1->size1, size21 = source2->size1;
	int size12 = source1->size2, size22 = source2->size2;
	int size13 = source1->size3, size23 = source2->size3;
	int dim1 = 1, dim2 = 1;  // 兩矩陣真正的layer數必須相等，先假設是1
	/* check dimensions */
	if ( size13 == 1 && ( color1 == GG || color1 == BB ) )
		error( "m_mul(): source1 has only one layer." );
	if ( size23 == 1 && ( color2 == GG || color2 == BB ) )
		error( "m_mul(): source2 has only one layer." );
	if ( size12 != size21 )
		error( "m_mul(): Demensions disagree." );
	if ( color1 == ALL ) dim1 = size13;
	if ( color2 == ALL ) dim2 = size23;
	if ( dim1 != dim2 )
		error( "m_mul(): Layers disagree." );
	/* set dimensions */
	dest->size1 = size11;
	dest->size2 = size22;
	dest->size3 = dim1;
	/* memory allocation */
	dest->data = (float ***) malloc( dim1 * sizeof( float ** ) );
	for ( layer = 0; layer < dim1; layer++ ) {
		dest->data[ layer ] = (float **) malloc( size11 * sizeof( float * ) );
		for ( row = 0; row < size11; row++ ) {
			dest->data[ layer ][ row ] = malloc( size22 * sizeof( float ) );
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

void full_assign( Matrix *source, Matrix *dest, COLOR sColor, COLOR dColor ) {
	int row, col, layer;
	/* find the actual number of layers */
	int sLayer = 1, dLayer = 1;
	if ( sColor == ALL ) sLayer = source->size3;
	if ( dColor == ALL ) dLayer = dest->size3;

	/* check dimensions */
	if ( source->size3 == 1 && ( sColor == GG || sColor == BB ) )
		error( "full_assign(): source has only one layer." );
	if ( dest->size3 == 1 && ( dColor == GG || dColor == BB ) )
		error( "full_assign(): dest has only one layer." );
	if ( source->size1 != dest->size1 || source->size2 != dest->size2 ||
		sLayer != dLayer ) {
		error( "full_assign(): Dimensions disagree." );
	}

	for ( layer = 0; layer < dLayer; layer++ ) {
		for ( row = 0; row < dest->size1; row++ ) {
			for ( col = 0; col < dest->size2; col++ ) {
				dest->data[ layer ][ row ][ col ] = source->data[ layer ][ row ][ col ];
			}
		}
	}
}

/* 指定row, col, layer範圍的matrix assignment */
void part_assign( Matrix *source, Matrix *dest, 
    int sRowBegin, int sRowEnd, int sColBegin, int sColEnd, int sLayerBegin, int sLayerEnd,
	int dRowBegin, int dRowEnd, int dColBegin, int dColEnd, int dLayerBegin, int dLayerEnd ) {

	int sRow, sCol, sLayer, dRow, dCol, dLayer;
	/* check dimensions */
	if ( sRowEnd - sRowBegin != dRowEnd - dRowBegin ||
		sColEnd - sColBegin != dColEnd - dColBegin ||
		sLayerEnd - sLayerBegin != dLayerEnd - dLayerBegin ) {
		error( "part_assign(): Dimensions disagree." );
	}

	for ( sLayer = sLayerBegin, dLayer = dLayerBegin; sLayer <= sLayerEnd; sLayer++, dLayer++ ) {
		for ( sRow = sRowBegin, dRow = dRowBegin; sRow <= sRowEnd; sRow++, dRow++ ) {
			for ( sCol = sColBegin, dCol = dColBegin; sCol <= sColEnd; sCol++, dCol++ ) {
				dest->data[ dLayer ][ dRow ][ dCol ] = source->data[ sLayer ][ sRow ][ sCol ];
			}
		}
	}
}


#if DEBUG
int main() {
	Matrix A, B, C, D, E, I, Ra, Rb;
	clock_t tic, toc;
	srand( time( NULL ) );

	tic = clock();
	ones( &A, 3, 3, 3 );
	dump( &A, "A", BB, 0, A.size1-1, 0, A.size2-1, INT );
	RAND( &Ra, 5, 5, 1, 20, 29 );
	dump( &Ra, "Ra", ALL, 0, Ra.size1-1, 0, Ra.size2-1, INT );
	s_add( &Ra, -20.f );
	s_mul( &Ra, 5 );
	s_pow( &Ra, 0.5 );
	dump( &Ra, "((Ra-20)*5)^0.5, ", ALL, 0, Ra.size1-1, 0, Ra.size2-1, DOUBLE );

	eye( &I, 3 );
	m_mul( &A, &I, GG, RR, &B );
	dump( &B, "A(:,:,1)*I, ", ALL, 0, B.size1-1, 0, B.size2-1, INT );

	RAND( &Rb, 3, 3, 3, 0, 9 );
	dump( &Rb, "Rb ", ALL, 0, Rb.size1-1, 0, Rb.size2-1, INT );
	m_mul( &Rb, &A, ALL, ALL, &C );
	dump( &C, "Rb * A ", ALL, 0, C.size1-1, 0, C.size2-1, INT );

	zeros( &D, 3, 3, 3 );
	full_assign( &C, &D, ALL, ALL );
	dump( &D, "C = Rb * A ", GG, 0, D.size1-1, 0, D.size2-1, INT );
	ones( &E, 15, 15, 1 );
	part_assign( &Ra, &E, 0, 4, 0, 4, 0, 0,  8, 12, 8, 12, 0, 0 );
	dump( &E, "E(part_assigned from Ra) ", ALL, 0, E.size1-1, 0, E.size2-1, INT );

	toc = clock();
	runningTime( tic, toc );
	/* free memory space */
	freeMatrix( &A ); freeMatrix( &B ); freeMatrix( &C ); freeMatrix( &D ); freeMatrix( &E );
	freeMatrix( &I ); freeMatrix( &Ra ); freeMatrix( &Rb );

	return 0;
}

#endif
