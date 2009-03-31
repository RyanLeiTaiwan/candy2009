/** File: image.h
 ** Author: ryanlei
 ** Creation: 2009/03/29
 ** Modification: 2009/04/01
 ** Description: Image header and read/write, conversion operations.
                 Handled in "Little-Endian".
 **/

#include "matrix.h"

/* Change the typedef settings for your environment, if necessary.
 * Mine is Macbook with Intel x86-64 CPU.
 */
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;

/**********    BMP related    **********/
/* BMP file header, 14 bytes */
typedef struct BMP_file_header {
	uint16 ID; /* identifier, mostly with value "BM", or 0x4D42 */
	uint32 file_size; /* file size in bytes */
	uint16 reserved1; /* reserved, must be zero */
	uint16 reserved2; /* reserved, must be zero */
	/* offset to start of image data in bytes, mostly 54, or 0x36000000 */
	uint32 data_offset;
} BMP_file_header;

 /* BMP info header, 40 bytes */
typedef struct BMP_info_header {
	int32 info_size; /* size of BITMAPINFOHEADER structure, mostly 40 */
	uint32 width; /* columns, in pixels, always positive */
	/* rows, in pixels */
	/* negative if image is read from top to bottom (mostly)
	 * positive if image is read from bottom to top */
	int32 height;
	uint16 planes; /* number of planes in the image, must be 1 */
	/* number of bits per pixel, can be 1/4/8/16/24/32 */
	uint16 bits_per_pixel;
	uint32 compression; /* without compression: 0. can be 0,1,2,3 */
	/* size of image data in bytes (including padding) */
	/* if without compression, this field can be 0 */
	uint32 data_size;
	/* to convert to "dpi"(pixels per inch), divide by 39.37 */
	uint32 H_resolution; /* horizontal resolution in pixels per meter */
	uint32 V_resolution; /* vertical resolution in pixels per meter */
	uint32 used_colors; /* number of colors in image, or zero */
	uint32 important_colors; /* number of important colors, or zero */
} BMP_info_header;

/* BMP Palette */
/* the color palette is not used when the bitmap is 16-bit or higher;
 * there are no palette bytes in those BMP files. */
typedef struct BMP_palette {
	uint8 blue;
	uint8 green;
	uint8 red;
	uint8 reserved;
} BMP_palette;

/**********    function prototypes    **********/
void imread( char *filename, Matrix *dest );  // dest is not yet allocted
void imwrite( char *filename, Matrix *dest );
void color2Gray( Matrix *source ); // dest is source itself
void gray2Color( Matrix *source ); // dest is source itself

