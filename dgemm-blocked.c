/*
 *  A simple blocked implementation of matrix multiply
 *  Provided by Jim Demmel at UC Berkeley
 *  Modified by Scott B. Baden at UC San Diego to
 *    Enable user to select one problem size only via the -n option
 *    Support CBLAS interface
 */
#include <emmintrin.h>
const char* dgemm_desc = "Simple blocked dgemm.";

#if !defined(BLOCK_SIZE)
#define BLOCK_SIZE 41
#define block_size_i 41
#define block_size_j 41
#endif

#define min(a,b) (((a)<(b))?(a):(b))


// Aligned buffers for A and B
double A_buffered[32*800*sizeof(double)] __attribute__((aligned(16)));
double B_global_transpose[800*800*sizeof(double)] __attribute__((aligned(16)));

/* This auxiliary subroutine performs a smaller dgemm operation
 *  C := C + A * B
 * where C is M-by-N, A is M-by-K, and B is K-by-N. */
static void do_block (int lda, int M, int N, int K, double* A, double* B, double* C)
{
  /* For each row i of A */
  for (int i = 0; i < M; ++i)
    /* For each column j of B */ 
    for (int j = 0; j < N; ++j) 
    {
      /* Compute C(i,j) */
      double cij = C[i*lda+j];
      for (int k = 0; k < K; ++k)
	cij += A[i*lda+k] * B[k*lda+j];
      C[i*lda+j] = cij;
    }
}

void transpose_B(int lda, double* B) {
	const register int fringe_start_i = lda / (block_size_i) * (block_size_i); 
	const register int fringe_start_j = lda / (block_size_j) * (block_size_j); 
	int odd_increment = lda % 2;
	for (int i = 0; i < fringe_start_i; i += (block_size_i)) { 
  		for (int j = 0; j < fringe_start_j; j += (block_size_i)) { 
    			for (int i0 = i; i0 < i + (block_size_i); ++i0) { 
				register int index_L = i0*(lda + (odd_increment)) + j; 
				register int index_R = i0 + j*lda; 
      				for (int j0 = j; j0 < j + (block_size_j); ++j0) { 
        				B_global_transpose[index_L] = B[index_R]; 
					index_L++; 
					index_R += lda; 
      				}	 
    			}	 
  		} 
	}
	for (int i = fringe_start_i; i < lda; i++) { 
		for (int j = 0; j < fringe_start_j; j++) { 
			B_global_transpose[i*(lda + (odd_increment)) + j] = B[j*lda + i]; 
		} 
	} 
	for (int i = 0; i < fringe_start_i; i++) {
    		for (int j = fringe_start_j; j < lda; j++) {
      			B_global_transpose[i*(lda + (odd_increment)) + j] = B[j*lda + i]; 
    		}		 
  	}
	for (int i = fringe_start_i; i < lda; i++) { 
    		for (int j = fringe_start_j; j < lda; j++) {
      			B_global_transpose[i*(lda + (odd_increment)) + j] = B[j*lda + i]; 
    		}
  	}
}
/* This routine performs a dgemm operation
 *  C := C + A * B
 * where A, B, and C are lda-by-lda matrices stored in row-major order
 * On exit, A and B maintain their input values. */  
void square_dgemm (int lda, double* A, double* B, double* C)
{
	transpose_B(lda, B);
}

