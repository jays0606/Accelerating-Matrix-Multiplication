#include "matmul.h"

void matmul_ref(const int* const matrixA, const int* const matrixB,
                int* const matrixC, const int n) {

  for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++)
      for (int k = 0; k < n; k++)
        matrixC[i * n + j] += matrixA[i * n + k] * matrixB[k * n + j];
}

void matmul_optimized(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n) {

  int i,j,k,ii,jj,kk,tmp;
  int b = 64;
  int *matrixB_T = new int[n*n]; 

  if (n%b == 0) {     // n divisible by b (no remainder)  
    // Transpose matrix B
    #pragma omp parallel for collapse(3) num_threads(128)
    for (i=0; i < n; i+=b) 
      for (j=0; j < n; j+=b)
        for (ii=0; ii < b; ii++)
          for (jj=0; jj < b; jj++)
            matrixB_T[(ii+i)*n+jj+j] = matrixB[(jj+j)*n+ii+i];
  
    // Main Calculation
    #pragma omp parallel for collapse(2) num_threads(128)
    for (i = 0; i < n; i+=b/2) 
      for (j = 0; j < n; j+=b/2)
        for (k = 0; k < n; k+=b*8)
          for (ii=i; ii < i+b/2; ii++)
            for (jj=j; jj < j+b/2; jj++){
              tmp = 0; 
              for(kk=k; kk < k+b*8; kk++)
                tmp += matrixA[ii*n+kk]*matrixB_T[jj*n+kk];
              matrixC[ii*n+jj] += tmp;
        }
  }

  else {   // n not divisible by b (yes remainder)
    int c = n-n%(b*8);   
    // Transpose matrix B
    #pragma omp parallel for collapse(2) num_threads(128)
    for (i=0; i < n; i++)
      for (j=0; j < n; j++) 
        matrixB_T[i*n+j] = matrixB[j*n+i];

    // Main Calculation
    #pragma omp parallel for collapse(2) num_threads(128)
    for (i = 0; i < c; i+=b/2) 
      for (j = 0; j < c; j+=b/2)
        for (k = 0; k < c; k+=b*8)
          for (ii=i; ii < i+b/2; ii++)
            for (jj=j; jj < j+b/2; jj++){
              tmp = 0;
              for(kk=k; kk < k+b*8; kk++)
                tmp += matrixA[ii*n+kk]*matrixB_T[jj*n+kk];
              matrixC[ii*n+jj] += tmp;
          }

    // Remainder Calculation
    #pragma omp parallel for collapse(2)
    for(i=c; i<n; i++)
      for(j=0; j<c; j++)
        for(k=0; k<n; k++)
          matrixC[i*n + j] += matrixA[i*n + k] * matrixB_T[j*n + k];
    #pragma omp parallel for collapse(2)
    for(i=0; i<n; i++)
      for(j=c; j<n; j++)
        for(k=0; k<n; k++)
          matrixC[i*n + j] += matrixA[i*n + k] * matrixB_T[j*n + k];
    #pragma omp parallel for collapse(2)
    for(i=0; i<c; i++)
      for(j=0; j<c; j++)
        for(k=c; k<n; k++)
          matrixC[i*n + j] += matrixA[i*n + k] * matrixB_T[j*n + k];
    }
  delete[] matrixB_T;
}

void matmul_block_tmp(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n) {

  int i,j,k,ii,jj,kk;

  // Transposing B to B_T
  int *matrixB_T = new int[n*n];
  #pragma omp parallel for collapse(2) num_threads(128)
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) 
      matrixB_T[i*n+j] = matrixB[j*n+i];

  int b = 64; int tmp;
  #pragma omp parallel for collapse(2) num_threads(128)
  for (i = 0; i < n; i+=b)
    for (j = 0; j < n; j+=b) 
      for (k = 0; k < n; k+=b)
        for (ii=i; ii < i+b; ii++)
          for(jj=j; jj < j+b; jj++){
            tmp = 0;
            for(kk=k; kk < k+b; kk++)
              tmp += matrixA[ii*n+kk] * matrixB_T[jj*n+kk];
            matrixC[ii*n+jj] += tmp;
          }
  delete[] matrixB_T;
}

void matmul_block(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n) {

  int i,j,k,ii,jj,kk;

  // Transposing B to B_T
  int *matrixB_T = new int[n*n];
  #pragma omp parallel for collapse(2) num_threads(128)
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) 
      matrixB_T[i*n+j] = matrixB[j*n+i];

  int b = 64; int tmp;
  #pragma omp parallel for collapse(2) num_threads(128)
  for (i = 0; i < n; i+=b)
    for (j = 0; j < n; j+=b) 
      for (k = 0; k < n; k+=b)
        for (ii=i; ii < i+b; ii++)
          for(jj=j; jj < j+b; jj++)
            for(kk=k; kk < k+b; kk++)
              matrixC[ii*n+jj] += matrixA[ii*n+kk] * matrixB_T[jj*n+kk];

  delete[] matrixB_T;
}

void matmul_naive(const int* const matrixA, const int* const matrixB,
                      int* const matrixC, const int n) {

  int i,j,k;
  // Transposing B to B_T
  int *matrixB_T = new int[n*n];
  #pragma omp parallel for collapse(2) num_threads(128)
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) 
      matrixB_T[i*n+j] = matrixB[j*n+i];
  
  // Navie Approach
  #pragma omp parallel for collapse(2) num_threads(128)// threading
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      for (k = 0; k < n; k++)
        // matrixC[i * n + j] += matrixA[i * n + k] * matrixB[k * n + j]; // Not using transpose
        matrixC[i * n + j] += matrixA[i * n + k] * matrixB_T[j * n + k];  // Using Transpose
}


