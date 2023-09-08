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
    int N = 1000;
    int A[N][N], B[N][N], C[N][N];
    double t1 = 0.0, t2 = 0.0;
    for(int i = 0; i < N; ++i){
        for(int j = 0; j < N; ++j){
          A[i][j] = rand() % (5 * N + 1);
          B[j][i] = rand() % (7 * N + 1);
        }
    }

    system("clear");
    t1 = seconds();
    #pragma omp target
    {
        for(int i = 0; i < N; ++i){
            for(int j = 0; j < N; ++j){
            C[i][j] = M_PI * (A[i][j] + B[i][j]);
            printf("matrix value %i\n", C[i][j]);
            }
        }
    }
    t2 = seconds();
    printf("\nTotal time: %.6g\n", (t2-t1));
    return(0);
}