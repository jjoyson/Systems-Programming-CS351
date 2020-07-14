/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"


int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N]){
    if(M == 61){
        int i, j, k, l, temp;   // 5 VARRIABLES!!
        for (i = 0; i < N; i = i + 18){
            for (j = 0; j < M; j = j + 18){
                for (k = i; k < i + 18 && k < N; k++){ //odd number checks
                    for (l = j; l < j + 18 && l < M; l++){ //odd number checks
                        temp = A[k][l];                       // cant check for for diagonals
                        B[l][k] = temp;
                    }
                }
            }
        }
    }
    else if(M == 32){
        int i, j, k, l, m, temp;    // 6 VARRIABLES!!
        for (i = 0; i < N; i = i + 8){
            for (j = 0; j < M; j = j + 8){
                for (k = j; k < j + 8; k++) {
                    for (l = i; l < i + 8; l++) {
                        if(k != l)  //diagonal check
                            B[l][k] = A[k][l];
                        else{
                            temp = A[k][k];
                            m = k;
                        }
                    }
                    if(i == j) 
                        B[m][m] = temp;                   
                }
            }
        }
    }
    else{   // 12 VARRIABLES!!
        int i, j, temp, temp1, temp2, temp3, temp4, temp5, temp6, temp7, temp8, temp9;
        for (j = 0; j < N; j = j + 4){
            for (i = 0; i < M; i = i + 4){
                temp = A[j][i];
                temp1 = A[j][i+1];
                temp2 = A[j][i+2];
                temp3 = A[j][i+3];
                temp4 = A[j+1][i];
                temp5 = A[j+1][i+1];
                temp6 = A[j+1][i+2];
                temp7 = A[j+1][i+3];
                temp8 = A[j+2][i];
                temp9 = A[j+2][i+1];

                B[i][j] = temp;
                B[i][j+1] = temp4;
                B[i][j+2] = temp8;

                temp = A[j+2][i+2];
                temp4 = A[j+2][i+3];
                temp8 = A[j+3][i];

                B[i][j+3] = temp8;
                B[i+1][j] = temp1;
                B[i+1][j+1] = temp5;
                B[i+1][j+2] = temp9;

                temp8 = A[j+3][i+1];
                temp1 = A[j+3][i+2];
                temp5 = A[j+3][i+3];

                B[i+1][j+3] = temp8;
                B[i+2][j] = temp2;
                B[i+2][j+1] = temp6;
                B[i+2][j+2] = temp;
                B[i+2][j+3] = temp1;
                B[i+3][j] = temp3;
                B[i+3][j+1] = temp7;
                B[i+3][j+2] = temp4;
                B[i+3][j+3] = temp5;
            }
        }
    }
}



   


/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}