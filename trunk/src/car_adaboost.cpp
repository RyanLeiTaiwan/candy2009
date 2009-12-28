/** File: car_adaboost.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: XXXX/XX/XX
 ** Description: The implementations of cascaded AdaBoost.
 **   This learning algorithm is based on the Chen-and-Chen paper,
 **   which is "real" AdaBoost in a "cascaded" structure.
 **   But for now, only "binary" AdaBoost is implemented.
 **/

#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
using namespace std;

void function( int param1, int param2 );


