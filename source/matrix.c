/** File: matrix.c
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/09/16
 ** Description: matrix data structure
 **/
#include "../include/matrix.h"
#define DEBUG 0

/* 顯示全部矩陣內容( 矩陣、名稱、第三維、INT或FLOAT )：
 * color: {0,1,2,3} == {RR,GG,BB,ALL}
 * 2D矩陣只有R component有值，可以傳R或ALL
 */
void full_dump( Matrix *source, char *name, COLOR color, TYPE type ) {
	int row, col, layer, layer_start, layer_end;
	/* set layer range */
	switch ( color ) {
		case RR: layer_start = 0; layer_end = 0; break;
		case GG: layer_start = 1; layer_end = 1; break;
		case BB: layer_start = 2; layer_end = 2; break;
		case ALL: layer_start = 0; layer_end = source->size3 - 1 ; break;
		default: error( "dump(): Parameter COLOR error." );
	}
	/* check fake layers */
	if ( source->size3 == 1 && ( color == GG || color == BB ) ) {
		error( "dump(): source has only one layer." );
	}

	for ( layer = layer_start; layer <= layer_end; layer++ ) {
		printf( "%s( :, :, %d ) =\n\n", name, layer );
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				if ( type == INT )
					printf( "%4d", (int) source->data[ layer ][ row ][ col ] );
				else if ( type == FLOAT )
					printf( "%7.3f", source->data[ layer ][ row ][ col ] );
				else
					error( "dump(): Parameter TYPE error." );
			}
			printf( "\n" );
		}
		printf( "\n" );
	}
}

/* 顯示部分矩陣內容：
 * 與full_dump相似，但是row跟col要指定起點、終點
 */
void part_dump( Matrix *source, char *name, COLOR color, int rowBegin, int rowEnd, int colBegin, int colEnd, TYPE type ) {
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
				else if ( type == FLOAT )
					printf( "%7.3f", source->data[ layer ][ row ][ col ] );
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

void clear( Matrix *source ) {
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	int row, col, layer;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
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

void Gaussian( Matrix *dest, int size, float sigma ) {
    /* 2D Gaussian filter with 0 mean and sigma stddev:
     *                      1                x^2 + y^2 
     * Gauss( x, y ) = ------------ * exp( - --------- )
     *                 2*pi*sigma^2          2*sigma^2 
     *
	 * Need to adjust to make the sum = 1
     */
	 int x, y;
	 float center = ( size - 1 ) / 2.f;
	 float sum;
	 zeros( dest, size, size, 1 );

	 for ( x = 0; x < size; x++ ) {
		 float xVal = x - center;
		 for ( y = 0; y < size; y++ ) {
			float yVal = y - center;
			/* skip the denominator, because we'll normalize the matrix, anyway. */
			dest->data[ 0 ][ x ][ y ] = exp( -( xVal*xVal + yVal*yVal ) / ( 2.f*sigma*sigma ) );
		 }
	 }
	 /* adjust to make the sum = 1 */
	 sum = m_sum( dest );
	 s_mul( dest, 1.f / sum );
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

void s_pow( Matrix *source, float power ) {
	int row, col, layer;
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
			for ( col = 0; col < size2; col++ ) {
				float value = source->data[ layer ][ row ][ col ];
				source->data[ layer ][ row ][ col ] = pow( value, power );
			}
		}
	}
}

void s_sqrt( Matrix *source ) {
	int row, col, layer;
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
			for ( col = 0; col < size2; col++ ) {
				float value = source->data[ layer ][ row ][ col ];
				source->data[ layer ][ row ][ col ] = sqrt( value );
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
	int row, col, sLayer, dLayer;
	int sLayerBeg = (int)sColor, sLayerEnd = (int)sColor;
	int dLayerBeg = (int)dColor, dLayerEnd = (int)dColor;
	/* find the actual number of layers, and set beginning, ending layers */
	int sLayerNum = 1, dLayerNum = 1;
	if ( sColor == ALL ) { 
		sLayerNum = source->size3;
		sLayerBeg = 0;
		sLayerEnd = source->size3 - 1;
	}
	if ( dColor == ALL ) {
		dLayerNum = dest->size3;
		dLayerBeg = 0;
		dLayerEnd = dest->size3 - 1;
	}
	/* check dimensions */
	if ( source->size3 == 1 && ( sColor == GG || sColor == BB ) )
		error( "full_assign(): source has only one layer." );
	if ( dest->size3 == 1 && ( dColor == GG || dColor == BB ) )
		error( "full_assign(): dest has only one layer." );
	if ( source->size1 != dest->size1 || source->size2 != dest->size2 ||
		sLayerNum != dLayerNum ) {
		error( "full_assign(): Dimensions disagree." );
	}

	for ( sLayer = sLayerBeg, dLayer = dLayerBeg; sLayer <= sLayerEnd; sLayer++, dLayer++ ) {
		for ( row = 0; row < dest->size1; row++ ) {
			for ( col = 0; col < dest->size2; col++ ) {
				dest->data[ dLayer ][ row ][ col ] = source->data[ sLayer ][ row ][ col ];
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

/* 2D cross-correlation
 * 參考matlab/cross_cor.m :
 * image: M x N; filter: K x L */
void cross( Matrix *image, Matrix *filter, Matrix *dest ) {
    int K = filter->size1;
    int L = filter->size2;
    int M = image->size1;
    int N = image->size2;
    int x, y, u, v;
    int targetX, targetY;

	/* check image and filter dimensions: should be 2D, square, odd */
	if ( image->size3 != 1 ) {
		error( "cross(): Image should be 2D." );
	}
	if ( filter->size3 != 1 ) {
		error( "cross(): Filter should be 2D." );
	}
    if ( K != L || !( K & 1 ) ) {
        error( "cross(): Filter dimension error." );
    }

    zeros( dest, M, N, 1 );

    for ( x = 0; x < M; x++ ){
        for ( y = 0; y < N; y++ ){
            for ( u = 0; u < K; u++ ){
                for ( v = 0; v < K; v++ ){
                    targetX = x + u - ( ( K - 1 ) >> 1 );  /* >> 1 means / 2 */
                    targetY = y + v - ( ( K - 1 ) >> 1 );

                    if ( targetX < 0 ) targetX = -targetX;
                    if ( targetX >= M ) targetX = 2 * M - 2 - targetX;
                    if ( targetY < 0 ) targetY = -targetY;
                    if ( targetY >= N ) targetY = 2 * N - 2 - targetY;

                    dest->data[ 0 ][ x ][ y ] += 
						filter->data[ 0 ][ u ][ v ] * image->data[ 0 ][ targetX ][ targetY ];

                }
            }
        }
    }
}

/* 2D gradient filters: horizontal/vertical; centered/uncentered */
void gradient( Matrix *source, Matrix *dest, DIRECTION dir, bool centered ) {
	int row, col;
	if ( source->size3 != 1 ) {
		error( "gradient(): Image should be 2D." );
	}
    zeros( dest, source->size1, source->size2, 1 );

	/* 分成四種case各別處理 */
	if ( dir == horizontal && !centered ) {
		/* [-1.,1] */
		for ( row = 0; row < source->size1; row++ ) {
			for ( col = 0; col < source->size2 - 1; col++ ) {
				dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row ][ col + 1 ] - source->data[ 0 ][ row ][ col ];
			}
			/* boundary: rightmost */
			dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row ][ col - 1 ] - source->data[ 0 ][ row ][ col ];
		}
	}
	else if ( dir == horizontal && centered ) {
		/* [-1,0,1] */
		for ( row = 0; row < source->size1; row++ ) {
			/* boundary: leftmost */
			dest->data[ 0 ][ row ][ 0 ] = 
					source->data[ 0 ][ row ][ 1 ] - source->data[ 0 ][ row ][ 0 ];
			for ( col = 1; col < source->size2 - 1; col++ ) {
				dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row ][ col + 1 ] - source->data[ 0 ][ row ][ col - 1 ];
			}
			/* boundary: rightmost */
			dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row ][ col ] - source->data[ 0 ][ row ][ col - 1 ];
		}

	}
	else if ( dir == vertical && !centered ) {
		/* [-1.,1]T */
		for ( row = 0; row < source->size1 - 1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row + 1 ][ col ] - source->data[ 0 ][ row ][ col ];
			}
		}
		/* boundary: bottom */
		for ( col = 0; col < source->size2; col++ ) {
			dest->data[ 0 ][ row ][ col ] = 
				source->data[ 0 ][ row - 1 ][ col ] - source->data[ 0 ][ row ][ col ];
		}
	}
	else if ( dir == vertical && centered ) {
		/* [-1,0,1]T */
		/* boundary: top */
		for ( col = 0; col < source->size2; col++ ) {
			dest->data[ 0 ][ 0 ][ col ] = 
				source->data[ 0 ][ 1 ][ col ] - source->data[ 0 ][ 0 ][ col ];
		}
		for ( row = 1; row < source->size1 - 1; row++ ) {
			for ( col = 0; col < source->size2; col++ ) {
				dest->data[ 0 ][ row ][ col ] = 
					source->data[ 0 ][ row + 1 ][ col ] - source->data[ 0 ][ row - 1 ][ col ];
			}
		}
		/* boundary: bottom */
		for ( col = 0; col < source->size2; col++ ) {
			dest->data[ 0 ][ row ][ col ] = 
				source->data[ 0 ][ row ][ col ] - source->data[ 0 ][ row - 1 ][ col ];
		}
	}
	else {
		error( "gradient(): unexpected gradient type." );
	}
}

/* 求2D矩陣的最大值和最小值，只求值不求index
 * 使用Algorithms教的simultaneous max and min
 * Time: O( 1.5mn )
 */
void max_min( Matrix *image, float *maxRet, float *minRet ) {
	/* check image dimensions: should be 2D */
	if ( image->size3 != 1 ) {
		error( "max_min(): Image should be 2D." );
	}
	int total = image->size1 * image->size2;
	int half = total >> 1;
	int colEnd = image->size2 - 1;
	float winners[ half + 1 ], losers[ half + 1 ];
	int lPtr = -1, wPtr = -1;  /* end of array index */
	int row = 0, col = 0;
	int max, min;

	/* [1] divide the image->data into winners, losers arrays */
	while ( half-- ) {
		float a, b;
		a = image->data[ 0 ][ row ][ col ];
		if ( col == colEnd ) {
			row++;
			col = 0;
		}
		else {
			col++;
		}
		b = image->data[ 0 ][ row ][ col ];
		
		if ( a < b ) {
			losers[ ++lPtr ] = a;
			winners[ ++wPtr ] = b;
		}
		else {
			winners[ ++wPtr ] = a;
			losers[ ++lPtr ] = b;
		}

		if ( col == colEnd ) {
			row++;
			col = 0;
		}
		else {
			col++;
		}
	}
	/* total is odd => 種子隊 */
	if ( total & 1 ) {
		float last = image->data[ 0 ][ row ][ col ];
		if ( last > winners[ wPtr ] ) {
			winners[ ++wPtr ] = last;
		}
		else {
			losers[ ++lPtr ] = last;
		}
	}

	/* [2] find max from the winners */
	max = winners[ wPtr ];
	while ( wPtr-- ) {
		float temp = winners[ wPtr ];
		if ( temp > max ) {
			max = temp;
		}
	}
	*maxRet = max;

	/* [3] find min from the losers */
	min = losers[ lPtr ];
	while ( lPtr-- ) {
		float temp = losers[ lPtr ];
		if ( temp < min ) {
			min = temp;
		}
	}
	*minRet = min;
}	

/* 將矩陣的值線性對應到[0, 255] */
void map_0_255( Matrix *source ) {
	float max, min;
	max_min( source, &max, &min );
	/* source = ( source - min ) / ( max - min ) * 255 */
	s_add( source, -min );
	s_mul( source, 255.f / ( max - min ) );
}

void ABS( Matrix *source ) {
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	int row, col, layer;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
			for ( col = 0; col < size2; col++ ) {
				source->data[ layer ][ row ][ col ] =
					fabs( source->data[ layer ][ row ][ col ] );
			}
		}
	}
}

float m_sum( Matrix *source ) {
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	int row, col, layer;
	float sum = 0.f;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
			for ( col = 0; col < size2; col++ ) {
				sum += source->data[ layer ][ row ][ col ];
			}
		}
	}
	return sum;
}

void change_0_to_1( Matrix *source ) {
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	int row, col, layer;
	for ( row = 0; row < size1; row++ ) {
		for ( col = 0; col < size2; col++ ) {
			for ( layer = 0; layer < size3; layer++ ) {
				float value = source->data[ layer ][ row ][ col ];
				if ( value >= 0.f && value < 1.f ) {
					source->data[ layer ][ row ][ col ] = 1.f;
				}
			}
		}
	}
}

/* 把整個矩陣當作vector取2-norm */
float v_norm2( Matrix *source ) {
	int size1 = source->size1, size2 = source->size2, size3 = source->size3;
	int row, col, layer;
	float norm = 0.f;
	for ( layer = 0; layer < size3; layer++ ) {
		for ( row = 0; row < size1; row++ ) {
			for ( col = 0; col < size2; col++ ) {
				float value = source->data[ layer ][ row ][ col ];
				norm += value * value;
			}
		}
	}
	return sqrt( norm );
}

/* 對2D矩陣取integral(自左上方的累積量值)
 * Recursion:
 * ii(x,y) = ii(x-1,y) + ii(x,y-1) - ii(x-1,y-1) + i(x,y)
 * base case: ii(-,y) = 0; ii(x,-) = 0
 */
void integral( Matrix *source, Matrix *dest ) {
	int size1 = source->size1, size2 = source->size2;
	int row, col;
	/* check it is 2-D */
	if ( source->size3 != 1 ) {
		error( "integral(): must be 2-D" );
	}

	zeros( dest, size1, size2, 1 );
	for ( row = 0; row < size1; row++ ) {
		for ( col = 0; col < size2; col++ ) {
			dest->data[ 0 ][ row ][ col ] = ii_eval( dest, row-1, col ) + ii_eval( dest, row, col-1 ) 
				- ii_eval( dest, row-1, col-1 ) + source->data[ 0 ][ row ][ col ];
		}
	}
}
inline float ii_eval( Matrix *ii, int row, int col ) {
	return ( row < 0 || col < 0 ) ? 0.f : ii->data[ 0 ][ row ][ col ];
}

/* rectangular sum of 4 corners 
 * if upper-left(beginning): (x1,y1), bottom-right(ending): (x2,y2)
 * => recsum = ii(x2,y2) + ii(x1-1,y1-1) - ii(x1-1,y2) - ii(x2,y1-1)
 */
float recSum( Matrix *ii, int rowBeg, int colBeg, int rowEnd, int colEnd ) {
	/* check for valid range */
	if ( rowBeg < 0 || rowEnd >= ii->size1 || colBeg < 0 || colEnd >= ii->size2 ) {
		error( "recSum(): Invalid specified endpoints." );
	}
	/* make sure the argument points are upper-left, then bottom-right */
	if ( rowBeg > rowEnd || colBeg > colEnd ) {
		error( "recSum(): Should be recSum( &ii, upper-left point, bottom-right point )." );
	}

	return ii_eval( ii, rowEnd, colEnd ) + ii_eval( ii, rowBeg-1, colBeg-1)
		- ii_eval( ii, rowBeg-1, colEnd ) - ii_eval( ii, rowEnd, colBeg-1 );
}

#if DEBUG
int main() {
	Matrix A, B, C, D, E, F, G, Gauss, H, H2, I, I2, ii, Ra, Rb, Rc, Rd;
	int row, col, count = 0;
	float max, min;
	float array[ 11 * 13 ] = {
	    2,-1,0,1,3,3,-4,2,-2,4,0,-5,-5,
	    -3,1,-3,0,2,5,4,3,4,3,1,1,4,
	    -1,-2,1,4,2,4,-4,0,-4,2,1,3,3,
	    -3,-1,4,2,-5,-5,3,5,-4,0,-4,-2,-3,
	    4,-1,-5,-2,-1,-2,3,-5,5,2,-4,0,4,
	    5,0,-1,-2,4,-3,2,0,-4,5,-2,-3,2,
	    2,0,-5,-3,-2,-2,-4,-2,-4,0,3,5,2,
	    2,5,1,5,1,5,-1,0,-4,-2,-1,0,5,
	    5,-3,0,-2,-2,1,0,2,-5,3,4,0,-5,
	    2,-2,3,-4,-4,3,0,-3,-1,-3,5,-3,-1,
	    1,1,-3,-2,-1,4,4,1,2,-3,-1,-4,0
	};
	clock_t tic, toc;
	srand( time( NULL ) );

	tic = clock();
	ones( &A, 3, 3, 3 );
	full_dump( &A, "A", BB, INT );
	RAND( &Ra, 5, 5, 1, 20, 29 );
	full_dump( &Ra, "Ra", ALL, INT );
	s_add( &Ra, -20.f );
	s_mul( &Ra, 5 );
	s_pow( &Ra, 0.5 );
	full_dump( &Ra, "((Ra-20)*5)^0.5, ", ALL, FLOAT );

	eye( &I, 3 );
	m_mul( &A, &I, GG, RR, &B );
	full_dump( &B, "A(:,:,1)*I, ", ALL, INT );

	RAND( &Rb, 3, 3, 3, 0, 9 );
	full_dump( &Rb, "Rb ", ALL, INT );
	m_mul( &Rb, &A, ALL, ALL, &C );
	full_dump( &C, "Rb * A ", ALL, INT );

	zeros( &D, 3, 3, 3 );
	full_assign( &C, &D, ALL, ALL );
	full_dump( &D, "C = Rb * A ", GG, INT );
	ones( &E, 15, 15, 1 );
	part_assign( &Ra, &E, 0, 4, 0, 4, 0, 0,  8, 12, 8, 12, 0, 0 );
	full_dump( &E, "E(part_assigned from Ra) ", ALL, INT );

    /* 2D cross-correlation */
    zeros( &F, 11, 13, 1 );
    for ( row = 0; row < 11; row++ ) {
        for ( col = 0; col < 13; col++ ) {
            F.data[ 0 ][ row ][ col ] = array[ count++ ];
        }
    }
    full_dump( &F, "F", ALL, INT );
    ones( &H, 5, 5, 1 );
    cross( &F, &H, &G );
    full_dump( &G, "F ** H ", RR, INT );

	/* simultaneous max and min */
	RAND( &Rc, 10, 9, 1, 0, 999 );
	full_dump( &Rc, "Rc", ALL, INT );
	max_min( &Rc, &max, &min );
	printf( "Rc: max = %d; min = %d\n", (int) max, (int) min );
	RAND( &Rd, 11, 9, 1, 0, 999 );
	full_dump( &Rd, "Rc", ALL, INT );
	max_min( &Rd, &max, &min );
	printf( "Rc: max = %d; min = %d\n", (int) max, (int) min );

	/* 4 gradient filter variants */
	zeros( &H2, 3, 3, 1 );
	H2.data[ 0 ][ 1 ][ 1 ] = 1;
	H2.data[ 0 ][ 1 ][ 2 ] = -1;
	cross( &F, &H2, &G );
	full_dump( &G, "cross[0,1,-1]", ALL, INT );
	gradient( &F, &G, horizontal, false );
	full_dump( &G, "grad[0,1,-1]", ALL, INT );

	H2.data[ 0 ][ 1 ][ 0 ] = -1;
	H2.data[ 0 ][ 1 ][ 1 ] = 0;
	H2.data[ 0 ][ 1 ][ 2 ] = 1;
	cross( &F, &H2, &G );
	full_dump( &G, "cross[-1,0,1]", ALL, INT );
	gradient( &F, &G, horizontal, true );
	full_dump( &G, "grad[-1,0,1]", ALL, INT );

	H2.data[ 0 ][ 0 ][ 1 ] = 0;
	H2.data[ 0 ][ 1 ][ 1 ] = 1;
	H2.data[ 0 ][ 2 ][ 1 ] = -1;
	H2.data[ 0 ][ 1 ][ 0 ] = 0;
	H2.data[ 0 ][ 1 ][ 2 ] = 0;
	cross( &F, &H2, &G );
	full_dump( &G, "cross[0,1,-1]T", ALL, INT );
	gradient( &F, &G, vertical, false );
	full_dump( &G, "grad[0,1,-1]T", ALL, INT );

	H2.data[ 0 ][ 0 ][ 1 ] = -1;
	H2.data[ 0 ][ 1 ][ 1 ] = 0;
	H2.data[ 0 ][ 2 ][ 1 ] = 1;
	cross( &F, &H2, &G );
	full_dump( &G, "cross[-1,0,1]T", ALL, INT );
	gradient( &F, &G, vertical, true );
	full_dump( &G, "grad[-1,0,1]T", ALL, INT );

	Gaussian( &Gauss, 8, 8 );
	full_dump( &Gauss, "Gauss( 8, 8 )", ALL, FLOAT );
	printf( "sum of Gauss = %f\n", m_sum( &Gauss ) );
	Gaussian( &Gauss, 8, 1 );
	full_dump( &Gauss, "Gauss( 8, 1 )", ALL, FLOAT );
	printf( "sum of Gauss = %f\n", m_sum( &Gauss ) );

	s_mul( &H, 1.5 ); /* H was ones( 5, 5, 1 ); */
	printf( "norm2( H ) = %f\n", v_norm2( &H ) );

	/* integral() and recSum() */
	ones( &I2, 8, 10, 1 );
	full_dump( &I2, "I2", ALL, INT );
	integral( &I2, &ii );
	full_dump( &ii, "integral( &I2 )", ALL, INT );
	printf( "recSum((0,0),(7,9)) = %d\n", (int)recSum( &ii, 0, 0, 7, 9 ) );
	printf( "recSum((5,5),(5,5)) = %d\n", (int)recSum( &ii, 5, 5, 5, 5 ) );
	printf( "recSum((0,0),(3,2)) = %d\n", (int)recSum( &ii, 0, 0, 3, 2 ) );
	printf( "recSum((3,3),(7,7)) = %d\n", (int)recSum( &ii, 3, 3, 7, 7 ) );
	printf( "recSum((4,0),(4,9)) = %d\n", (int)recSum( &ii, 4, 0, 4, 9 ) );

	toc = clock();
	runningTime( tic, toc );

	/* free memory space */
	freeMatrix( &A ); freeMatrix( &B ); freeMatrix( &C ); freeMatrix( &D ); freeMatrix( &E );
	freeMatrix( &F ); freeMatrix( &G ); freeMatrix( &H ); freeMatrix( &H2 );
	freeMatrix( &I ); freeMatrix( &I2 ); freeMatrix( &ii ); freeMatrix( &Ra ); freeMatrix( &Rb ); 
	freeMatrix( &Rc ); freeMatrix( &Rd ); 

	return 0;
}

#endif
