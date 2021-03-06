/** File: util.h
 ** Author: iopic, ryanlei
 ** Creation : 2009/03/23
 ** Modification: 2009/09/24
 ** Description: utility functions
 **/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#define EPSILON 1e-9 /* a small positive number to prevent from division-by-zero */

typedef enum { false, true } bool; // define 'bool' type
void error( char *msg ); // 錯誤訊息，並中斷程式
void runningTime( clock_t tic, clock_t toc ); // 計時
void cos_sin_table( float COS[], float SIN[] ); // cos, sin的建表函式
void cot_csc_table( float COT[], float CSC[], float COS[], float SIN[] );
// cot, csc的建表函式：用建好的COS, SIN來除
inline short sign( float num ); /* sign of the value: +1 or -1 or 0 */
