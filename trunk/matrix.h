/** File: matrix.h
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/04/01
 ** Description: matrix data structure
 **/
#include "util.h"

typedef struct Matrix {

	/* array of values, 先一律用float 3D array */
	float ***data; /* [ layer ][ row ][ col ] */
	/* dimensions */
	int size1; /* row: 碰到row vector的話size1 = 1 */
	int size2; /* col: 碰到column vector的話size2 = 1 */
	int size3; /* layer: 碰到2D的話size3 = 1 */

} Matrix;

/* matrix basics */
typedef enum COLOR { RR, GG, BB, ALL } COLOR;
// 對於2D矩陣來說，RR和ALL意義是一樣的
typedef enum TYPE { INT, DOUBLE } TYPE;
void dump( Matrix *source, char *name, COLOR color, int rowBegin, int rowEnd, int colBegin, int colEnd, TYPE type );
// 顯示矩陣內容( 矩陣、名稱、第三維、INT或DOUBLE )
void zeros( Matrix *dest, int size1, int size2, int size3 ); // 產生全0的矩陣
void ones( Matrix *dest, int size1, int size2, int size3 ); // 產生全1的矩陣
void eye( Matrix *dest, int size ); // 產生2D Identity矩陣
void RAND( Matrix *dest, int size1, int size2, int size3, int lower, int upper );
// 產生整數亂數[lower,upper]矩陣
void freeMatrix( Matrix *source ); // 清除矩陣的記憶體空間

/* scalar operations */
void s_add( Matrix *source, float number ); // 元素加上一個純量
void s_mul( Matrix *source, float number ); // 元素乘上一個純量（係數積）
void s_pow( Matrix *source, float number ); // 元素的純量次方

/* matrix or element-wise operations */
void m_add( Matrix *source1, Matrix *source2, Matrix *dest );
// 元素對應相加，sources不可和dest相同。
void e_mul( Matrix *source1, Matrix *source2, Matrix *dest );
// 元素對應相乘，sources不可和dest相同。
void m_mul( Matrix *source1, Matrix *source2, COLOR color1, COLOR color2, Matrix *dest );
// 2D矩陣相乘，sources皆可指定用哪些layer，sources不可和dest相同。
void full_assign( Matrix *source, Matrix *dest, COLOR sColor, COLOR dColor );
// 全範圍的matrix assignment，可指定兩者的layer。假設皆已malloc過。
void part_assign( Matrix *source, Matrix *dest,
	int sRowBegin, int sRowEnd, int sColBegin, int sColEnd, int sLayerBegin, int sLayerEnd,
	int dRowBegin, int dRowEnd, int dColBegin, int dColEnd, int dLayerBegin, int dLayerEnd );
// 指定row, col, layer範圍的matrix assignment，兩者皆已malloc過。
void cross( Matrix *image, Matrix *filter, Matrix *dest );
// 2D cross-correlation運算，filter必須為2D奇數X奇數的方陣，且size小於image，邊界用鏡射法。


