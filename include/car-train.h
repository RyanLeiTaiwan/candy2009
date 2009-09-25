/** File: car-train.h
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/23
 ** Description: Function prototypes of the car-training program
 **/
#include "image.h"
/* feature-ralated constants */
#define FEATURE_COUNT 15
#define REC_BEGIN 0
#define EOH_BEGIN 5
#define ED_BEGIN 14

typedef struct {
	int Bid;
	int Fid;
	short parity;
	float decision; /* classification threshold */
	float alpha; /* the weight of this weak classifier */
} Ada;

/* learn an AdaBoost stage */
void learnA( int posCount, int negCount, int blockCount, int *rejectCount, bool rejected[], 
	float ***POS, float ***NEG, Ada *strong, float *F_current, float d_minA, float f_maxA );
/* learn Meta stage M[i] */
void learnM( int i, int posCount, int negCount, bool rejected[], float *F_current, float d_minM );
/* randomly select neg examples from the bootstrap */
void select_neg( int posCount, int negCount, bool rejected[], int selectTable[] ); 
/* Add a weak learner to the strong classifier */
void addWeak( int posCount, int negCount, int blockCount, int selectTable[],
	float ***POS, float ***NEG, Matrix *posWeight, Matrix *negWeight, Ada *strong, int *hUsed );
/* count # of images in the directory */
int count_images( char *fileName, int pathLen ); 
/* feature extraction of a whole directory */
void extract_all_images( char *directory, float ****POOL, int *imgCount, int *blockCount );
/* count # of blocks in the window */
int count_blocks( int d_width, int d_height, int b_size_min, int b_size_step, int b_pos_step ); 
/* feature extraction of a single image */
void extract_image( char *fileName, int Iid, float ***POOL, int d_width, int d_height,
	int b_size_min, int b_size_step, int b_pos_step ); 
/* feature extraction of a single block */
void extract_block( int Iid, int Bid, float ***POOL, Matrix *ii_norm, Matrix *ii_EOH,
	Matrix *ii_ED, int rowBeg, int colBeg, int rowEnd, int colEnd );


