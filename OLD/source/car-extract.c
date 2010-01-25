/** File: car-extract.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2010/01/26
 ** Description: The function implementations of feature extraction
    Features:
	  1. Rectangle features -- Integral image, 45'-tilted-version techniques
	  2. Edge Orientation Histograms (EOH) -- Integral histogram
	  3. Edge Density (ED) -- Integral image
 **/
#include "../include/car-extract.h"

int count_images( char *fileName, int pathLen ) {
	FILE *ftest;
	char D1, D2, D3;
	int imgCount = 0;
	for ( D1 = '0'; D1 <= '9'; D1++ ) {
		for ( D2 = '0'; D2 <= '9'; D2++ ) {
			for ( D3 = '0'; D3 <= '9'; D3++ ) {
				fileName[ pathLen + 5 ] = D1;
				fileName[ pathLen + 6 ] = D2;
				fileName[ pathLen + 7 ] = D3;
				/* termination condition */
				if ( !( ftest = fopen( fileName, "r" ) ) ) {
					return imgCount++;
				}
				else {
					fclose( ftest );
					imgCount++;
				}
			}
		}
	}
	return -1;
}

int count_blocks() {
	int ret = 0;
	int b_width, b_height, x_beg, x_end, y_beg, y_end;
	/* try all possible sizes and positions within the image */
	for ( b_width = b_size_min; b_width <= d_width; b_width += b_size_step ) {
		for ( b_height = b_size_min; b_height <= d_height; b_height += b_size_step ) {
			for ( x_beg = 0, x_end = x_beg + b_height - 1; x_end < d_height; 
				x_beg += b_pos_step, x_end += b_pos_step ) {
				for ( y_beg = 0, y_end = y_beg + b_width - 1; y_end < d_width; 
					y_beg += b_pos_step, y_end = y_beg + b_width - 1 ) {
						
					ret++;
					/* Note: vertical first for convention */
					/*printf( "%d x %d: (%d,%d) to (%d,%d)\n", b_height, b_width, 
						x_beg, y_beg, x_end, y_end );*/
				}
			}
		}
	}
	return ret;
}


void extract_all_images( char *directory, float ****POOL, int *imgCount, int *blockCount ) {
	/** max # of images: 1000 **/
	char fileName[ 80 ];
	int pathLen;
	FILE *ftest;
	char D1, D2, D3;
	int Iid, Bid; /* image id, block id */

	strcpy( fileName, directory );
	pathLen = strlen( fileName );
    /* append '/' if necessary (unix) */
    if ( fileName[ pathLen - 1 ] != '/' ) {
        fileName[ pathLen++ ] = '/';
        fileName[ pathLen ] = '\0';
    }
	sprintf( fileName, "%strain000.bmp", fileName );

	/* count the number of images */
	*imgCount = count_images( fileName, pathLen );
	/* count the number of blocks per image, 
	 * and allocate the feature pool memory (3-D array) for the entire data set */
	*blockCount = count_blocks();
	*POOL = (float ***) malloc( (*imgCount) * sizeof( float ** ) );
	for ( Iid = 0; Iid < *imgCount; Iid++ ) {
		(*POOL)[ Iid ] = (float **) malloc( (*blockCount) * sizeof( float * ) );
		for ( Bid = 0; Bid < *blockCount; Bid++ ) {
			(*POOL)[ Iid ][ Bid ] = (float *) malloc( FEATURE_COUNT * sizeof( float ) );
		}
	}

	/* extract each image */
	Iid = 0;
	for ( D1 = '0'; D1 <= '9'; D1++ ) {
		for ( D2 = '0'; D2 <= '9'; D2++ ) {
			for ( D3 = '0'; D3 <= '9'; D3++ ) {
				fileName[ pathLen + 5 ] = D1;
				fileName[ pathLen + 6 ] = D2;
				fileName[ pathLen + 7 ] = D3;
				/* termination condition */
				if ( !( ftest = fopen( fileName, "r" ) ) ) {
					return;
				}
				else {
					fclose( ftest );
					extract_image( fileName, Iid++, *POOL );
				}
			}
		}
	}
}

void extract_image( char *fileName, int Iid, float ***POOL ) {
	int b_width, b_height; /* block width, block height */
	int x_beg, x_end, y_beg, y_end; /* x, y coordinates */
	int Bid = 0; /* block id */
	int row, col, size1, size2;
	/* original image, image after normalization, integral image, and their "square" versions */
	/* [BUG] 20100126: Mean and variance normalization should instead be applied to each "block", not the entire image.
	 * In this case, the "block size effect" can be reduced.
	 */
	Matrix img, img2, ii, ii2, img_norm, ii_norm;
	/* vertical/horizontal gradients, gradient magnitude/angle */
	const int binNum = 9; /* parameter: # of EOH bins */
	const int binSize = 360 / binNum;
	const int halfBinSize = binSize / 2;
	int dist = 0; /* distance from bin's central value */
	float weight;
	Matrix Gv, Gh, GmEOH, GmED, ii_EOH, ii_ED; /* GmEOH is a 3D matrix with binNum layers */
	assert( !( 360 % ( binNum * 2 ) ) ); /* binNum*2 should divide 360 */

	printf( "Extracting %s ... ", fileName );
	/* Read the image and transform it into gray-scale */
	imread( fileName, &img );
	/* check height & width */
	if ( img.size1 != d_height || img.size2 != d_width ) {
		printf( "\nDetection window size: %d x %d.\n", d_width, d_height );
		error( "extract_single(): Image size should be consistent with detection window size." );
	}
	color2Gray( &img ); /* transform into gray-scale */
	copy( &img, &img2 );
	copy( &img, &img_norm );
	s_pow( &img2, 2.f );
	/* [1] for rectangle features, compute usual/square integral images */
	integral( &img, &ii );
	integral( &img2, &ii2 );
	freeMatrix( &img2 );
	/* [2] for EOH and ED features, compute the vertical/horizontal gradients */
	gradient( &img, &Gv, vertical, true );
	gradient( &img, &Gh, horizontal, true ); 
	freeMatrix( &img );
	/* mean and variance normalization using integral image */
	mean_variance_normalize( &img_norm, &ii, &ii2 );
	freeMatrix( &ii ); freeMatrix( &ii2 );
	/* compute integral image of img_norm */
	integral( &img_norm, &ii_norm );
	freeMatrix( &img_norm );

	size1 = img.size1; size2 = img.size2;
	zeros( &GmEOH, size1, size2, binNum );
	zeros( &GmED, size1, size2, 1 );
	/* Next, compute gradient magnitude and angle for each pixel */
	for ( row = 0; row < size1; row++ ) {
		for ( col = 0; col < size2; col++ ) {
			/* temp gradient data */
			float Fv = Gv.data[ 0 ][ row ][ col ];
			float Fh = Gh.data[ 0 ][ row ][ col ];
			float Fm = sqrt( Fv * Fv + Fh * Fh ); /* gradient magnitude */
			float ang_float = atan( Fv / Fh ) * 180.f / M_PI; /* gradient angle */
			int ang;
			int bin; /* the bin it falls into */

			/* write directly to the GmED matrix, and ED is done */
			GmED.data[ 0 ][ row ][ col ] = Fm;

			/** Need to ADJUST the angle into II, III, IV quadrants **/
			if ( Fh == 0 && Fv == 0 ) {
				/* special case: zero magnitude */
				ang_float = 0;
			}
            else if ( Fh < 0 ) { /* II, III */
                ang_float += 180;
            }
            else if ( Fv < 0 ) { /* IV */
                ang_float += 360;
            }
            assert( ang_float >= 0 && ang_float < 360 );
			ang = (int)ang_float;
			//printf( "Fv: %f, Fh: %f, Fm: %f, ang: %d\n", Fv, Fh, Fm, ang );
            /* convert angle into orientation bins */
            /* 2009.07.28: 避免角度落在兩個bin之間的boundary effects，不該直接轉成bin，
             * 而要用SIFT Sec.6.1說的"trilinear interpolation"把票分配給鄰近的兩個bin。
             */
			/* [a.1] Special case: exactly central values */
			if ( !( ( ang - halfBinSize ) % binSize ) ) {
				bin = ( ang - halfBinSize ) / binSize;
				GmEOH.data[ bin ][ row ][ col ] = Fm;
			}
			/* [a.2] Special case: first half-bin region */
			else if ( ang >= 0 && ang < halfBinSize ) {
				dist = ang + halfBinSize;
				weight = 1 - abs( dist ) / (float)binSize;
				GmEOH.data[ binNum-1 ][ row ][ col ] = Fm * ( weight );
				GmEOH.data[ 0 ][ row ][ col ] = Fm * ( 1 - weight );
			}
			/* [a.3] Special case: last half-bin region */
			else if ( ang >= 360 - halfBinSize && ang < 360 ) {
				dist = ang - 360 + halfBinSize;
				weight = 1 - abs( dist ) / (float)binSize;
				GmEOH.data[ binNum-1 ][ row ][ col ] = Fm * ( weight );
				GmEOH.data[ 0 ][ row ][ col ] = Fm * ( 1 - weight );
			}
			else {
				/* [b] Find bin-center to the LEFT and compute the weight */
				bin = ( ang - halfBinSize ) / binSize;
				dist = ang - ( bin * binSize + halfBinSize);
				weight = 1 - abs( dist ) / (float)binSize;
				GmEOH.data[ bin ][ row ][ col ] = Fm * ( weight );
				/* [c] The bin-center to the RIGHT and weight are automatically determined */
				GmEOH.data[ bin+1 ][ row ][ col ] = Fm * ( 1 - weight );
			}
			assert( abs( dist ) < binSize );
#if 0
			printf( "Fv: %f, Fh: %f, Gm: %f, ang: %d\n", Fv, Fh, Fm, ang );
			int i;
			printf( "b0: %.3f", GmEOH.data[ 0 ][ row ][ col ] );
			for ( i = 1; i < binNum; i++ ) {
				printf( ", b%d: %.3f", i, GmEOH.data[ i ][ row ][ col ] );
			}
			printf( "\n" );
#endif
		}
	}
	freeMatrix( &Gv ); freeMatrix( &Gh );

	/* compute the ii_EOH, ii_ED */
	integral( &GmEOH, &ii_EOH );
	freeMatrix( &GmEOH );
	integral( &GmED, &ii_ED );
	freeMatrix( &GmED );
	
	/* try all possible sizes and positions within the image */
	for ( b_width = b_size_min; b_width <= d_width; b_width += b_size_step ) {
		for ( b_height = b_size_min; b_height <= d_height; b_height += b_size_step ) {
			for ( x_beg = 0, x_end = x_beg + b_height - 1; x_end < d_height; 
				x_beg += b_pos_step, x_end = x_beg + b_height - 1 ) {
				for ( y_beg = 0, y_end = y_beg + b_width - 1; y_end < d_width; 
					y_beg += b_pos_step, y_end = y_beg + b_width - 1 ) {
					/* Note: vertical first for convention */
#if 0
					 printf( "%d x %d: (%d,%d) to (%d,%d)\n", b_height, b_width, 
						x_beg, y_beg, x_end, y_end );
#endif
					/** extract features of this block (car-extract.c) **/
					extract_block( Iid, Bid++, POOL, &ii_norm, &ii_EOH, &ii_ED,
						x_beg, y_beg, x_end, y_end );
				}
			}
		}
	}
	printf( "done\n" );
	
	/* free remaining matrices */
	freeMatrix( &ii_norm ); freeMatrix( &ii_EOH ); freeMatrix( &ii_ED );
}

void extract_block( int Iid, int Bid, float ***POOL, Matrix *ii_norm, Matrix *ii_EOH,
	Matrix *ii_ED, int rowBeg, int colBeg, int rowEnd, int colEnd ) {
	/* define row and col middle, 1st quarter, 3rd quarter points */
	const int rowMid = ( rowBeg + rowEnd ) / 2;
	const int rowQ1 = ( rowBeg + rowMid ) / 2;
	const int rowQ3 = ( rowMid+1 + rowEnd ) / 2;
	const int colMid = ( colBeg + colEnd ) / 2;
	const int colQ1 = ( colBeg + colMid ) / 2;
	const int colQ3 = ( colMid+1 + colEnd ) / 2;
	const int area = ( rowEnd - rowBeg + 1 ) * ( colEnd - colBeg + 1 );
	const int binNum = ii_EOH->size3;
	int k;
	float all; /* for debug */
	float EOH_total;
	float EOH_bin[ binNum ];
	/* [1.1] 5 rectangle features, normal version */
	POOL[ Iid ][ Bid ][ REC_BEGIN ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowMid, colEnd ) -
		recSum( ii_norm, 0, rowMid+1, colBeg, rowEnd, colEnd );
#if 0
	printf( "image: %d, block: %d, value: %f\n", Iid, Bid, POOL[ Iid ][ Bid ][ REC_BEGIN ] );
#endif
	POOL[ Iid ][ Bid ][ REC_BEGIN + 1 ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowEnd, colMid ) -
		recSum( ii_norm, 0, rowBeg, colMid+1, rowEnd, colEnd );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 2 ] = 
		recSum( ii_norm, 0, rowBeg, colBeg, rowMid, colMid ) +
		recSum( ii_norm, 0, rowMid+1, colMid+1, rowEnd, colEnd ) -
		recSum( ii_norm, 0, rowBeg, colMid+1, rowMid, colEnd ) -
		recSum( ii_norm, 0, rowMid+1, colBeg, rowEnd, colMid );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 3 ] =
		recSum( ii_norm, 0, rowBeg, colBeg, rowQ1, colEnd ) -
		recSum( ii_norm, 0, rowQ1+1, colBeg, rowQ3, colEnd ) +
		recSum( ii_norm, 0, rowQ3+1, colBeg, rowEnd, colEnd );
	POOL[ Iid ][ Bid ][ REC_BEGIN + 4 ] =
		recSum( ii_norm, 0, rowBeg, colBeg, rowEnd, colQ1 ) -
		recSum( ii_norm, 0, rowBeg, colQ1+1, rowEnd, colQ3 ) +
		recSum( ii_norm, 0, rowBeg, colQ3+1, rowEnd, colEnd );

	/* [1.2] 5 rectangle features, 45'-tilted version (NOT YET implemented) */

	/* [2] 9 EOH features */
	/* compute the bin EOH values and total EOH values */
	EOH_total = 0.f;
	for ( k = 0; k < binNum; k++ ) {
		float sum = recSum( ii_EOH, k, rowBeg, colBeg, rowEnd, colEnd );
		EOH_bin[ k ] = sum;
		EOH_total += sum;
	}
	
	/* record the EOH features */
	all = 0.f; /* for debug */
	for ( k = 0; k < binNum; k++ ) {
		float ratio = EOH_bin[ k ] / ( EOH_total + EPSILON );
		POOL[ Iid ][ Bid ][ EOH_BEGIN + k ] = ratio;
		all += ratio;
	}
#if 0
	printf( "image: %d, block: %d\nb0: %.3f", Iid, Bid, POOL[ Iid ][ Bid ][ EOH_BEGIN ] );
	for ( k = 1; k < binNum; k++ ) {
		printf( ", b%d: %.3f", k, POOL[ Iid ][ Bid ][ EOH_BEGIN + k ] );
	}
	printf( ", all: %.2f\n", all );
#endif

	/* [3] 1 ED feature */
	POOL[ Iid ][ Bid ][ ED_BEGIN ] =
		recSum( ii_ED, 0, rowBeg, colBeg, rowEnd, colEnd ) / (float)area;
}

