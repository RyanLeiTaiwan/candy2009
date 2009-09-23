/** File: car-adaboost.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/20
 ** Modification: 2009/09/23
 ** Description: AdaBoost learning details.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include "../include/car-train.h"

/* Randomly select negative examples from the bootstrap */
void select_neg( int posCount, int negCount, bool rejected[], int selectTable[] ) {
	int i;
	/* indicate whether this image has been selected */
	bool *selected = (bool *)malloc( negCount * sizeof( bool ) );
	for ( i = 0; i < negCount; i++ ) {
		selected[ i ] = false;
	}

	for ( i = 0; i < posCount; i++ ) {
		int Pid;
		/* Randomly select an image that is not yet REJECTED or SELECTED 
		 * by the AdaBoost learning */
		do {
			Pid = rand() % negCount;
			printf( "Try to select %d\n", Pid );
		}
		while ( rejected[ Pid ] || selected[ Pid ] );
		printf( "Selected %d\n", Pid );
		selected[ Pid ] = true;
		selectTable[ i ] = Pid;
	}

	free( selected );
}

/* Select a weak learner h to the strong classifier H */
void weak_select( int posCount, int negCount, int blockCount, float ***POS, float ***NEG, Ada *H ) {
	/* For all possible features (Bid, FeatureType, Fid) */
	int Bid, Fid;
	for ( Bid = 0; Bid < blockCount; Bid++ ) {
		for ( Fid = 0; Fid < FEATURE_COUNT; Fid++ ) {
			classify( posCount, negCount, POS, NEG, H, Bid, Fid );
		}
	}
}


void classify( int posCount, int negCount, float ***POS, float ***NEG, Ada *H, int Bid, int Fid ) {


}

/* learn AdaBoost stage A[i,j] */
void learnA( int posCount, int negCount, int blockCount, bool rejected[], float ***POS, float ***NEG,
	Ada *H, float *F_current, float d_minA, float f_maxA ) {

	int *selectTable; /* image selection table */
    float f_local = 1.0;
	int h_allocated = 10;
	int h_used = 0;

	selectTable = (int *)malloc( posCount * sizeof( int ) );
	H = (Ada *)malloc( h_allocated * sizeof( Ada ) );

    /** [0] Randomly select NEG examples from the bootstrap set **/
    select_neg( posCount, negCount, rejected, selectTable );
    while ( f_local > f_maxA ) {
        /* [1] Add a weak learner h to the strong classifier A[i,j] */
		if ( h_used == h_allocated ) {
			h_allocated *= 2;
			printf( "call realloc()\n" );
			H = (Ada*)realloc( H, h_allocated * sizeof( Ada ) );
		}
        weak_select( posCount, negCount, blockCount, POS, NEG, H );
		h_used++;
		printf( "h_used: %d\n", h_used );
        /* [2] Modify the threshold of A[i,j] to fulfill d_minA */
        /* [3] Calculate f_local of A[i,j] */
        f_local = 0.49;
    }
    *F_current *= f_local;
    printf( "f_local: %f, F_current: %f\n", f_local, *F_current );

	/* output to file */
	free( selectTable );
	free( H );
}

/* Learn Meta stage M[i] */
void learnM( int i, int posCount, int negCount, bool rejected[], float *F_current, float d_minM ) {
    printf( "Learning stage M[%d]...\n", i );
}



