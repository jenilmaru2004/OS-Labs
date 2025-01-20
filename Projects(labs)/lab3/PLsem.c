#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

// Shared variable representing the bank account balance
int amount = 0;

// Mutex lock for mutual exclusion
pthread_mutex_t mutex;

// Semaphores to control deposits and withdrawals
sem_t empty_slots; // Represents available capacity for deposits
sem_t full_slots;  // Represents available amount for withdrawals

void *deposit(void *param) {
    printf("Executing deposit function\n");

    // Wait on empty_slots semaphore (wait if amount >= 400)
    if (sem_wait(&empty_slots) != 0) {
        fprintf(stderr, "Error waiting on empty_slots semaphore\n");
        pthread_exit(NULL);
    }

    // Lock the mutex before modifying the shared variable
    if (pthread_mutex_lock(&mutex) != 0) {
        fprintf(stderr, "Error locking mutex\n");
        pthread_exit(NULL);
    }

    // Critical section: deposit amount
    amount += 100;
    printf("Amount after deposit = %d\n", amount);

    // Unlock the mutex after modification
    if (pthread_mutex_unlock(&mutex) != 0) {
        fprintf(stderr, "Error unlocking mutex\n");
        pthread_exit(NULL);
    }

    // Signal full_slots semaphore (increment available withdrawals)
    if (sem_post(&full_slots) != 0) {
        fprintf(stderr, "Error posting to full_slots semaphore\n");
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}

void *withdraw(void *param) {
    printf("Executing Withdraw function\n");

    // Wait on full_slots semaphore (wait if amount <= 0)
    if (sem_wait(&full_slots) != 0) {
        fprintf(stderr, "Error waiting on full_slots semaphore\n");
        pthread_exit(NULL);
    }

    // Lock the mutex before modifying the shared variable
    if (pthread_mutex_lock(&mutex) != 0) {
        fprintf(stderr, "Error locking mutex\n");
        pthread_exit(NULL);
    }

    // Critical section: withdraw amount
    amount -= 100;
    printf("Amount after Withdrawal = %d\n", amount);

    // Unlock the mutex after modification
    if (pthread_mutex_unlock(&mutex) != 0) {
        fprintf(stderr, "Error unlocking mutex\n");
        pthread_exit(NULL);
    }

    // Signal empty_slots semaphore (increment available deposits)
    if (sem_post(&empty_slots) != 0) {
        fprintf(stderr, "Error posting to empty_slots semaphore\n");
        pthread_exit(NULL);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s 100\n", argv[0]);
        exit(1);
    }

    int transaction_amount = atoi(argv[1]);
    if (transaction_amount != 100) {
        fprintf(stderr, "Transaction amount must be 100\n");
        exit(1);
    }

    // Initialize the mutex lock
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        fprintf(stderr, "Error initializing mutex\n");
        exit(1);
    }

    // Initialize semaphores
    if (sem_init(&empty_slots, 0, 4) != 0) { // Capacity for deposits
        fprintf(stderr, "Error initializing empty_slots semaphore\n");
        exit(1);
    }
    if (sem_init(&full_slots, 0, 0) != 0) {  // Amount available for withdrawals
        fprintf(stderr, "Error initializing full_slots semaphore\n");
        exit(1);
    }

    pthread_t deposit_threads[7];
    pthread_t withdraw_threads[3];

    // Create 7 deposit threads
    for (int i = 0; i < 7; i++) {
        if (pthread_create(&deposit_threads[i], NULL, deposit, NULL) != 0) {
            fprintf(stderr, "Error creating deposit thread %d\n", i);
        }
    }

    // Create 3 withdraw threads
    for (int i = 0; i < 3; i++) {
        if (pthread_create(&withdraw_threads[i], NULL, withdraw, NULL) != 0) {
            fprintf(stderr, "Error creating withdraw thread %d\n", i);
        }
    }

    // Wait for all deposit threads to finish
    for (int i = 0; i < 7; i++) {
        pthread_join(deposit_threads[i], NULL);
    }

    // Wait for all withdraw threads to finish
    for (int i = 0; i < 3; i++) {
        pthread_join(withdraw_threads[i], NULL);
    }

    // Destroy the mutex lock and semaphores
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);

    // Output the final amount
    printf("Final amount = %d\n", amount);

    return 0;
}
