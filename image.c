/** File: image.c
 ** Author: Ryan Lei
 ** Creation: 2009/03/29
 ** Modification: 2009/04/01
 ** Description: Implementation of image operations.
                 Handled in "Little-Endian".
 **/
#include "image.h"
#define DEBUG 1
 
/*** Important Note:
 *** I safely assume that all images are uncompressed BMPs, 24-bit color or 
 *** 8-bit grayscale, so do NOT use this if you wish to handle BMP files
 *** in all encoding modes.
 ***/
void imread( char *filename, Matrix *dest ) {
	BMP_file_header BFH;
	BMP_info_header BIH;
	int row, row_begin, row_end, row_change, col, layer, layer_begin;
	int pad_data_size; /* raw data size in bytes, including the padding */
	uint8 *tempData;
	int byteCount = 0;
	int giveUp; /* # of bytes of padding to give up at the end of each row */

	/* read the whole 54-byte header */
	FILE *fin = fopen( filename, "r" );
	if ( fin == NULL ) {
		error( "imread(): File open error." );
	}
	fread( &BFH.ID, 2, 1, fin );
	if ( BFH.ID != 0x4D42 ) {
		error( "imread(): Not a BMP image." );
	}
	fread( &BFH.file_size, 12, 1, fin );
	fread( &BIH, sizeof( BMP_info_header ), 1, fin );

	/** check for cases we can't handle **/
	if ( BIH.bits_per_pixel != 24 && BIH.bits_per_pixel != 8 ) {
		error( "imread(): Only accept 24-bit or 8-bit BMP." );
	}
	if ( BIH.compression != 0 ) {
		error( "imread(): Only accept uncompressed BMP." );
	}

	/* Now it's the BMP we CAN handle. */
	/* memory allocation and set dimensions */
	zeros( dest, abs( BIH.height ), BIH.width, BIH.bits_per_pixel / 8 );
	/* beginning layer for filling dest->data */
	layer_begin = BIH.bits_per_pixel / 8 - 1;

	/*** the data part ***/
	/** consider the padding that the size in each row must be a multiple of 4 **/
	pad_data_size = BIH.width * BIH.bits_per_pixel / 8;
	pad_data_size = ( pad_data_size % 4 == 0 ) ? pad_data_size : 
		pad_data_size + ( 4 - pad_data_size % 4 );
	pad_data_size *= abs( BIH.height );

#if DEBUG
	printf( "**imread( '%s' ):\n", filename );
	printf( "file_size: %d\n", BFH.file_size );
	printf( "data_offset: %d\n", BFH.data_offset );
	printf( "width: %d\n", BIH.width );
	printf( "height: %d\n", BIH.height );
	printf( "bits_per_pixel: %d\n", BIH.bits_per_pixel );
	printf( "padded_data_size = %d\n", pad_data_size );
	getchar();
#endif

	/* read the whole data first, which come in BGR order */
	tempData = (uint8 *) malloc( sizeof ( uint8 ) * pad_data_size );
	fseek( fin, BFH.data_offset, SEEK_SET );
	fread( tempData, sizeof( uint8 ), pad_data_size, fin );

	/* check if height is positive/negative */
	if ( BIH.height > 0 ) {
		row_begin = BIH.height - 1;
		row_end = -1; /* trick */
		row_change = -1;
	}
	else if ( BIH.height < 0 ) {
		row_begin = 0;
		row_end = -BIH.height; /* trick */
		row_change = 1;
	}
	else {
		error( "imread(): height must not be 0." );
	}

	/* fill the data in tempData to dest->data */
	/* first, calculate the padding for each row */
	giveUp = dest->size2 * BIH.bits_per_pixel / 8;
	giveUp = (giveUp % 4 == 0) ? 0 : 4 - giveUp % 4;

	/* note the trick of termination condition */
	for ( row = row_begin; row != row_end; row += row_change ) {
		for ( col = 0; col < dest->size2; col++ ) {

			for ( layer = layer_begin; layer >= 0; layer-- ) {
				dest->data[ layer ][ row ][ col ] = tempData[ byteCount++ ];
			}

			/** adjust for padding at the end of each row **/
			if ( col == dest->size2 - 1 ) {
				byteCount += giveUp;
			}
		}
	}

	free( tempData );
	fclose( fin );
}

void imwrite( char *filename, Matrix *dest ) {

}

void color2Gray( Matrix *source ) {

}

void gray2Color( Matrix *source ) {

}

#if DEBUG
int main() {
	Matrix img;
	clock_t tic, toc;
	tic = clock();
	imread( "pics/huge.bmp", &img );
	//dump( &img, "img", ALL, 10, 29, 20, 29, INT );
	//dump( &img, "img", ALL, 0, img.size1-1, 0, img.size2-1, INT );
	toc = clock();
	runningTime( tic, toc );
	return 0;
}
#endif
