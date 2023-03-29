#include<stdio.h>
#include <stdlib.h>
#include <time.h>

long long int is_in_circle();

int main() {

    long long int number_in_circle = 0;
	long long int total_tosses = 0;
    time_t t;

    int seed = rand() / 2;
    srand((unsigned) time(&t));


    scanf_s("%lld", &total_tosses);

    double START,END;
    START = clock();

    for (int i = 0; i < total_tosses; i++) {
			number_in_circle += is_in_circle();
	}

    printf("%lld\n",number_in_circle);

    END = (double)(clock() - START) / CLOCKS_PER_SEC;

    printf ("Excute time: %.6f\n",END );

    double ans =  (double)(4 * number_in_circle) / total_tosses;
	printf("PI = %.6f\n",ans);

}

long long int is_in_circle() {

	int min_value = -1;
	int max_value = 1;

	double x = ((max_value - min_value) * (double)rand() / (RAND_MAX)) + min_value;
	double y = ((max_value - min_value) * (double)rand() / (RAND_MAX)) + min_value;
	double distance_squared = x * x + y * y;

	return  (distance_squared <= 1) ? 1 : 0;

}