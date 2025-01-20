#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int amount = 0;  // variable for accoun balance.
pthread_mutex_t mutex;  // make the mutex lock

// retrievs the passed paramter as the deposit amount. 
void *deposit(void *param) {
    int deposit_amount = *(int *)param;

    // Lock the mutex before accessing and modifying the shared variable
    pthread_mutex_lock(&mutex);
    //Adds the deposit amount to amount and prints the new balance.
    amount += deposit_amount;
    printf("Deposit amount = %d\n", amount);
    //Unlock the mutex before accessing and modifying the shared variable
    pthread_mutex_unlock(&mutex);

    free(param);  

    pthread_exit(0);
}

// retrievs the passed paramter as the withdraw amount. 
void *withdraw(void *param) {
    int withdraw_amount = *(int *)param;

    pthread_mutex_lock(&mutex);
    //Subtracts the withdrawal amount from amount.
    amount -= withdraw_amount;
    printf("Withdrawal amount = %d\n", amount);
    pthread_mutex_unlock(&mutex);

    free(param);  // Free the allocated memory

    pthread_exit(0);
}

//Expects exactly two command-line arguments: deposit amount and withdrawal amount.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s deposit_amount withdraw_amount\n", argv[0]);
        exit(1);
    }

    // Convert command line arguments to integers
    int deposit_amount = atoi(argv[1]);
    int withdraw_amount = atoi(argv[2]);

    // Initialize the mutex lock
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex\n");
        exit(1);
    }

    // Creates an array withdraw_threads to hold thread identifiers.
    pthread_t withdraw_threads[3];
    pthread_t deposit_threads[3];


    // Starts three withdrawal threads in a loop:
    // Allocates memory for the withdrawal amount to be passed to each thread.
    // Checks for successful memory allocation.
    // Assigns the withdrawal amount and creates the thread using pthread_create.
    for (int i = 0; i < 3; i++) {
        int *w_amount = malloc(sizeof(int));
        if (w_amount == NULL) {
            fprintf(stderr, "Error allocating memory for withdraw amount\n");
            exit(1);
        }
        *w_amount = withdraw_amount;
        if (pthread_create(&withdraw_threads[i], NULL, withdraw, w_amount) != 0) {
            fprintf(stderr, "Error creating withdraw thread %d\n", i);
        }
    }

    // Waits for all withdrawal threads to complete using pthread_join.
    for (int i = 0; i < 3; i++) {
        pthread_join(withdraw_threads[i], NULL);
    }

    // Similar to withdrawal threads, but for deposit operations.
    for (int i = 0; i < 3; i++) {
        int *d_amount = malloc(sizeof(int));
        if (d_amount == NULL) {
            fprintf(stderr, "Error allocating memory for deposit amount\n");
            exit(1);
        }
        *d_amount = deposit_amount;
        if (pthread_create(&deposit_threads[i], NULL, deposit, d_amount) != 0) {
            fprintf(stderr, "Error creating deposit thread %d\n", i);
        }
    }

    // Wait for all deposit threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(deposit_threads[i], NULL);
    }

    // Destroys the mutex lock using pthread_mutex_destroy.
    // Prints the final account balance.
    pthread_mutex_destroy(&mutex);

    // Output the final amount
    printf("Final amount = %d\n", amount);

    return 0;
}

