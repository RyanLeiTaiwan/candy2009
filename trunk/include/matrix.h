/** File: matrix.h
 ** Author: ryanlei
 ** Creation : 2009/03/21
 ** Modification: 2009/09/16
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

typedef enum { RR, GG, BB, ALL } COLOR;
// 對於2D矩陣來說，RR和ALL意義是一樣的
typedef enum { INT, FLOAT } TYPE;
// 格式指定：int, float
typedef enum { Gradient, Sobel, Laplace } FILTER;
// 水平gradient，水平Sobel，二階Laplacian filter
typedef enum { horizontal, vertical } DIRECTION;

void full_dump( Matrix *source, char *name, COLOR color, TYPE type );
// 顯示矩陣全部內容( 矩陣、名稱、第三維、INT或FLOAT )
void part_dump( Matrix *source, char *name, COLOR color, int rowBegin, int rowEnd, int colBegin, int colEnd, TYPE type );
// 顯示矩陣部分內容( 矩陣、名稱、第三維、前兩維度的範圍、INT或FLOAT )
void zeros( Matrix *dest, int size1, int size2, int size3 ); // 產生全0的矩陣
void clear( Matrix *source ); // 將矩陣數值歸零
void ones( Matrix *dest, int size1, int size2, int size3 ); // 產生全1的矩陣
void eye( Matrix *dest, int size ); // 產生2D Identity矩陣
void RAND( Matrix *dest, int size1, int size2, int size3, int lower, int upper );
// 產生整數亂數[lower,upper]矩陣
void Gaussian( Matrix *dest, int size, float sigma );
// 產生2D Gaussian filter，weight的總和是1
void freeMatrix( Matrix *source ); // 清除矩陣的記憶體空間

/* scalar operations */
void s_add( Matrix *source, float number ); // 元素加上一個純量
void s_mul( Matrix *source, float number ); // 元素乘上一個純量（係數積）
void s_pow( Matrix *source, float power ); // 元素的純量次方
void s_sqrt( Matrix *source ); // 元素開根號，是s_pow的特例。

/* matrix or element-wise operations */
void m_add( Matrix *source1, Matrix *source2, Matrix *dest );
// 元素對應相加，sources不可和dest相同。
void e_mul( Matrix *source1, Matrix *source2, Matrix *dest );
// 元素對應相乘，sources不可和dest相同，假設dest尚未malloc。
void m_mul( Matrix *source1, Matrix *source2, COLOR color1, COLOR color2, Matrix *dest );
// 2D矩陣相乘，sources皆可指定用哪些layer，sources不可和dest相同。
void full_assign( Matrix *source, Matrix *dest, COLOR sColor, COLOR dColor );
// 全範圍的matrix assignment，可指定兩者的layer。假設皆已malloc過。
void part_assign( Matrix *source, Matrix *dest,
	int sRowBegin, int sRowEnd, int sColBegin, int sColEnd, int sLayerBegin, int sLayerEnd,
	int dRowBegin, int dRowEnd, int dColBegin, int dColEnd, int dLayerBegin, int dLayerEnd );
// 指定row, col, layer範圍的matrix assignment，兩者皆已malloc過。
void ABS( Matrix *source );
// 整個矩陣取絕對值
float m_sum( Matrix *source );
// 求整個matrix的總和
void max_min( Matrix *source, float *maxRet, float *minRet );
// 求2D矩陣的最大值和最小值，只求值不求index
void map_0_255( Matrix *source );  
// 將範圍線性對應到[0, 255]
void change_0_to_1( Matrix *source );
// 把所有值是0的都改成1
void cross( Matrix *image, Matrix *filter, Matrix *dest );
// 2D cross-correlation運算，filter必須為2D奇數X奇數的方陣，且size小於image，邊界用鏡射法，
// dest尚未malloc。
void gradient( Matrix *source, Matrix *dest, DIRECTION dir, bool centered );
// gradient filters: horizontal/vertical, centered/uncentered，dest is not yet malloced
float v_norm2( Matrix *source );
// 把整個矩陣當作vector取2-norm
void integral( Matrix *source, Matrix *dest );
// 對2D矩陣取integral(自左上方的累積量值)，dest尚未malloc。
inline float ii_eval( Matrix *ii, int row, int col );
// integral()用到的DP查表
float recSum( Matrix *ii, int rowBeg, int colBeg, int rowEnd, int colEnd );
// 指定四角的rectangular sum，必須傳入integral matrix
void mean_var_normalize( Matrix *source, Matrix *ii, Matrix *ii2 );
// Chen-and-Chen paper提到的 mean and variance normalization，
// 需傳入原始矩陣，數值和與平方和的integral image(加速用)，結果直接寫回ii。
