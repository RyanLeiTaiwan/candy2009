/** File: car-train.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/25
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
	FILE *fout;
	clock_t tic, toc;
	/* Feature pools for POS and NEG data */
	float ***POS = NULL, ***NEG = NULL;
	/* image rejection record */
	bool *rejected;
	int rejectCount = 0;
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
		"Example: ./car-train pics/INRIA/train_car pics/INRIA/train_nocar train.txt 0.001 0.999 0.50 1.00" );
	}
	/* check file open */
	if ( !( fout = fopen( argv[ 3 ], "w" ) ) ) {
		error( "car-train(): Output file open error." );
	}
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
	printf( "# of blocks per image: %d.\n", blockCount );
	getchar();

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
		learnA( posCount, negCount, blockCount, &rejectCount, rejected, 
			POS, NEG, H, &F_current, d_minA, f_maxA, fout );
	}
#if META
	learnM( i, posCount, negCount, rejected, &F_current, d_minM );
#endif
	i++;

	/*** Other A[i,j] stages ***/
	while ( F_current > F_target ) {
		for ( j = 1; j <= ni; j++ ) {
			printf( "\nLearning stage A[%d,%d]...\n", i, j );
			learnA( posCount, negCount, blockCount, &rejectCount, 
				rejected, POS, NEG, H, &F_current, d_minA, f_maxA, fout );
		}
#if META
		learnM( i, posCount, negCount, rejected, &F_current, d_minM );
#endif
		i++;
	}

	/* Write "0 0" to mark the end of file */
	fprintf( fout, "0 0\n" );

	/* stop timer */
	toc = clock();
	runningTime( tic, toc );

	fclose( fout );
	free( rejected );

	return 0;
}

