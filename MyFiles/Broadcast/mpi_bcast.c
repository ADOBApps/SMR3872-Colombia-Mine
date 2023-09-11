#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
 
/**
 * @brief Illustrates how to broadcast a message.
 * @details This code picks a process as the broadcast root, and makes it
 * broadcast a specific value. Other processes participate to the broadcast as
 * receivers. These processes then print the value they received via the 
 * broadcast.
 **/
int main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);
 
    // Get my rank in the communicator
    int my_proc_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_proc_id);
 
    // Determine the rank of the broadcast emitter process
    int broadcast_root = 0;
 
    int buffer;
    if(my_proc_id == broadcast_root){
        buffer = 12345;
        printf("[MPI process %d] I am the broadcast root, and send value %d.\n", my_proc_id, buffer);
    }
    MPI_Bcast(&buffer, 1, MPI_INT, broadcast_root, MPI_COMM_WORLD);
    if(my_proc_id != broadcast_root){
        printf("[MPI process %d] I am a broadcast receiver, and obtained value %d.\n", my_proc_id, buffer);
    }
 
    MPI_Finalize();
 
    return EXIT_SUCCESS;
}
//Logo of the Rookie HPC website.