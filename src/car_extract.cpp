/** File: car_extract.cpp
 ** Author: Ryan Lei
 ** Creation: 2009/12/28
 ** Modification: XXXX/XX/XX
 ** Description: The implementations of car feature extraction
    Features:
      1. Rectangle features
      2. Edge Orientation Histograms (EOH)
      3. Edge Density (ED)
    Notice the use of integral histogram in computation.
 **/

#include <iostream>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
#include "car_extract.h"
using namespace std;



