#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define N 16

void print_mat(double *mat, int n_loc){
  for (int i = 0; i < n_loc; i += 1){
    for (int j = 0; j < N; j += 1){
      fprintf(stdout, "%.3g ", mat[i * N + j]);
    }
    fprintf(stdout, "\n");
  }
}

int main(int argc, char * argv[]){

  double * mat;
  int i, j;

  int numprocs = 1, myproc_id = 0, n_loc, i_g, j_g; 
  int offset, rest;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
  MPI_Comm_rank( MPI_COMM_WORLD, &myproc_id );

  if( !myproc_id ){
    fprintf( stdout, "Running with %d processes...\n", numprocs);
  }

  n_loc = N / numprocs;
  rest = N % numprocs;
  
  if ( myproc_id < rest ){
    n_loc += 1;
    offset = 0;
  }
  else{
    offset = rest;
  } 
  
  mat = (double *) malloc( n_loc * N * sizeof(double) );

  for( i = 0; i< n_loc; i++ ){
    for( j = 0; j< N; j++ ){
      j_g = j;
      i_g = (myproc_id * n_loc) + i + offset;
      if( i_g == j_g ){
        mat[ i * N + j ] = 1.0;
      }
      else{
        mat[ i * N + j ] = 0.0;
      }
    }
  }

#ifdef PRINT_WITH_MPI_GATHER
  double * mat_g;
  if( !myproc_id ){
    mat_g = (double *) malloc( N * N * sizeof(double) );
  }
  MPI_Gather(mat, n_loc * N, MPI_DOUBLE, mat_g, n_loc * N, MPI_DOUBLE, 0, MPI_COMM_WORLD );

  if( ! myproc_id ){
    for( i = 0; i< N; i++ ){
      for( j = 0; j< N; j++ ){
        fprintf( stdout, "%.3g " );
      }
      fprintf( stdout, "\n" );
    }
  }

  if( !myproc_id ){
    free( mat_g );
  }
#else
  if( !myproc_id ){
    print_mat(mat, n_loc);
    for( int count = 1; count < numprocs; count ++ ){
	    MPI_Recv(mat, n_loc * N, MPI_DOUBLE, count, 100, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      if (rest && rest < myproc_id){
	      print_mat(mat, n_loc -1);
      }
      else{
        print_mat(mat, n_loc -1);
      }
    }
  }else{
    MPI_Send(mat, n_loc * N, MPI_DOUBLE, 0, 100, MPI_COMM_WORLD);
  }
  
#endif
  free( mat );
  MPI_Finalize();
  return 0;
}
      
