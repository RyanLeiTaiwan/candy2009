/** File: util.cpp
 ** Author: Ryan Lei
 ** Creation: 2010/01/09
 ** Modification: XXXX/XX/XX
 ** Description: Utility functions
 **/

#include <iostream>
#include <cstring>
#include <dirent.h>
#include <cxcore.h>
#include <cv.h>
#include "parameters.h"
#include "util.h"
using namespace std;


/* count the number of images in a directory */
int	countImages(const char *PATH_BASE) {
	int ret = 0;
	DIR *dir;
	struct dirent *dp;
	
	try {
		if (!(dir = opendir(PATH_BASE))) {
			throw "Directory open exception.";
		}
		if (Unix) {
			/* In unix-like systems, hidden file names start with '.' */
			while (dp = readdir(dir)) {
				if (dp->d_name[ 0 ] != '.') {
					ret++;
				}
			}
		}
		else {
			/* In Windows system, ignore the "desktop.ini" */
			while (dp = readdir(dir)) {
				if (!strcmp(dp->d_name, "desktop.ini")) {
					ret++;
				}
			}
		}

	}
	catch (const char *e) {
		cerr << "countImages(): " << e << endl;
	}

	return ret;
}

