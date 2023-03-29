#include <stdio.h>     // printf()
#include <limits.h>    // UINT_MAX
#include <mpi.h>       // MPI functions

int checkCircuit(int, int);
int calculate_legal_circuit_count(int, int);

int main() {

    int count = 0;        /* number of solutions */
    int comm_sz; /*number of process*/
    int my_rank; /*process id */


    MPI_Init(NULL,NULL); // MPI initialization

    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); // The number of processes associated with the communicator
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Determine the rank of a process

    double startTime = 0.0, totalTime = 0.0;
    startTime = MPI_Wtime();

    // Distribute every processes to check circuit
    int local_count = calculate_legal_circuit_count(my_rank, comm_sz);  



    // Use tree-structured communication to send local sum to process 0 
    int m = comm_sz;

    count = local_count;

    while (m > 1) {

        int r = m % 2; // check whether procesess count could be divide by 2

        m = m / 2; // Devide proceeses to receive local sum part and send local sum part

        if (my_rank < m) {
            MPI_Recv(&local_count, 1, MPI_INT, my_rank + m + r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            count += local_count;

            m = m + r; //Calculate how many processes would partipate in the next round 
            
        }
        else if (my_rank >= m + r) {

            MPI_Send(&count, 1, MPI_INT, my_rank - m - r, 0, MPI_COMM_WORLD);
            totalTime = MPI_Wtime() - startTime;

            m = 1; // After send the local count, the process finished the task then could leave the while loop to wait other processes.
 
        }
        else {
            m = m + r;
        }

    }

    totalTime = MPI_Wtime() - startTime;

    if (my_rank == 0) {

        printf("Process %d finished in time %f secs.\n", my_rank, totalTime);
        printf("\nA total of %d solutions were found.\n\n", count);
    
    }
    else {
        printf("Process %d finished in time %f secs.\n", my_rank, totalTime);
    }

    MPI_Finalize(); // Clean up all MPI data structures and operations

    fflush(stdout);
    return 0;

}

/* EXTRACT_BIT is a macro that extracts the ith bit of number n.
 *
 * parameters: n, a number;
 *             i, the position of the bit we want to know.
 *
 * return: 1 if 'i'th bit of 'n' is 1; 0 otherwise
 */

#define EXTRACT_BIT(n,i) ( (n & (1<<i) ) ? 1 : 0)


 /* checkCircuit() checks the circuit for a given input.
  * parameters: id, the id of the process checking;
  *             bits, the (long) rep. of the input being checked.
  *
  * output: the binary rep. of bits if the circuit outputs 1
  * return: 1 if the circuit outputs 1; 0 otherwise.
  */

#define SIZE 16

int checkCircuit(int id, int bits) {
    int v[SIZE];        /* Each element is a bit of bits */
    int i;

    for (i = 0; i < SIZE; i++)
        v[i] = EXTRACT_BIT(bits, i);

    if ((v[0] || v[1]) && (!v[1] || !v[3]) && (v[2] || v[3])
        && (!v[3] || !v[4]) && (v[4] || !v[5])
        && (v[5] || !v[6]) && (v[5] || v[6])
        && (v[6] || !v[15]) && (v[7] || !v[8])
        && (!v[7] || !v[13]) && (v[8] || v[9])
        && (v[8] || !v[9]) && (!v[9] || !v[10])
        && (v[9] || v[11]) && (v[10] || v[11])
        && (v[12] || v[13]) && (v[13] || !v[14])
        && (v[14] || v[15]))
    {
        printf("%d) %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d \n", id,
            v[15], v[14], v[13], v[12],
            v[11], v[10], v[9], v[8], v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
        fflush(stdout);
        return 1;
    }
    else {
        return 0;
    }
}

// Let very processes to calculate how many circuit are legal with their distributed local count 
int calculate_legal_circuit_count(int id,int process_size) {

    int local_res = 0;

    for (int i = id; i <= USHRT_MAX; i+= process_size) {

        local_res += checkCircuit(id, i);
    }

    return local_res;
}

