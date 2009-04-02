<<<<<<< .mine
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
    FILE          *fp_s = NULL;  // source file handler
    BMP_file_header file_header;
    BMP_info_header info_header;
    uint8 *image_s = NULL; // temp one-dimension array to save
    int col,row,layer,ivalue,i;
    int mod4width,ine,hivalue,temp;
    int pad_data_size;
    unsigned char headertemp[54];
    int *record_ine=NULL;
    //dest=NULL;
    hivalue=1; //assuming the height value is positive
    mod4width=0; //asuming width mod 4=0
    fp_s = fopen(filename, "rb");
    if (fp_s == NULL) {
        /* 請愛用自己寫的error: stderr兼跳出程式 */
        error( "imread(): FIle open error." );
    }
    fread(&file_header.ID,sizeof(uint16), 1, fp_s);

   // fread(&file_header,sizeof(BMP_file_header), 1, fp_s);

    //fseek(fp_s,sizeof(BMP_file_header),SEEK_SET);
    if(file_header.ID!=0x4D42){
        error("imread(): Not a BMP image.");
    }
    fread(&file_header.file_size,sizeof(uint32), 1, fp_s);
    fread(&file_header.reserved1,sizeof(uint16), 1, fp_s);
    fread(&file_header.reserved2,sizeof(uint16), 1, fp_s);
    fread(&file_header.data_offset,sizeof(uint32), 1, fp_s);

    fread(&info_header,sizeof(BMP_info_header), 1, fp_s);

    //fread(&headertemp,sizeof(headertemp), 1, fp_s);
    //fseek(fp_s, 2, SEEK_SET);
    //fread(&file_header.file_size, sizeof(uint32), 1, fp_s);
    //fseek(fp_s, 10, SEEK_SET);
    //fread(&file_header.data_offset, sizeof(uint32), 1, fp_s);
   // fseek(fp_s, 18, SEEK_SET);
    //fread(&info_header.width, sizeof(uint32), 1, fp_s);
    //fseek(fp_s, 22, SEEK_SET);
    //fread(&info_header.height, sizeof(int32), 1, fp_s);
    //fseek(fp_s, 28, SEEK_SET);
    //fread(&info_header.bits_per_pixel, sizeof(uint16), 1, fp_s);
   // for(i=0;i<=53;i++){
      // printf("header[%d]=%d ",i, headertemp[i] );
    //    }
   // file_header.file_size=headertemp[2]+headertemp[3]*16+headertemp[4]*256+headertemp[5]*4096;
    //file_header.data_offset=headertemp[10]+headertemp[11]*16+headertemp[12]*256+headertemp[13]*4096;
    printf("off=%d",file_header.data_offset);
   /* if(file_header.data_offset!=54){
        error("the form is not correct.");
        }*/
  //  info_header.width=headertemp[18]+headertemp[19]*16+headertemp[20]*256+headertemp[21]*4096;
    if(info_header.bits_per_pixel!=24 && info_header.bits_per_pixel!=8){
        error("the bits per pixel is not 24 or 8.");
        }
    if(info_header.width%4!=0 ){
        mod4width=4-((info_header.width*info_header.bits_per_pixel/8)%4);
        printf(" mod4w=%d\n",mod4width);
       // info_header.width=info_header.width+mod4width;
        }
    if(info_header.height<0){
        hivalue=0;
        info_header.height=abs(info_header.height);
        }
    /*if(headertemp[25]=255){////
        temp=headertemp[22]+ headertemp[23]*16+ headertemp[24]*256+ headertemp[25]*4096;
        info_header.height =((headertemp[22]^255)+(headertemp[23]^255)*16+(headertemp[24]^255)*256+(headertemp[25]^255)*4096)+1  ;
        //headertemp[22]=(headertemp[22])^0;
        //info_header.height =abs(headertemp[22])+abs(headertemp[23])*16+abs(headertemp[24])*256+abs(headertemp[25])*4096;
        printf("22:%d,hi= %d\n",headertemp[23],info_header.height);

        hivalue=0;
                }*/

   // info_header.bits_per_pixel=headertemp[28]+headertemp[29]*16;////24
    //printf("xx=%d",info_header.bits_per_pixel);

pad_data_size=info_header.height*mod4width+info_header.height*info_header.width;
/* messages of debugging header */
#if DEBUG
    printf( "**imread( '%s' ):\n", filename );
    printf( "file_size: %d\n", file_header.file_size );
    printf( "data_offset: %d\n", file_header.data_offset );
    printf( "width: %d\n", info_header.width );
    printf( "height: %d\n", info_header.height );
    printf( "bits_per_pixel: %d\n", info_header.bits_per_pixel );
   // printf( "padded_data_size = %d\n", pad_data_size );

#endif

    /* 用現成的zeros來malloc就好 */
    zeros( dest, abs( info_header.height ), info_header.width, info_header.bits_per_pixel / 8 );

    fseek(fp_s, file_header.data_offset, SEEK_SET);

    image_s = (unsigned char *)malloc(info_header.width * abs(info_header.height) * 3);
    record_ine=(int*)malloc(info_header.height * mod4width );
    if (image_s == NULL) {
        error("malloc images_s error\n");
    }

    fread(image_s, sizeof(unsigned char), (size_t)(long)info_header.width * abs( info_header.height ) * 3, fp_s);
    //ivalue=0;
        /*if(mod4width==1 && info_header.bits_per_pixel==24){
            ine=info_header.width*info_header.bits_per_pixel/8;

            while(ine<3*info_header.width *info_header.height){
                    record_ine[ivalue]=ine;
                    ivalue++;
                    printf("i=%d ",ine);
                    ine=ine+info_header.width*info_header.bits_per_pixel/8+1;
            }
        }
        else if(mod4width==2 && info_header.bits_per_pixel==24){
             ine=info_header.width*info_header.bits_per_pixel/8;
             while(ine<3*info_header.width *info_header.height){
                    record_ine[ivalue]=ine;
                    record_ine[ivalue+1]=ine+1;
                    ivalue=ivalue+2;
                    printf("i=%d ",ine);
                    ine=ine+info_header.width*(info_header.bits_per_pixel/8)+2;
             }
        }
        else if(mod4width==3 && info_header.bits_per_pixel==24){
                ine=info_header.width*info_header.bits_per_pixel/8;
                while(ine<3*info_header.width *info_header.height){

                        record_ine[ivalue]=ine;
                        record_ine[ivalue+1]=ine+1;
                        record_ine[ivalue+2]=ine+2;
                        printf("v:%d,%d,%d ",record_ine[ivalue],record_ine[ivalue+1],record_ine[ivalue+2]);
                        ivalue=ivalue+3;
                        printf("i=%d ",ine);
                        //printf("value:%d ,%d ,%d",image_s[ine],image_s[ine+1],image_s[ine+2]);

                        ine=ine+info_header.width*(info_header.bits_per_pixel/8)+3;

                }

        }*/


    if(info_header.bits_per_pixel==24){

        if(hivalue==0){  //height is negative
            for(layer=0;layer<info_header.bits_per_pixel/8 ;layer++){
                i=info_header.width*3;
                if(layer==0){
                    ine=2;
                    }
                else if(layer==2){
                    ine=0;
                    }
                else {ine=1;}

                for(row=0;row<dest->size1;row++){
                    for ( col = 0; col < dest->size2; col++ ) {
                        dest->data[ layer][ row ][ col ] = image_s[ine];
                       // printf("im_s=%d ",image_s[ine]);
                        ine=ine+3;
                           if(mod4width==1){

                                    if(ine==i || ine==i+1 || ine==i+2){
                                       // printf("i+3:%d",ine);
                                        ine++;
                                        i=i+info_header.width*3+mod4width;
                                    }

                           }
                            else if(mod4width==2){

                                    if(ine==i || ine==i+1 || ine==ine+2){
                                       // printf("i+2:%d",ine);
                                        ine=ine+2;
                                        i=i+info_header.width*3+mod4width;
                                        }


                                }
                            else if(mod4width==3){

                                    if(ine==i || ine==i+1 || ine==i+2){
                                      //  printf("i+1:%d",ine);
                                        ine=ine+3;
                                        i=i+info_header.width*3+mod4width;
                                    }



                            }

                    }
                }

            }
        }
        else {  //height is positive
            for(layer=0;layer<info_header.bits_per_pixel/8 ;layer++){
                i=info_header.width*3;
                if(layer==0){
                    ine=2;
                    }
                else if(layer==2){
                    ine=0;
                    }
                else {ine=1;}
                for(row=dest->size1-1;row>=0;row--){
                    for ( col = 0; col < dest->size2; col++ ) {
                        dest->data[ layer ][ row ][ col ] = image_s[ine];

                       // printf("%d=%d  ",ine,image_s[ine]);
                        ine=ine+3;

                            if(mod4width==1){

                                   if(ine==i || ine==i+1 || ine==i+2){
                                       // printf("i+3:%d",ine);
                                        ine++;
                                        i=i+info_header.width*3+mod4width;
                                    }


                            }
                            else if(mod4width==2){
                                  if(ine==i || ine==i+1 || ine==ine+2){
                                       // printf("i+2:%d",ine);
                                        ine=ine+2;
                                        i=i+info_header.width*3+mod4width;
                                        }
                            }
                            else if(mod4width==3){
                                if(ine==i || ine==i+1 || ine==i+2){
                                       // printf("X %d ",image_s[ine]);
                                        ine=ine+3;
                                        i=i+info_header.width*3+mod4width;
                                    }
                                   /* else if(ine==i+1){
                                         printf("i+1=%d",i+1);
                                        ine=ine+3;
                                        i=i+info_header.width*3+mod4width;
                                    }
                                    else if(ine==i+2){
                                         printf("i+2=%d",i+2);
                                        ine=ine+3;
                                        i=i+info_header.width*3+mod4width;
                                    }*/
                            }

                    }
                }
            }

        }
    }
    else if(info_header.bits_per_pixel==8){

        /*for ( layer = 1; layer < 3; layer++ ) {
            for ( row = 0; row < dest->size1; row++ ) {
                free( dest->data[ layer ][ row ] );
            }
            free( dest->data[ layer ] );
        }*/

        layer=0;
        ine=0;
        i=info_header.width;
        if(hivalue==0){  //height is negative

            for(row=0;row<dest->size1;row++){
                for ( col = 0; col < dest->size2; col++ ) {
                    dest->data[ layer][ row ][ col ] = image_s[ine];
                    //printf("di=%d ",image_s[ine]);
                    ine++;
                    if(mod4width==1){
                        if(ine==i) {
                            //printf("==");
                            ine++;
                            i=i+info_header.width+mod4width;
                         }
                    }
                    else if(mod4width==2){
                            if(ine==i){
                                ine=ine+2;
                                i=i+info_header.width+mod4width;
                            }
                    }
                    else if(mod4width==3){
                            if(ine==i){
                                ine=ine+3;
                                i=i+info_header.width+mod4width;
                            }

                    }
                }
            }
        }


        else {  //height is positive
            printf("p");
            for(row=dest->size1-1;row>=0;row--){
                for ( col = 0; col < dest->size2; col++ ) {
                    dest->data[ layer ][ row ][ col ] = image_s[ine];
                    //printf("di=%d ",image_s[ine]);
                    ine++;
                   if(mod4width==1){
                        if(ine==i) {
                            //printf("==");
                            ine++;
                           // printf(" ine=%d ",ine);
                            i=i+info_header.width+mod4width;
                         }
                    }
                    else if(mod4width==2){
                            if(ine==i){
                                ine=ine+2;
                                i=i+info_header.width+mod4width;
                            }
                    }
                    else if(mod4width==3){
                            if(ine==i){
                                ine=ine+3;
                                i=i+info_header.width+mod4width;
                            }

                    }
                }
            }

        }

    }
    fclose(fp_s);
    return;
}

void imwrite( char *filename, Matrix *dest ){
    FILE          *fp_d = NULL; //destination


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

    imread(  "pics/paint_61.bmp", &A );
    dump( &A, "A", RR, 0, 15, 16, 31, INT );
    toc = clock();
	/* 使用runningTime()來印計時結果 */
	runningTime( tic, toc );
    //dump( &A, "A", GG, INT );
    //dump( &A, "A", RR, INT );
    return 0;


}

#endif
=======
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
>>>>>>> .r33
