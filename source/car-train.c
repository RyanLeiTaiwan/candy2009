/** File: car-train.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/23
 ** Description: The car-training program based on the paper:
    Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
	Important techniques / concepts:
	1. Real AdaBoost -> For now, use basic binary Adaboost
	2. Cascaded classifiers -> For now, it is without meta-stages (no confidence values)
	3. Rectangle / Edge Orientation Histogram / Edge-Density features
	4. Integral image / Integral histogram
 **/
#include "../include/car-train.h"
#define META 0

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

void extract_all_images( char *directory, float ****POOL, int *imgCount, int *blockCount ) {
	/** max # of images: 1000 **/
	char fileName[ 80 ];
	int pathLen;
	FILE *ftest;
	char D1, D2, D3;
	int Iid, Bid; /* image id, block id */
	/** Some self-defined feature extraction parameters: **/
	const int d_width = 128; /* detection window width */
	const int d_height = 128; /* detection window height */
	const int b_size_min = 8; /* minumum block size */
	const int b_size_step = 8; /* block size increment */
	const int b_pos_step = 8; /* block position increment */
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
	 * and allocate the feature pool memory (2-D array) for the entire data set */
	*blockCount = count_blocks( d_width, d_height, b_size_min, b_size_step, b_pos_step );
	printf( "# of blocks per image: %d.\n", *blockCount );
	getchar();
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
					extract_image( fileName, Iid++, *POOL, d_width, d_height, 
						b_size_min, b_size_step, b_pos_step );
				}
			}
		}
	}
}

int count_blocks( int d_width, int d_height, int b_size_min, int b_size_step, int b_pos_step ) {
	int ret = 0;
	int b_width, b_height, x_beg, x_end, y_beg, y_end;
	/* try all possible sizes and positions within the image */
	for ( b_width = b_size_min; b_width <= d_width; b_width += b_size_step ) {
		for ( b_height = b_size_min; b_height <= d_height; b_height += b_size_step ) {
			for ( x_beg = 0, x_end = x_beg + b_height - 1; x_end < d_height; 
				x_beg += b_pos_step, x_end = x_beg + b_height - 1 ) {
				for ( y_beg = 0, y_end = x_beg + b_width - 1; y_end < d_width; 
					y_beg += b_pos_step, y_end = y_beg + b_width - 1 ) {
					ret++;
					/* Note: vertical first for convention */
					/* printf( "%d x %d: (%d,%d) to (%d,%d)\n", b_height, b_width, 
						x_beg, y_beg, x_end, y_end ); */
				}
			}
		}
	}
	return ret;
}

void extract_image( char *fileName, int Iid, float ***POOL, int d_width, int d_height, 
	int b_size_min, int b_size_step, int b_pos_step ) {
	int b_width, b_height; /* block width, block height */
	int x_beg, x_end, y_beg, y_end; /* x, y coordinates */
	int Bid = 0; /* block id */
	int row, col, size1, size2;
	/* original image, image after normalization, integral image, and their "square" versions */
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
				for ( y_beg = 0, y_end = x_beg + b_width - 1; y_end < d_width; 
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


/********************************     start of main     **********************************/
int main( int argc, char *argv[] ) {
	/* F means false-positive rate; d means detection rate */
	/* A means AdaBoost stage; M means meta stage */
	float F_target; /* target overall faGlse positive rate */
	float F_current = 1.0; /* current overall false positive rate */
	float d_minA; /* minimum acceptable detection rate per AdaBoost stage */
	float f_maxA; /* maximum acceptable false positive rate per AdaBoost stage */
	float d_minM; /* minimum acceptable detection rate per meta-stage */
	int posCount, negCount, blockCount; /* count # of images and # of blocks */
	int i = 1, j; /* AdaBoost stage counter */
	int m;
	/* parameters ni, j */
	const int ni = 1;
	FILE *ftest;
	clock_t tic, toc;
	/* Feature pools for POS and NEG data */
	float ***POS = NULL, ***NEG = NULL;
	/* image rejection record */
	bool *rejected;
	Ada *H = NULL; /* H is the strong classifier A(i,j) consisting of many AdaWeak objects */


	/* Usage: car-train POS NEG output F_target d_minA f_maxA d_minM
	 * POS: directory of positive training data
	 * NEG: directory of negative training data
	 * output: output file of the training program
	 * The images in POS and NEG should have file names: trainDDD.bmp, D form [0,9],
	 * and starts with train000.bmp without discontinuities.
	 */
	if ( argc != 8 ) {
		error( "Usage: car-train POS NEG Output F_target d_minA f_maxA d_minM.\n"
		"Example: ./car-train pics/INRIA/train_car pics/INRIA/train_nocar train.txt 0.01 0.999 0.50 1.00" );
	}
	/* check file open */
	if ( !( ftest = fopen( argv[ 3 ], "w" ) ) ) {
		error( "car-train(): Output file open error." );
	}
	fclose( ftest );
	/* get arguments */
	F_target = atof( argv[ 4 ] );
	d_minA = atof( argv[ 5 ] );
	f_maxA = atof( argv[ 6 ] );
	d_minM = atof( argv[ 7 ] );
	/* start timer */
	tic = clock();

	/*** the feature extraction process ***/
	printf( "Start of feature extraction...\n" );
	extract_all_images( argv[ 1 ], &POS, &posCount, &blockCount );
	printf( "Extraction of POS data completed.\n" );
	extract_all_images( argv[ 2 ], &NEG, &negCount, &blockCount );
	printf( "Extraction of NEG data completed.\n" );

	/*****  pseudo-code from the Chen-and-Chen paper  *****/
	srand( time( NULL ) );
	printf( "Start of car-train...\n" );
	/* allocate rejection table */
	rejected = (bool *)malloc( negCount * sizeof( bool ) );
	for ( m = 0; m < negCount; m++ ) {
		rejected[ m ] = false;
	}
	/*** A[1,j] stage as an exception ***/
	for ( j = 1; j <= ni + 1; j++ ) {
		printf( "\nLearning stage A[%d,%d]...\n", i, j );
		learnA( posCount, negCount, blockCount, rejected, POS, NEG, H, &F_current, d_minA, f_maxA );
	}
#if META
	learnM( i, posCount, negCount, rejected, &F_current, d_minM );
#endif
	i++;

	/*** Other A[i,j] stages ***/
	while ( F_current > F_target ) {
		for ( j = 1; j <= ni; j++ ) {
			printf( "\nLearning stage A[%d,%d]...\n", i, j );
			learnA( posCount, negCount, blockCount, rejected, POS, NEG, H, &F_current, d_minA, f_maxA );
		}
#if META
		learnM( i, posCount, negCount, rejected, &F_current, d_minM );
#endif
		i++;
	}

	/* free memory */
	free( rejected );
	/* stop timer */
	toc = clock();
	runningTime( tic, toc );

	return 0;
}

