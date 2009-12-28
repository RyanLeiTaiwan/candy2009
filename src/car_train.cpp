/** File: car_extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: XXXX/XX/XX
 ** Description: The car-training program based on the paper:
    Fast Human Detection Using a Novel Boosted Cascading Structure With Meta Stages, Chen and Chen, 2008.
    Important techniques / concepts:
      1. Real AdaBoost -> For now, use basic binary Adaboost
      2. Cascaded classifiers -> For now, it is without meta-stages (no confidence values)
      3. Rectangle / Edge Orientation Histogram / Edge-Density features
      4. Integral image technique in computation
 **/

/* Usage: car_train */
#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
using namespace std;

void function( int param1, int param2 );


int main() {
	
	cout << "Hello, world2223322" << endl;
	cout << WINDOW_WIDTH << ", " << WINDOW_HEIGHT << endl;
	getchar();

	return 0;
}
