#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int message;
    int next, prev;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Determine the rank of the next and previous processes in the ring
    next = (rank + 1) % size;
    prev = (rank - 1 + size) % size;

    // Send the rank in a ring from process 0 to process size-1
    if (rank == 0) {
        message = rank;
        MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
        printf("Process %d sent message %d\n", rank, message);
    }

    // Pass the message along the ring
    MPI_Recv(&message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
    printf("Process %d received message %d\n", rank, message);
    MPI_Send(&message, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    printf("Process %d sent message %d\n", rank, message);

    // Receive the final message from process 0
    if (rank == 0) {
        MPI_Recv(&message, 1, MPI_INT, prev, 0, MPI_COMM_WORLD, &status);
        printf("Process %d received message %d\n", rank, message);
    }

    MPI_Finalize();
    return 0;
}