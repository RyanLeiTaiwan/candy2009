/** File: image.c
 ** Author: ryanlei
 ** Creation: 2009/03/29
 ** Modification: 2009/04/01
 ** Description: Implementation of image operations.
                 Handled in "Little-Endian".
 **/
#include "image.h"
#define DEBUG 0
 
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

void imwrite( char *filename, Matrix *dest ){
    FILE          *fp_d = NULL; //destination
    BMP_file_header file_header;
    BMP_info_header info_header;
	BMP_palette *palette=NULL;
	int pad_data_size;
	
	fp_d = fopen(filename, "wb");
    if (fp_d == NULL) {
        error("imwrite:file error");
    }
    file_header.ID=0x4D42;  //bmp
	file_header.reserved1=0;
	file_header.reserved2=0;
	info_header.info_size=40;
	info_header.height= -(dest->size1);  //height
	info_header.width=dest->size2;       //width-
	info_header.planes=1;
	info_header.bits_per_pixel=dest->size3 * 8;  //bits per pixel
	//file_header.data_offset=54
	pad_data_size = info_header.width * info_header.bits_per_pixel / 8;
	pad_data_size = ( pad_data_size % 4 == 0 )? pad_data_size:pad_data_size+( 4 - pad_data_size % 4 );
	pad_data_size *= abs( info_header.height );
	info_header.compression=0;
	info_header.data_size=0;
	info_header.H_resolution=0;
	info_header.V_resolution=0;
	info_header.used_colors=0;////
	info_header.important_colors=0;				
	if(info_header.bits_per_pixel==24){
		file_header.file_size=pad_data_size+54;
		file_header.data_offset=54;
		
	}
	else if(info_header.bits_per_pixel==8){//palette
		file_header.data_offset=54+1024;
		file_header.file_size=pad_data_size+1024+54;  //1024 for palette
		/* malloc and generate BMP palette */
	//	palete=malloc()
		/* genPalette( BPT, GRAY/RED ); */
			
	}		
    //fwrite();  //header
   // fwrite(dest,fp_d)
    fclose(fp_d);


}
void color2Gray( Matrix *source ){ // dest is source itself
    int col,row,layer;

     for( row=0; row<source->size1; row++){
        for ( col = 0; col < source->size2; col++ ) {
            for(layer=1;layer<source->size3;layer++){
            	source->data[ 0 ][ row ][ col ] = source->data[ layer ][ row ][ col ]+ source->data[ 0 ][ row ][ col ] ;
            	free(source->data[layer][row]); //not sure...
			}
            source->data[ 0 ][ row ][ col ] =source->data[ 0 ][ row ][ col ] /3;
        	free(source->data[layer]);
		}
     }
	 source->size1=1;
}
void gray2Color( Matrix *source ){ // dest is source itself
    int col,row,layer;
	source->size1=3; 
    for(layer=1;layer<source->size3;layer++){
        source->data[layer]=(float **) malloc( size1 * sizeof( float *) );
		for( row=0; row<source->size1; row++){
			source->data[ layer ][ row ] = (float *) malloc( size2 * sizeof( float ) );
            for ( col = 0; col < source->size2; col++ ) {
                source->data[layer][row][col]=source->data[0][row][col];
            }
        }
    }


}
#if DEBUG
int main(){
    Matrix A;
    clock_t tic, toc;

	tic = clock();

    imread(  "pics/huge.bmp", &A );
    color2Gray( &A  );	
 //   dump( &A, "A", RR, 0, 15, 16, 31, INT );
    toc = clock();
	/* 使用runningTime()來印計時結果 */
	runningTime( tic, toc );
    //dump( &A, "A", GG, INT );
    //dump( &A, "A", RR, INT );
    return 0;


}

#endif
