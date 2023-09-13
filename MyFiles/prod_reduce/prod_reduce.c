/**
 * @author RookieHPC
 * @brief Original source code at https://rookiehpc.org/mpi/docs/mpi_reduce/index.html
 **/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/**
 * @brief Illustrates how to use a reduce.
 * @details This application consists of a sum reduction; every MPI process
 * sends its rank for reduction before the sum of these ranks is stored in the
 * root MPI process. It can be visualised as follows, with MPI process 0 as
 * root:
 *
 * +-----------+ +-----------+ +-----------+ +-----------+
 * | Process 0 | | Process 1 | | Process 2 | | Process 3 |
 * +-+-------+-+ +-+-------+-+ +-+-------+-+ +-+-------+-+
 *   | Value |     | Value |     | Value |     | Value |
 *   |  val  |     |  val  |     |  val  |     |  val  |
 *   +-------+     +-------+     +-------+     +-------+
 *            \         |           |         /
 *             \        |           |        /
 *              \       |           |       /
 *               \      |           |      /
 *                +-----+-----+-----+-----+
 *                            |
 *                        +---+---+
 *                        | PROD  |
 *                        +---+---+
 *                            |
 *                        +---+---+
 *                        | VAL^4 |
 *                        +-------+
 **/
int main(int argc, char* argv[]){

    // Determine root's rank
    int root_rank = 0;

    // Get the size of the communicator
    int size = 0;

    // var to use
    int val = 1;
    if( argc == 2 ) {
        printf("The argument supplied is %s\n", argv[1]);
        val = atoi(argv[1]);
    } else if( argc > 2 ) {
        printf("Too many arguments supplied.\n");
    } else {
        printf("One argument expected.\n");
    }

    // MPI cycle
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if(size != 4){
        printf("This application is meant to be run with 4 MPI processes.\n");
        MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
    }

    // Get my rank
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    // Each MPI process sends its rank to reduction, root MPI process collects the result
    int reduction_result = 0;
    /*
    * MPI_Reduce(const void* send_buffer,
               void* receive_buffer,
               int count,
               MPI_Datatype datatype,
               MPI_Op operation,
               int root,
               MPI_Comm communicator);
    */
    MPI_Reduce(&val, &reduction_result, 1, MPI_INT, MPI_PROD, root_rank, MPI_COMM_WORLD);
    if(my_rank == root_rank){
        printf("The pow(2, %d) %d.\n", size, reduction_result);
    }

    MPI_Finalize();

    return EXIT_SUCCESS;
}
