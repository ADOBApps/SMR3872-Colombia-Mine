#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
 
/**
 * This code picks a process as the broadcast root, and makes it
 * broadcast a specific value. Other processes participate to the broadcast as
 * receivers. These processes then print the value they received via the 
 * broadcast.
 **/
int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);
 
    // Get my rank in the communicator
    int myproc_id, numprocs;
    int broadcast_root;
    int buffer;
    MPI_Comm_rank(MPI_COMM_WORLD, &myproc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
 
    // Determine the rank of the broadcast emitter process
    broadcast_root = numprocs -1;

    if( !myproc_id ) fprintf( stdout, "Running with %d processes...\n", numprocs);

    if(myproc_id == broadcast_root){
        buffer = 12345;
        printf("[MPI process %d] I am the broadcast root (the last), and send value %d.\n", myproc_id, buffer);
    }
    MPI_Bcast(&buffer, 1, MPI_INT, broadcast_root, MPI_COMM_WORLD);
    if(myproc_id != broadcast_root){
        printf("[MPI process %d] I am a broadcast receiver, and obtained value %d.\n", myproc_id, buffer);
    }
 
    MPI_Finalize();
 
    return EXIT_SUCCESS;
}