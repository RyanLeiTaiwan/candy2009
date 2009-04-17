/** File: image.c
 ** Author: ryanlei snowm
 ** Creation: 2009/03/29
 ** Modification: 2009/04/07
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

	/* message upon success */
	printf( "imread( %s ): success!.\n", filename );
	printf( "File size: %d bytes; ", BFH.file_size );
	printf( "Height: %d; Width: %d.\n", abs( BIH.height ), BIH.width );

	free( tempData );
	fclose( fin );
}

void imwrite( char *filename, Matrix *source, w_mode mode ) {
    BMP_file_header file_header;
    BMP_info_header info_header;
    BMP_palette *palette = NULL;
    int pad_data_size;
    int row, col, layer;
    int byteCount = 0;
    int giveUp; /* # of bytes of padding to give up at the end of each row */
    int ocount = 0;
    uint8 *tempData;
    FILE *fp = fopen( filename, "w" );

	if ( fp == NULL ) {
		error( "imwrite(): File open error." );
	}

	file_header.ID = 0x4D42 ;  /* BMP id */
	file_header.reserved1 = 0;
	file_header.reserved2 = 0;

	info_header.info_size = 40;
	info_header.width = source->size2;       //width
	info_header.height = -( source->size1 );  //height, negative
	info_header.planes = 1;
	info_header.bits_per_pixel = source->size3 * 8;  //bits per pixel
	info_header.compression = 0;
	/* compute padded data size */
	pad_data_size = info_header.width * info_header.bits_per_pixel / 8;
	pad_data_size = ( pad_data_size % 4 == 0 ) ? pad_data_size : 
		pad_data_size + ( 4 - pad_data_size % 4 );
	pad_data_size *= -( info_header.height );
	info_header.data_size = pad_data_size;
	info_header.H_resolution = 0;
	info_header.V_resolution = 0;
	info_header.used_colors = 0; /* ryanlei: 0 is correct */
	info_header.important_colors = 0;
	/* compute data offset & total file size */	
	/* 只有兩種情況：24-bit 全彩，8-bit 256色 */
	if ( info_header.bits_per_pixel == 24 ) {
	    file_header.data_offset = 54;
	    file_header.file_size = pad_data_size + 54;
    }
    else if ( info_header.bits_per_pixel == 8 ) {
        file_header.data_offset = 1078; /* 54 + 4 * 256 = 1078 */
        file_header.file_size = 1078 + pad_data_size;  //1024 for palette
        /* malloc and generate BMP palette */
		/* ryanlei: 這邊要cast成BMP_palette*，之前轉成uint8*是錯的 */
        palette = (BMP_palette *) malloc( 256 * sizeof( BMP_palette ) );
		/* genPalette( BPT, GRAY/RED ); */
    }

#if DEBUG
	printf( "**imwrite( '%s' ):\n", filename );
	printf( "file_size: %d\n", file_header.file_size );
	printf( "data_offset: %d\n", file_header.data_offset );
	printf( "width: %d\n", info_header.width );
	printf( "height: %d\n", info_header.height );
	printf( "bits_per_pixel: %d\n", info_header.bits_per_pixel );
	printf( "padded_data_size = %d\n", pad_data_size );
	getchar();
#endif

    fwrite( &file_header.ID, 2, 1, fp );
    fwrite( &file_header.file_size, 12, 1, fp );
    fwrite( &info_header, sizeof( info_header ), 1, fp);
    /*if(palette!='NULL'){
        fwrite(&palette, 256*sizeof(BMP_palette),1, fp);
    }*/
    tempData = (uint8 *) malloc( sizeof ( uint8 ) * pad_data_size );
    giveUp = source->size2 * info_header.bits_per_pixel / 8;
	giveUp = (giveUp % 4 == 0) ? 0 : 4 - giveUp % 4;

    /* write the raw data into 1D array */
    for ( row = 0; row < source->size1; row ++ ) {
		for ( col = 0; col < source->size2; col++ ) {

			for ( layer = 0; layer < source->size3; layer++ ) {
				tempData[ byteCount++ ]= source->data[ layer ][ row ][ col ];
			}

			/** adjust for padding at the end of each row **/
			if ( col == source->size2 - 1 ) {
			    while(ocount<giveUp){
			        tempData[byteCount++]=0;
			        ocount++;
                }
				ocount=0;
			}
		}
	}
	/*write the tempData into the file*/
	fwrite(&tempData, pad_data_size*sizeof(uint8),sizeof ( uint8 ), fp);

	/* message upon success */
	printf( "imwrite( %s ): success!.\n", filename );
	printf( "File size: %d bytes; ", file_header.file_size );
	printf( "Height: %d; Width: %d.\n", -( info_header.height ), info_header.width );

	free( tempData );
    fclose( fp );
}
/*void genPalette(){


}*/
void color2Gray( Matrix *source ) {
    int col,row,layer;
	/* check layer */
	if ( source->size3 != 3 ) {
		error( "color2Gray(): Source is not color image " );
	}

	/* ryanlei: 拆掉一個迴圈 */
    for( row = 0; row < source->size1; row++ ){
        for ( col = 0; col < source->size2; col++ ) {
            source->data[ 0 ][ row ][ col ] = 
				source->data[ 0 ][ row ][ col ] + 
				source->data[ 1 ][ row ][ col ] +
				source->data[ 2 ][ row ][ col ];
			source->data[ 0 ][ row ][ col ] /= 3;
		}
    }

	/* free memory */
    for( layer = 1; layer <= 2; layer++ ){
        for ( row = 0; row < source->size1; row++ ) {
        	free( source->data[ layer ][ row ] );
        }
    	free( source->data[ layer ] );
    }
    source->size3=1;
}

void gray2Color( Matrix *source ) {
    int col, row, layer;
	/* check layer */
	if ( source->size3 != 1 ) {
		error( "gray2Color(): Source is not grayscale image " );
	}

	source->size3 = 3;
	/* ryanlei: Yes, it is "float" */
    for( layer = 1; layer<source->size3; layer++ ) {
        source->data[layer]=(float **) malloc( source->size1 * sizeof( float * ) );
		for( row = 0; row<source->size1; row++) {
			source->data[ layer ][ row ] = (float *) malloc( source->size2 * sizeof( float ) );
            for ( col = 0; col < source->size2; col++ ) {
                source->data[ layer ][ row ][ col ] = source->data[ 0 ][ row ][ col ];
            }
        }
    }
}

#if DEBUG
int main() {
	Matrix img, img2;
	clock_t tic, toc;
	tic = clock();
	imread( "pics/paint_15.bmp", &img );
	//dump( &img, "img(color)", ALL, 0, img.size1-1, 0, img.size2-1, INT );
	//color2Gray( &img );
	//dump( &img, "img(gray)", ALL, 0, img.size1-1, 0, img.size2-1, INT );
	//gray2Color( &img );
	//dump( &img, "img(gray)", ALL, 0, img.size1-1, 0, img.size2-1, INT );
	imwrite("pics/p15out.bmp", &img, RGB );
	imread( "pics/p15out.bmp", &img2 ); /* read back */
	toc = clock();
	runningTime( tic, toc );
	return 0;
}
#endif
