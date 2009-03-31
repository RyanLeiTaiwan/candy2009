
#include "image.h"
#include "util.h"

#define DEBUG 0


int imread( char *filename, Matrix *dest ){  // dest is not yet allocted
    FILE          *fp_s = NULL;  // source file handler
    BMP_file_header file_header;
    BMP_info_header info_header;
    unsigned char *image_s = NULL; // temp one-dimension array to save
    int col,row,layer,ine;
    //dest=NULL;

    fp_s = fopen(filename, "rb");
     if (fp_s == NULL) {
      printf("fopen fp_s error\n");
      return -1;
    }
    fseek(fp_s, 10, SEEK_SET);
    fread(&file_header.data_offset, sizeof(unsigned int), 1, fp_s);
    //printf("offset= %d",file_header.data_offset);
    fseek(fp_s, 18, SEEK_SET);
    fread(&info_header.width, sizeof(int), 1, fp_s);
    //printf("width=%d",info_header.width);
    fseek(fp_s, 22, SEEK_SET);
    fread(&info_header.height, sizeof(int), 1, fp_s);
    // printf("hei=%d",info_header.height);
    fseek(fp_s, 28, SEEK_SET);
    fread(&info_header.bits_per_pixel, sizeof(unsigned short), 1, fp_s);
    //printf("%d",info_header.bits_per_pixel);
    dest->size1=info_header.height;
    dest->size2=info_header.width;
    dest->size3=3;
    dest->data=(char***)malloc(3*sizeof(char**));
    fseek(fp_s, file_header.data_offset, SEEK_SET);
    image_s = (unsigned char *)malloc((size_t)info_header.width * info_header.height * 3);
    if (image_s == NULL) {
      printf("malloc images_s error\n");
      return -1;
    }
    fread(image_s, sizeof(unsigned char), (size_t)(long)info_header.width * info_header.height * 3, fp_s);

    for(layer=0;layer<3;layer++){
        ine=layer;
        dest->data[layer]=(char**)malloc(dest->size1*sizeof(char*));
        for(row=0;row<dest->size1;row++){
            dest->data[layer][row]=malloc( dest->size2 * sizeof( char ) );

            for ( col = 0; col < dest->size2; col++ ) {
				dest->data[ layer ][ row ][ col ] = image_s[ine];
				ine=ine+3;
            }
        }

    }

    fclose(fp_s);
}

void imwrite( char *filename, Matrix *dest ){


}
void color2Gray( Matrix *source ){ // dest is source itself

}
void gray2Color( Matrix *source ){ // dest is source itself


}
#if DEBUG
int main(){
    Matrix A,B;
    int i,j,k;

    imread(  "redtest.bmp", &A );
    dump( &A, "A", BB, INT );

    return 0;


}

#endif
