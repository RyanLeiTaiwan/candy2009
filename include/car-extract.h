/** File: car-extract.h
 ** Author: Ryan Lei
 ** Creation: 2009/09/11
 ** Modification: 2009/09/27
 ** Description: Function prototypes of the car feature extraction
 **/
#include "image.h"
/* feature-ralated constants */
#define FEATURE_COUNT 15
#define REC_BEGIN 0
#define EOH_BEGIN 5
#define ED_BEGIN 14
/* Some self-defined feature extraction parameters: */
#define d_width 128 /* detection window width */
#define d_height 128 /* detection window height */
#define b_size_min 8 /* minumum block size */
#define b_size_step 8 /* block size increment */
#define b_pos_step 8 /* block position increment */

/* feature extraction of a whole directory */
void extract_all_images( char *directory, float ****POOL, int *imgCount, int *blockCount );
/* feature extraction of a single image */
void extract_image( char *fileName, int Iid, float ***POOL ); 
/* feature extraction of a single block */
void extract_block( int Iid, int Bid, float ***POOL, Matrix *ii_norm, Matrix *ii_EOH,
	Matrix *ii_ED, int rowBeg, int colBeg, int rowEnd, int colEnd );
/* count # of images in the directory */
int count_images( char *fileName, int pathLen );
/* count # of blocks in the window */
int count_blocks();

