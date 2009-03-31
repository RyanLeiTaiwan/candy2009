/** File: util.h
 ** Author: iopic, ryanlei
 ** Creation : 2009/03/23
 ** Modification: 2009/04/01
 ** Description: utility functions
 **/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void error( char *msg ); // 錯誤訊息，並中斷程式
void runningTime( clock_t tic, clock_t toc ); // 計時
void cos_sin_table( float COS[], float SIN[] ); // cos, sin的建表函式
