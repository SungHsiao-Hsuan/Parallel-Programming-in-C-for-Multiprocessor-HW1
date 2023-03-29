#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h> 

// Calculate how many tosses in the circle within distributed tosses for each process
long long int is_in_circle();

int main() {

	long long int number_in_circle = 0;
	long long int total_tosses = 0;
	long long int local_number_in_circle;

	int comm_sz = 0;
	int my_rank = 0;

	double startTime = 0.0, totalTime = 0.0;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	// Make every round would get difference number of tosses in the circle
	time_t t;
	srand((unsigned)time(&t) + my_rank);
	
	if (my_rank == 0) {

		local_number_in_circle = 0;
		
		// Scan total tosses number from keyboard
		scanf("%lld", &total_tosses);

		startTime = MPI_Wtime();

		if (comm_sz == 2) {

			// Send total tosses to other processes
			for (int i = 1; i <= 1; i++) {
				MPI_Send(&total_tosses, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
			}

		}
		else if (comm_sz > 2){
			for (int i = 1; i <= 2; i++) {
				MPI_Send(&total_tosses, 1, MPI_LONG_LONG_INT, i, 0, MPI_COMM_WORLD);
			}
		}


		// Caculate the number of tosses in the circle
		for (int i = my_rank; i < total_tosses; i += comm_sz) {
			local_number_in_circle += is_in_circle();
		}
		
	}
	else {
;
		
		// Tree-structed communication of passing total tosses
		int recv_partner = (my_rank - 1) / 2;
		int send_partner1 = my_rank * 2 + 1;
		int send_partner2 = my_rank * 2 + 2;

		// Receive total tosses
		MPI_Recv(&total_tosses, 1, MPI_LONG_LONG_INT, recv_partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Send total tosses to own partner
		if (my_rank * 2 + 1 < comm_sz) {
			MPI_Send(&total_tosses, 1, MPI_LONG_LONG_INT, send_partner1, 0, MPI_COMM_WORLD);
		}

		if (my_rank * 2 + 2 < comm_sz) {
			MPI_Send(&total_tosses, 1, MPI_LONG_LONG_INT, send_partner2, 0, MPI_COMM_WORLD);
		}

 	        startTime = MPI_Wtime();

		// Caculate the number of tosses in the circle
		local_number_in_circle = 0;

		for (int i = my_rank; i < total_tosses; i += comm_sz) {
			local_number_in_circle += is_in_circle();
		}

	}

	// Use tree-structured communication to send local sum to process 0 
	int m = comm_sz;

	number_in_circle = local_number_in_circle;

	while (m > 1) {

		int r = m % 2; // check whether procesess count could be divide by 2
		
		m = m / 2; // Devide proceeses to receive local sum part and send local sum part

		if (my_rank < m) {
			MPI_Recv(&local_number_in_circle, 1, MPI_LONG_LONG_INT, my_rank + m + r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			number_in_circle += local_number_in_circle;

			m = m + r; //Calculate how many processes would partipate in the next round

		}
		else if (my_rank >= m + r) {
			MPI_Send(&number_in_circle, 1, MPI_LONG_LONG_INT, my_rank - m - r, 0, MPI_COMM_WORLD);
			totalTime = MPI_Wtime() - startTime;

			m = 1; // After send the local count, the process finished the task then could leave the while loop to wait other processes.
			printf("Process %d finished in time %f secs.\n", my_rank, totalTime);
		}
		else {
			m = m + r;
		}

	}

	totalTime = MPI_Wtime() - startTime;

	if (my_rank == 0) {
		
		totalTime = MPI_Wtime() - startTime;
		printf("Process %d finished in time %f secs.\n", my_rank, totalTime);
		
		double ans =  (double)(4 * number_in_circle) / total_tosses;
		printf("PI = %f\n", ans);
	}
	
	MPI_Finalize();
	return 0;
}

// Calculate how many tosses in the circle within distributed tosses for each process
long long int is_in_circle() {

	int min_value = -1;
	int max_value = 1;

	double x = ((max_value - min_value) * (double)rand() / (RAND_MAX)) + min_value;
	double y = ((max_value - min_value) * (double)rand() / (RAND_MAX)) + min_value;
	double distance_squared = x * x + y * y;

	//printf("x = %f, y = %f, d = %f\n", x, y, distance_squared);

	return  (distance_squared <= 1) ? 1 : 0;
		
}
