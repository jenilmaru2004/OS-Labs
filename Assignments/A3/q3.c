/* 
 * To compile: gcc -pthread -o q3 q3.c
 * To run: ./q3
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define NUM_STUDENTS 5      // The number of studens are 5.
#define NUM_CHAIRS 3        // The number of chairs in the hallway are 3.
#define MAX_HELP 5          // Each student seeks help 5 times.

sem_t students_sem;         // Counts the number of waiting students.
sem_t ta_sem;               // Signals the TA to help a student.
pthread_mutex_t mutex_lock; // Mutex lock for shared variables.

int waiting_students = 0;   // Number of students waiting.
int all_done = 0;           // Flag to indicate all students are done.

void *student(void *num);
void *ta(void *);

int main() {
    int i;
    pthread_t ta_thread;
    pthread_t student_threads[NUM_STUDENTS];
    int student_ids[NUM_STUDENTS];

    srand(time(NULL)); // Seed random number generator.

    // We initialize the semaphores and mutex.
    sem_init(&students_sem, 0, 0);
    sem_init(&ta_sem, 0, 0);
    pthread_mutex_init(&mutex_lock, NULL);

    // Create the TA thread for simulating the TA's role in helping students during office hours.
    pthread_create(&ta_thread, NULL, ta, NULL);

    // Create student threads.
    // Student IDs start from 0 here.
    for (i = 0; i < NUM_STUDENTS; i++) {
        student_ids[i] = i; 
        pthread_create(&student_threads[i], NULL, student, (void *)&student_ids[i]);
    }

    // Wait for student threads to finish.
    for (i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(student_threads[i], NULL);
    }

    // All students are done.
    all_done = 1;
    sem_post(&students_sem); // Wake up TA if sleeping.

    // Wait for TA thread to finish.
    pthread_join(ta_thread, NULL);

    // Destroy the mutex lock, clean up and exit.
    pthread_mutex_destroy(&mutex_lock);
    sem_destroy(&students_sem);
    sem_destroy(&ta_sem);

    return 0;
}

/*
The student function simulates a student thread that alternates between programming and seeking help from the TA.
When needing help, the student checks for an available chair; if one is available, they take a seat and wait for the TA, 
otherwise, they continue programming and will try again later. This process repeats until the student has received help 
a specified number of times, after which the student thread terminates.*/
void *student(void *number) {
    int id = *(int *)number;
    int help_count = 0;

    while (help_count < MAX_HELP) {
        // Programming for a random time.
        sleep(rand() % 5 + 1);

        // Try to get help from the TA.
        pthread_mutex_lock(&mutex_lock);
        if (waiting_students < NUM_CHAIRS) {
            waiting_students++;
            printf("                Student %d takes a seat waiting = %d\n", id, waiting_students);

            // This gives signal to the TA that a student is waiting.
            sem_post(&students_sem);
            pthread_mutex_unlock(&mutex_lock);

            // Wait for the TA to help.
            sem_wait(&ta_sem);

            // Getting help from the TA.
            help_count++;
        } else {
            // No chairs available, student will try later.
            printf("                        Student %d will try later\n", id);
            pthread_mutex_unlock(&mutex_lock);
        }
    }

    return NULL;
}

/*The ta function represents the Teaching Assistant's thread that continuously waits for students needing help.
When a student signals they're waiting, the TA assists by simulating help for a random duration and then signals
the student that assistance is complete. This process repeats until all students have received help the required
number of times, after which the TA thread terminates.
*/
void *ta(void *arg) {
    while (!all_done || waiting_students > 0) {
        // We wait for a student to arrive.
        sem_wait(&students_sem);

        if (all_done && waiting_students == 0)
            break;

        // Acquire the mutex to modify waiting_students.
        pthread_mutex_lock(&mutex_lock);
        if (waiting_students > 0) {
            // To decrease the number of waiting students.
            waiting_students--;

            int helpTime = rand() % 5 + 1;
            printf("Helping a student for %d seconds waiting students = %d\n", helpTime, waiting_students);

            // This gives signal to the student that the TA is ready to help.
            sem_post(&ta_sem);
            pthread_mutex_unlock(&mutex_lock);

            // This will simulate helping the student.
            sleep(helpTime);
        } else {
            pthread_mutex_unlock(&mutex_lock);
        }
    }

    return NULL;
}

