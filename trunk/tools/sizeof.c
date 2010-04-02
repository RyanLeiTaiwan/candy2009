/** File: sizeof.c
 ** Author: Ryan Lei
 ** Creation: 2009/03/29
 ** Modification: 2010/01/14
 ** Description: Test the size of all data types on the operating system.
 **/
#include <stdio.h>
int main() {
	printf( "size_t      = %2d byte(s).\n", (int) sizeof( size_t ) );
	printf( "char        = %2d byte(s).\n", (int) sizeof( char ) );
	printf( "short       = %2d byte(s).\n", (int) sizeof( short ) );
	printf( "int         = %2d byte(s).\n", (int) sizeof( int ) );
	printf( "long        = %2d byte(s).\n", (int) sizeof( long ) );
	printf( "long long   = %2d byte(s).\n", (int) sizeof( long long ) );
	printf( "float       = %2d byte(s).\n", (int) sizeof( float ) );
	printf( "double      = %2d byte(s).\n", (int) sizeof( double ) );
	printf( "long double = %2d byte(s).\n", (int) sizeof( long double ) );
	printf( "uchar*      = %2d byte(s).\n", (int) sizeof( unsigned char * ) );
	printf( "int*        = %2d byte(s).\n", (int) sizeof( int * ) );
	printf( "float*      = %2d byte(s).\n", (int) sizeof( float * ) );
	printf( "double*     = %2d byte(s).\n", (int) sizeof( double * ) );
}
