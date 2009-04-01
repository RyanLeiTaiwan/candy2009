/** File: image.h
 ** Author: snowm, ryanlei
 ** Creation: 2009/03/29
 ** Modification: 2009/04/01
 ** Description: Image header and read/write, conversion operations.
                 Handled in "Little-Endian".
 **/
#include "image.h"
#include "util.h"

#define DEBUG 0


void imread( char *filename, Matrix *dest ){  // dest is not yet allocted
    FILE *fp_s = NULL;  // source file handler
    BMP_file_header file_header;
    BMP_info_header info_header;
    uint8 *image_s = NULL; // temp one-dimension array to save
    int col,row,layer,ine;
    //dest=NULL;

    fp_s = fopen(filename, "rb");
    if (fp_s == NULL) {
        /* 請愛用自己寫的error: stderr兼跳出程式 */
        error( "imread(): FIle open error." );
    }
    fseek(fp_s, 2, SEEK_SET);
    fread(&file_header.file_size, sizeof(uint32), 1, fp_s);
    fseek(fp_s, 10, SEEK_SET);
    fread(&file_header.data_offset, sizeof(uint32), 1, fp_s);
    fseek(fp_s, 18, SEEK_SET);
    fread(&info_header.width, sizeof(uint32), 1, fp_s);
    fseek(fp_s, 22, SEEK_SET);
    fread(&info_header.height, sizeof(int32), 1, fp_s);
    fseek(fp_s, 28, SEEK_SET);
    fread(&info_header.bits_per_pixel, sizeof(uint16), 1, fp_s);

/* messages of debugging header */
#if DEBUG
    printf( "**imread( '%s' ):\n", filename );
    printf( "file_size: %d\n", file_header.file_size );
    printf( "data_offset: %d\n", file_header.data_offset );
    printf( "width: %d\n", info_header.width );
    printf( "height: %d\n", info_header.height );
    printf( "bits_per_pixel: %d\n", info_header.bits_per_pixel );
#endif

    /* 用現成的zeros來malloc就好 */
    zeros( dest, abs( info_header.height ), info_header.width, info_header.bits_per_pixel / 8 );
    fseek(fp_s, file_header.data_offset, SEEK_SET);
    image_s = (unsigned char *)malloc(info_header.width * abs(info_header.height) * 3);
    if (image_s == NULL) {
        error("malloc images_s error\n");
    }
    fread(image_s, sizeof(unsigned char), (size_t)(long)info_header.width * abs( info_header.height ) * 3, fp_s);

    for(layer=0;layer<3;layer++){
        ine=layer;
        for(row=0;row<dest->size1;row++){
            dest->data[layer][row]=malloc( dest->size2 * sizeof( char ) );

            for ( col = 0; col < dest->size2; col++ ) {
				dest->data[ layer ][ row ][ col ] = image_s[ine];
				ine=ine+3;
            }
        }

    }

    fclose(fp_s);
    return;
}

void imwrite( char *filename, Matrix *dest ){


}
void color2Gray( Matrix *source ){ // dest is source itself

}
void gray2Color( Matrix *source ){ // dest is source itself


}
#if DEBUG
int main(){
    Matrix A;
	clock_t tic, toc;
	tic = clock();

    imread( "pics/paint_16.bmp", &A );
    dump( &A, "A", ALL, 0, A.size1-1, 0, A.size2-1, INT );

	toc = clock();
	/* 使用runningTime()來印計時結果 */
	runningTime( tic, toc );
    return 0;


}

#endif
