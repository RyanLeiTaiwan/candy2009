/** File: car-adaboost.c
 ** Author: Ryan Lei
 ** Creation: 2009/09/20
 ** Modification: 2009/09/25
 ** Description: AdaBoost learning details.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include "../include/car-train.h"
#define SHOWAVGERROR 0

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
		}
		while ( rejected[ Pid ] || selected[ Pid ] );
		selected[ Pid ] = true;
		selectTable[ i ] = Pid;
	}

	free( selected );
}

/* Add a weak learner h to the strong classifier H */
void addWeak( int posCount, int negCount, int blockCount, int selectTable[],
	float ***POS, float ***NEG, Matrix *posWeight, Matrix *negWeight, Ada *strong, int *hUsed ) {
	/* For all possible features (Bid, FeatureType, Fid) */
	int Pid, Bid, Fid;
	Matrix posCorrect, negCorrect, ONES; /* classification correctness matrices */
	Matrix bestPosCorrect, bestNegCorrect;
	float bestError = 1.f;
	int bestBid, bestFid;
	short bestParity;
	float bestDecision;
	float alpha;
	float normalize;
#if SHOWAVGERROR
	float avgError = 1.f;
	int round = 0;
#endif
	ones( &posCorrect, 1, posCount, 1 );
	ones( &negCorrect, 1, posCount, 1 );
	ones( &ONES, 1, posCount, 1 );

#if 0
	/* Show the data weight */
	full_dump( posWeight, "posWeight", ALL, FLOAT );
	full_dump( negWeight, "negWeight", ALL, FLOAT );
#endif

	for ( Bid = 0; Bid < blockCount; Bid++ ) {
		for ( Fid = 0; Fid < FEATURE_COUNT; Fid++ ) {

			float POS_mean = 0.f, NEG_mean = 0.f;
			float decision;
			float error = 0.f;
			short parity = +1; /* default: ... NEG_mean ... | ... POS_mean ... */

			/* [0] Initilize the matrices: Restore to all 1's */
			full_assign( &ONES, &posCorrect, ALL, ALL );
			full_assign( &ONES, &negCorrect, ALL, ALL );

			/* [1] Compute the POS & NEG mean feature value */
			for ( Pid = 0; Pid < posCount; Pid++ ) {
				POS_mean += POS[ Pid ][ Bid ][ Fid ];
				NEG_mean += NEG[ selectTable[ Pid ] ][ Bid ][ Fid ];
			}
			POS_mean /= posCount;
			NEG_mean /= negCount;

			/* default threshold: avg of POS_mean and NEG_mean */
			decision = ( POS_mean + NEG_mean ) / 2.f;
			if ( POS_mean < NEG_mean ) {
				parity = -1; /* toggle: ... POS_mean ... | ... NEG_mean ... */
			}
#if 0
			printf( "Bid: %d, Fid: %d, POS_mean: %f, NEG_mean: %f\n", Bid, Fid, POS_mean, NEG_mean );
#endif

			/* [2] Classification and compute the WEIGHTED error rate */
			for ( Pid = 0; Pid < posCount; Pid++ ) {
				/* positive & wrong */
				if ( parity * POS[ Pid ][ Bid ][ Fid ] < parity * decision ) {
					error += posWeight->data[ 0 ][ 0 ][ Pid ];
					posCorrect.data[ 0 ][ 0 ][ Pid ] = -1.f;
				}
				/* negative & wrong */
				if ( parity * NEG[ selectTable[ Pid ] ][ Bid ][ Fid ] > parity * decision ) {
					error += negWeight->data[ 0 ][ 0 ][ Pid ];
					negCorrect.data[ 0 ][ 0 ][ Pid ] = -1.f;
				}
			}
#if SHOWAVGERROR
			printf( "Weighted error rate: %f\n", error );
#endif
			/* record the best h */
			if ( error < bestError ) {
				copy( &posCorrect, &bestPosCorrect );
				copy( &negCorrect, &bestNegCorrect );
				bestError = error;
				bestBid = Bid;
				bestFid = Fid;
				bestParity = parity;
				bestDecision = decision;
			}
#if SHOWAVGERROR
			round++;
			avgError += error;
#endif

		} /* end of loop Fid */
	} /* end of loop Bid */

#if SHOWAVGERROR
	avgError /= round;
	printf( "Avg. weighted error rate: %f\n", avgError );
#endif
	printf( "\nBest weighted error rate: %f, Bid: %d, Fid: %d\n", bestError, bestBid, bestFid );
	/* make sure that best error rate < 0.5 (random guessing) */
	if ( bestError >= 0.5 ) {
		error( "addWeak(): Best error rate is above 0.5!" );
	}
#if 0
	full_dump( &bestPosCorrect, "bestPosCorrect", ALL, INT );
	full_dump( &bestNegCorrect, "bestNegCorrect", ALL, INT );
#endif

	/* [3] Record the best parameters, then determine the alpha weight for this weak classifier. */
	strong[ *hUsed ].Bid = bestBid;
	strong[ *hUsed ].Fid = bestFid;
	strong[ *hUsed ].parity = bestParity;
	strong[ *hUsed ].decision = bestDecision;
	alpha = logf( ( 1.f - bestError ) / bestError ) / 2.f;
	strong[ *hUsed ].alpha = alpha;
	printf( "alpha%d: %f\n", *hUsed, alpha );

	/* [4] Update the data weight */
	s_mul( &bestPosCorrect, -alpha );
	s_mul( &bestNegCorrect, -alpha );
	s_expRaise( &bestPosCorrect );
	s_expRaise( &bestNegCorrect );
	e_mul( &bestPosCorrect, posWeight, posWeight );
	e_mul( &bestNegCorrect, negWeight, negWeight );
	normalize = m_sum( posWeight ) + m_sum( negWeight );
	s_mul( posWeight, 1.f / normalize );
	s_mul( negWeight, 1.f / normalize );

	(*hUsed)++;

	/* free memory space */
	freeMatrix( &posCorrect ); freeMatrix( &negCorrect );
	freeMatrix( &bestPosCorrect ); freeMatrix( &bestNegCorrect );
}

/* learn AdaBoost stage A[i,j] */
void learnA( int posCount, int negCount, int blockCount, int *rejectCount, bool rejected[], 
	float ***POS, float ***NEG, Ada *H, float *F_current, float d_minA, float f_maxA ) {

	int selectTable[ posCount ]; /* image selection table */
	int imgCount = posCount * 2;
	float initialW = 1.f / (float)imgCount;
	Matrix posWeight, negWeight; /* data weight in AdaBoost */
	int hAllocated = 10;
	int hUsed = 0;
	int Pid, t;
    float f_local = 1.f;
	float threshold = 0.f; /* threshold of the strong classifier A[i,j], should be recorded */
	short negResultSign[ posCount ]; /* sign( negResult ) */

	ones( &posWeight, 1, posCount, 1 );
	ones( &negWeight, 1, posCount, 1 );
	H = (Ada *)malloc( hAllocated * sizeof( Ada ) );

	/* Initialize the data weight */
	s_mul( &posWeight, initialW );
	s_mul( &negWeight, initialW );
    /* [0] Randomly select NEG examples from the bootstrap set 
	 * Report an error if NEG images are not enough
	 */
	if ( posCount + (*rejectCount) > negCount ) {
		error( "learnA(): Not enough NEG images." );
	}
    select_neg( posCount, negCount, rejected, selectTable );

    while ( f_local > f_maxA ) {
		int fpCount; /* false positive count */
		float d_local = 0.f;

        /* [1] Add a weak learner h to the strong classifier A[i,j] */
		/* Use realloc() if H is full */
		if ( hUsed == hAllocated ) {
			hAllocated *= 2;
			printf( "call realloc()\n" );
			H = (Ada*)realloc( H, hAllocated * sizeof( Ada ) );
		}
        addWeak( posCount, negCount, blockCount, selectTable, 
			POS, NEG, &posWeight, &negWeight, H, &hUsed );


		threshold = 0.f; /*** Adjust by the threshold ONLY WHEN NECESSARY ***/
		while ( d_local < d_minA ) {
			float minPosResult = 0.f; /* minimum value of the positive result */
			int detect = 0; /* detection count */
			fpCount = 0;

			/* [2.1] Use the H(x) so far to trial-classify 
			 * H(x) = sign( sum[alpha_t * h_t(x)] )
			 * Process the POS and NEG together in one loop
			 */
#if 1
			printf( "threshold: %f\n", threshold );
#endif
			for ( Pid = 0; Pid < posCount; Pid++ ) {
				float posResult = 0.f;
				float negResult = 0.f;
				for ( t = 0; t < hUsed; t++ ) {
					int Bid = H[ t ].Bid;
					int Fid = H[ t ].Fid;
					short parity = H[ t ].parity;
					float decision = H[ t ].decision;
					float alpha = H[ t ].alpha;
					/* determine that positive is positive */
					if ( parity * POS[ Pid ][ Bid ][ Fid ] >= parity * decision ) {
						posResult += alpha;	
					}
					/* determine that positive is negative */
					else {
						posResult -= alpha;
					}
					/* determine that negative is positive */
					if ( parity * NEG[ selectTable[ Pid ] ][ Bid ][ Fid ] >= parity * decision ) {
						negResult += alpha;	
					}
					/* determine that negative is negative */
					else {
						negResult -= alpha;
					}
				} /* end of loop "t" */
			
				/* Adjust by the threshold */
				posResult -= threshold;
				negResult -= threshold;

				if ( posResult < minPosResult ) {
					minPosResult = posResult;
				}

#if 0
				printf( "POS[ %d ]: %f, NEG[ %d ]: %f\n", Pid, posResult, Pid, negResult );
#endif
				/* count detections and false positives */
				if ( posResult >= 0.f ) {
					detect++;
				}
				if ( negResult >= 0.f ) {
					fpCount++;
				}
				/* Record signs of all negResults for later rejection */
				negResultSign[ Pid ] = sign( negResult );

			} /* end of loop "Pid" */
		
			d_local = (float)detect / (float)posCount;
			printf( "Detection rate: %f ( %d / %d )\n", d_local, detect, posCount );

	        /* [2.2] Modify the threshold of H(x) to fulfill d_minA 
			 * Let result + min( posResult ) so that all POS data are
			 * determined as positive, i.e., detection rate = 100% 
			 */
			threshold = minPosResult;

		} /* end of loop "d_local < d_minA" */ 

        /* [3] Calculate f_local of H(x) */
		f_local = (float)fpCount / (float)posCount;
		printf( "False positive rate: %f ( %d / %d )\n", f_local, fpCount, posCount );

    } /* end of loop "f_local > f_maxA" */

    *F_current *= f_local;
	printf( "\nWeak learners used: %d\n", hUsed );
    printf( "Overall false positive rate: %f\n", *F_current );
	getchar();

	/* [4] Put in the "rejection list" the negative images rejected by H(x) */
	for ( Pid = 0; Pid < posCount; Pid++ ) {
		if ( negResultSign[ Pid ] < 0.f ) {
			rejected[ selectTable[ Pid ] ] = true;
			(*rejectCount)++;
		}
	}

	/* [5] Output H(x) information to file */
	freeMatrix( &posWeight );
	freeMatrix( &negWeight );
	free( H );
}

/* Learn Meta stage M[i] */
void learnM( int i, int posCount, int negCount, bool rejected[], float *F_current, float d_minM ) {
    printf( "Learning stage M[%d]...\n", i );
}



