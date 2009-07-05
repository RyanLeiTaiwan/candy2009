/** File: util.h
 ** Author: iopic, ryanlei
 ** Creation : 2009/03/23
 ** Modification: 2009/07/04
 ** Description: utility functions
 **/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

typedef enum { false, true } bool; // define 'bool' type
void error( char *msg ); // 錯誤訊息，並中斷程式
void runningTime( clock_t tic, clock_t toc ); // 計時
void cos_sin_table( float COS[], float SIN[] ); // cos, sin的建表函式
void cot_csc_table( float COT[], float CSC[], float COS[], float SIN[] );
// cot, csc的建表函式：用建好的COS, SIN來除
