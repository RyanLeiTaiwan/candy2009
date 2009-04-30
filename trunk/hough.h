/** File: hough.h
 ** Author: ryanlei
 ** Creation: 2009/04/30
 ** Modification: XXXX/XX/XX
 ** Description: Hough transform, the Rho-Theta Version
 **/
#include "image.h"

/* the hough transform with several parameters */
void hough( Matrix *img, char *filename,
	FILTER filter, float min_edge, float min_diag );
