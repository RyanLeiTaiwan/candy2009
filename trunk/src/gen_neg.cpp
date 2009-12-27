/** File: gen_neg.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/26
 ** Modification: 2009/12/27
 ** Description: Generate multiple negative images chopped
 **              from large ones.
 **/

/* Usage: gen_neg [INPUT_DIR] [OUTPUT_DIR] */ 

#include <iostream.h>
#include <cxcore.h>
#include <cv.h>
#include <dirent.h>
#include "global.h"

int main( int argc, char *argv[] ) {
	DIR *dir;
	struct dirent *dp;
	FILE *fin, *fout;
	
	if ( argc != 3 ) {
		cout << "Usage: gen_neg [INPUT_DIR] [OUTPUT_DIR]" << endl;
		exit( EXIT_FAILURE );
	}
	if ( !( dir = opendir( argv[ 1 ] ) ) ) {
		cout << "gen_neg(): [INPUT_DIR] does not exist.\n";
		exit( EXIT_FAILURE );
	}
	
	cout << "gen_neg(): This program will take in a directory of large-sized images\n, " <<
		"ASSUMING THE ASPECT RATIO is the same as" << LARGE_WIDTH << ":" << LARGE_HEIGHT << 
	", then:\n 1. Resizing each image to exactly" << LARGE_WIDTH << "x"<< LARGE_HEIGHT << ".\n" <<
	"2. Discard the 4 sides of the image by window size: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ".\n" <<
	"3. Scan using the window, and write the new images to [OUTPUT_DIR]/FILENAME.jpg, where\n" <<
	"   FILENAME is a digit counting from 1.\n";
	
	
	return 0;
}
