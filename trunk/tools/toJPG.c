/** File: toJPG.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/25
 ** Modification: 2010/01/26
 ** Description: Generate a MATLAB program of converting BMP images into JPG images.
 **              Useful when batch convertion is unavailable.
 **/

#include <stdio.h>
int main() {
	FILE *fout = fopen("toJPG.m", "w");
	char ch1, ch2, ch3;
	for (ch1 = '0'; ch1 <= '2'; ch1++ ) {
		for (ch2 = '0'; ch2 <= '9'; ch2++ ) {
			for (ch3 = '0'; ch3 <= '9'; ch3++) {
				fprintf(fout, "imwrite(imread('train%c%c%c.bmp'), 'train%c%c%c.JPG');\n",
					ch1, ch2, ch3, ch1, ch2, ch3);
			}
		}
	}
}

