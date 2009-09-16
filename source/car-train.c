/** File: car-train.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/13
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

/* learn AdaBoost stage A[i,j] */
void learnA( int i, int j, float *F_current, float d_minA, float f_maxA ) {
	float f_local = 1.0;
	printf( "Learning stage A[%d,%d]...\n", i, j );
	/** [0] Randomly select NEG examples from the bootstrap set **/
	select_neg();
	while ( f_local > f_maxA ) {
		/* [1] Add a weak learner h to the strong classifier A[i,j] */
		weak_select();
		/* [2] Modify the threshold of A[i,j] to fulfill d_minA */
		/* [3] Calculate f_local of A[i,j] */
		f_local = 0.49;
	}
	*F_current *= f_local;
	printf( "f_local: %f, F_current: %f\n", f_local, *F_current );
}

/* Learn Meta stage M[i] */
void learnM( int i, float *F_current, float d_minM ) {
	printf( "Learning stage M[%d]...\n", i );
}
	
/* Randomly select negative examples from the bootstrap */
void select_neg() {
}

/* Select a weak learner h to the strong classifier A[i,j] */
void weak_select() {

}

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

void extract_all_images( char *directory, Feature ***POOL ) {
	/** max # of images: 1000 **/
	char fileName[ 80 ];
	int pathLen;
	FILE *ftest;
	char D1, D2, D3;
	int imgCount = 0;
	int blockCount = 0;
	int Iid; /* image id */
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
	imgCount = count_images( fileName, pathLen );
	/* count the number of blocks per image, 
	 * and allocate the feature pool memory (2-D array) for the entire data set */
	blockCount = count_blocks( d_width, d_height, b_size_min, b_size_step, b_pos_step );
	printf( "# of blocks per image: %d.\n", blockCount );
	getchar();
	*POOL = (Feature **) malloc( imgCount * sizeof( Feature * ) );
	for ( Iid = 0; Iid < imgCount; Iid++ ) {
		(*POOL)[ Iid ] = (Feature *) malloc( blockCount * sizeof( Feature ) );
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
					extract_image( fileName, Iid++, POOL, d_width, d_height, 
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

void extract_image( char *fileName, int Iid, Feature ***POOL, int d_width, int d_height, 
	int b_size_min, int b_size_step, int b_pos_step ) {
	int b_width, b_height; /* block width, block height */
	int x_beg, x_end, y_beg, y_end; /* x, y coordinates */
	int Bid = 0; /* block id */
	Matrix img;
	printf( "Extracting %s ... ", fileName );
	/* Read the image and transform it into gray-scale */
	imread( fileName, &img );
	/* check height & width */
	if ( img.size1 != d_height || img.size2 != d_width ) {
		printf( "\nDetection window size: %d x %d.\n", d_width, d_height );
		error( "extract_single(): Image size should be consistent with detection window size." );
	}
	color2Gray( &img );

	/* try all possible sizes and positions within the image */
	for ( b_width = b_size_min; b_width <= d_width; b_width += b_size_step ) {
		for ( b_height = b_size_min; b_height <= d_height; b_height += b_size_step ) {
			for ( x_beg = 0, x_end = x_beg + b_height - 1; x_end < d_height; 
				x_beg += b_pos_step, x_end = x_beg + b_height - 1 ) {
				for ( y_beg = 0, y_end = x_beg + b_width - 1; y_end < d_width; 
					y_beg += b_pos_step, y_end = y_beg + b_width - 1 ) {
					/* extract features of this block */
					extract_block( Bid++, POOL, &img, x_beg, y_beg, b_height, b_width );
				}
			}
		}
	}
	printf( "done\n" );
}

/*** feature extraction of a single block 
 *** the ACTUAL EXTRACTION part ***/
void extract_block( int Bid, Feature ***POOL, Matrix *img, 
	int x_beg, int y_beg, int b_height, int b_width ) {



}

/********************************     start of main     **********************************/
int main( int argc, char *argv[] ) {
	/* F means false-positive rate; d means detection rate */
	/* A means AdaBoost stage; M means meta stage */
	float F_target; /* target overall false positive rate */
	float F_current = 1.0; /* current overall false positive rate */
	float d_minA; /* minimum acceptable detection rate per AdaBoost stage */
	float f_maxA; /* maximum acceptable false positive rate per AdaBoost stage */
	float d_minM; /* minimum acceptable detection rate per meta-stage */
	int i = 1; /* AdaBoost stage counter */
	/* parameters ni, j */
	int ni = 1, j;
	FILE *ftest;
	/* Feature pools for POS and NEG data */
	Feature **POS = NULL, **NEG = NULL;

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

	/*** the feature extraction process ***/
	printf( "Start of feature extraction...\n" );
	extract_all_images( argv[ 1 ], &POS );
	printf( "Extraction of POS data completed.\n" );
	//extract_all_images( argv[ 2 ], &NEG );
	//printf( "Extraction of NEG data completed.\n" );

	/*****  pseudo-code from the Chen-and-Chen paper  *****/

	printf( "Start of car-train...\n" );
	/*** A[1,j] stage as an exception ***/
	for ( j = 1; j <= ni + 1; j++ ) {
		learnA( i, j, &F_current, d_minA, f_maxA );
	}
#if META
	learnM( i, &F_current, d_minM );
#endif
	i++;

	/*** Other A[i,j] stages ***/
	while ( F_current > F_target ) {
		for ( j = 1; j <= ni; j++ ) {
			learnA( i, j, &F_current, d_minA, f_maxA );
		}
#if META
		learnM( i, &F_current, d_minM );
#endif
		i++;
	}

	return 0;
}

