/** File: car-train.h
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/16
 ** Description: Function prototypes of the car-training program
 **/
#include "image.h"

/*** data structure of a block feature ***/
typedef struct {
	float REC[ 5 ]; /* 5 rectangle features => actually 10 */
	float EOH[ 9 ]; /* 9 EOH features */
	float ED;
} Feature;

/* learn AdaBoost stage A[i,j] */
void learnA( int i, int j, float *F_current, float d_minA, float f_maxA );
/* learn Meta stage M[i] */
void learnM( int i, float *F_current, float d_minM );
/* randomly select neg examples from the bootstrap */
void select_neg(); 
/* select a weak learner to the strong classifier */
void weak_select();
/* count # of images in the directory */
int count_images( char *fileName, int pathLen ); 
/* feature extraction of a whole directory */
void extract_all_images( char *directory, Feature ***POOL );
/* count # of blocks in the window */
int count_blocks( int d_width, int d_height, int b_size_min, int b_size_step, int b_pos_step ); 
/* feature extraction of a single image */
void extract_image( char *fileName, int Iid, Feature ***POOL, int d_width, int d_height,
	int b_size_min, int b_size_step, int b_pos_step ); 
/* feature extraction of a single block */
void extract_block( int Iid, int Bid, Feature ***POOL, Matrix *ii_norm,
	Matrix *ii_ED, int rowBeg, int colBeg, int rowEnd, int colEnd );


