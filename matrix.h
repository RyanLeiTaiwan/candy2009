/** File: matrix.h
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/03/22
 ** Description: matrix data structure 
 **/
typedef struct Matrix {
	
	/* array of values, 先一律用double 3D array */
	double ***data; /* [ layer ][ row ][ col ] */
	/* dimensions */
	int size1; /* row: 碰到row vector的話size1 = 1 */
	int size2; /* col: 碰到column vector的話size2 = 1 */
	int size3; /* layer: 碰到2D的話size3 = 1 */

} Matrix;

/* matrix basics */
typedef enum COLOR { RR, GG, BB, ALL } COLOR; 
// 對於2D矩陣來說，RR和ALL意義是一樣的
typedef enum TYPE { INT, DOUBLE } TYPE;
void dump( Matrix *source, char *name, COLOR color, TYPE type ); 
// 顯示矩陣內容( 矩陣、名稱、第三維、INT或DOUBLE )
void zeros( Matrix *source, int size1, int size2, int size3 ); // 產生全0的矩陣
void ones( Matrix *source, int size1, int size2, int size3 ); // 產生全1的矩陣
void eye( Matrix *source, int size ); // 產生2D Identity矩陣
void RAND( Matrix *source, int size1, int size2, int size3, int lower, int upper ); 
// 產生整數亂數[lower,upper]矩陣
void freeMatrix( Matrix *source ); // 清除矩陣的記憶體空間

/* scalar operations */
void s_add( Matrix *source, double number ); // 元素加上一個純量
void s_mul( Matrix *source, double number ); // 元素乘上一個純量（係數積）
void s_pow( Matrix *source, double number ); // 元素的純量次方 

/* matrix or element-wise operations */
void m_add( Matrix *source1, Matrix *source2, Matrix *dest ); 
// 元素對應相加，sources不可和dest相同
void e_mul( Matrix *source1, Matrix *source2, Matrix *dest ); 
// 元素對應相乘，sources不可和dest相同
void m_mul( Matrix *source1, Matrix *source2, COLOR color1, COLOR color2, Matrix *dest ); 
// 2D矩陣相乘，sources皆可指定用哪些layer，sources不可和dest相同


