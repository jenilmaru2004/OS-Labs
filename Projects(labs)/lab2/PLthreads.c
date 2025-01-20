#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// Define the size of the list
#define SIZE 20

// Global array to store the numbers
int numbers[SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

// Structure to hold the start and end indices for each thread
typedef struct {
    int from_index;
    int to_index;
} parameters;

// Global variable to store the sum computed by each thread
int partial_sum[2];

// Function executed by each thread
void* runner(void* param) {
    parameters* data = (parameters*) param;
    int sum = 0;

    // Sum the elements in the range from from_index to to_index
    for (int i = data->from_index; i <= data->to_index; i++) {
        sum += numbers[i];
    }

    // Store the computed sum in the appropriate position (0 for first thread, 1 for second thread)
    if (data->from_index == 0) {
        partial_sum[0] = sum;
    } else {
        partial_sum[1] = sum;
    }

    // Free the dynamically allocated memory
    free(data);
    pthread_exit(0);
}

int main() {
    pthread_t tid[2]; // Array to hold thread IDs
    pthread_attr_t attr; // Thread attributes

    // Initialize the thread attribute
    pthread_attr_init(&attr);

    // Create two threads, each summing half of the list
    for (int i = 0; i < 2; i++) {
        // Allocate memory for parameters to pass to the thread
        parameters* data = (parameters*) malloc(sizeof(parameters));

        // Set the range for the thread (0 to 9 for the first thread, 10 to 19 for the second thread)
        if (i == 0) {
            data->from_index = 0;
            data->to_index = (SIZE / 2) - 1;
        } else {
            data->from_index = SIZE / 2;
            data->to_index = SIZE - 1;
        }

        // Create the thread and pass the data as an argument
        pthread_create(&tid[i], &attr, runner, data);
    }

    // Wait for both threads to finish
    for (int i = 0; i < 2; i++) {
        pthread_join(tid[i], NULL);
    }

    // Sum the results from both threads
    int total_sum = partial_sum[0] + partial_sum[1];

    // Output the total sum
    printf("%d\n", total_sum);

    return 0;
}
