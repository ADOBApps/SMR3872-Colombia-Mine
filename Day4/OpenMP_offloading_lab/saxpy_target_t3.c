/* 
  TASK: Calculate a * X + Y using OpenMP offloading
*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

/* Returns elepsed seconds past from the last call to timer rest */
double seconds(){
  struct timeval tmp;
  double sec;
  gettimeofday( &tmp, (struct timezone *) 0 );
  sec = tmp.tv_sec + ( (double) tmp.tv_usec ) / 1000000.0;
  return sec;
}

int main (int argc, char *argv[]){
    // Your code goes here!
    int N = 100000;
    double * A;
    double * B; 
    double * C;
    double t1 = 0.0, t2 = 0.0;
    A = (double*)malloc(sizeof(double)*N);
    B = (double*)malloc(sizeof(double)*N);
    C = (double*)malloc(sizeof(double)*N);
    for(int i = 0; i < N; ++i){
      A[i] = rand() % (5 * N + 1);
      B[i] = rand() % (7 * N + 1);
    }

    system("clear");
    t1 = seconds();
    #pragma omp target teams num_teams(3)
    for(int i = 0; i < N; ++i){
      C[i] = M_PI * (A[i] + B[i]);
    }
    t2 = seconds();
    printf("\nTotal time: %.6g\n", (t2-t1));
    free(A);
    free(B);
    free(C);
    return(0);
}