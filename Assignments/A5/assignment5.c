// assignment5.c
#include <stdio.h>      // Standard Input/Output library for functions like printf and scanf.
#include <stdlib.h>     // Standard library for memory allocation, process control, conversions, etc.
#include <string.h>     // String handling library for functions like strcpy and strcmp.

#define MAX_CYLINDERS 300   // Define the maximum number of cylinders (0 to 299) on the disk.
#define REQUESTS 20         // Define the total number of disk requests to be handled.

int requests[REQUESTS];         // Array to store the disk requests read from the binary file.
int sorted_requests[REQUESTS];  // Array to store the sorted disk requests for certain algorithms.

// Comparison function used by qsort to sort the disk requests in ascending order.
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// This function reads 20 integer disk requests from the binary file "request.bin" into the 'requests' array.
// It opens the file in binary read mode, checks for errors, reads the data into 'requests', and then closes the file.
void read_requests_from_file() {
    FILE *file = fopen("request.bin", "rb");
    if (file == NULL) {
        perror("Error opening request.bin");
        exit(EXIT_FAILURE);
    }
    if (fread(requests, sizeof(int), REQUESTS, file) != REQUESTS) {
        perror("Error reading requests from request.bin");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    fclose(file);
}

// Implements the First-Come, First-Served (FCFS) disk scheduling algorithm.
// It services disk requests in the order they arrive and calculates the total head movement.
void fcfs(int head) {
    printf("\nFCFS DISK SCHEDULING ALGORITHM:\n\n");
    int total_movement = 0;
    int current_head = head;

    for (int i = 0; i < REQUESTS; i++) {
        printf("%d", requests[i]);
        if (i != REQUESTS - 1) {
            printf(", ");
        }
        total_movement += abs(requests[i] - current_head);
        current_head = requests[i];
    }

    printf("\n\nFCFS - Total head movements = %d\n", total_movement);
}

// Implements the Shortest Seek Time First (SSTF) disk scheduling algorithm.
// It services the nearest disk request to the current head position and calculates the total head movement.
void sstf(int head) {
    printf("\nSSTF DISK SCHEDULING ALGORITHM\n\n");
    int total_movement = 0;
    int current_head = head;
    int serviced[REQUESTS] = {0};
    int serviced_order[REQUESTS];

    for (int i = 0; i < REQUESTS; i++) {
        int min_distance = MAX_CYLINDERS + 1;
        int index = -1;
        for (int j = 0; j < REQUESTS; j++) {
            if (!serviced[j]) {
                int distance = abs(requests[j] - current_head);
                if (distance < min_distance) {
                    min_distance = distance;
                    index = j;
                }
            }
        }
        serviced[index] = 1;
        serviced_order[i] = requests[index];
        total_movement += abs(requests[index] - current_head);
        current_head = requests[index];
    }

    for (int i = 0; i < REQUESTS; i++) {
        printf("%d", serviced_order[i]);
        if (i != REQUESTS - 1) {
            printf(", ");
        }
    }

    printf("\n\nSSTF - Total head movements = %d\n", total_movement);
}

// Implements the SCAN disk scheduling algorithm.
// It moves the disk head in one direction, servicing requests until it reaches the end,
// then reverses direction and continues servicing any remaining requests.
void scan(int head, char *direction) {
    printf("\nSCAN DISK SCHEDULING ALGORITHM \n\n");
    int total_movement = 0;
    int current_head = head;

    printf("%d", head); // Print initial head position.

    // We separate requests into left of head and right of head.
    int left_requests[REQUESTS];
    int right_requests[REQUESTS];
    int left_count = 0;
    int right_count = 0;

    for (int i = 0; i < REQUESTS; i++) {
        if (sorted_requests[i] < head) {
            left_requests[left_count++] = sorted_requests[i];
        } else if (sorted_requests[i] > head) {
            right_requests[right_count++] = sorted_requests[i];
        }
    }

    if (strcmp(direction, "LEFT") == 0) {
        // We move left from the head.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }

        // We move to cylinder 0.
        if (current_head != 0) {
            total_movement += current_head; // Movement from current_head to 0.
            current_head = 0;
        }

        // We reverse direction, move right, servicing requests to the right of head.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }
    } else {
        // We move right from the head.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }

        // We move to cylinder MAX_CYLINDERS - 1.
        if (current_head != MAX_CYLINDERS - 1) {
            total_movement += (MAX_CYLINDERS - 1) - current_head;
            current_head = MAX_CYLINDERS - 1;
        }

        // We reverse direction, move left, servicing requests to the left of head.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }
    }

    printf("\n\nSCAN - Total head movements = %d\n", total_movement);
}

// Implements the C-SCAN disk scheduling algorithm.
// It moves the disk head in one direction, servicing requests until it reaches the end,
// then jumps to the opposite end without servicing any requests during the jump,
// and continues servicing in the same direction, effectively creating a circular path.
void c_scan(int head, char *direction) {
    printf("\nC-SCAN DISK SCHEDULING ALGORITHM \n\n");
    int total_movement = 0;
    int current_head = head;

    // We print initial head position.
    printf("%d", head);

    // We separate requests into left and right of head.
    int left_requests[REQUESTS];
    int right_requests[REQUESTS];
    int left_count = 0;
    int right_count = 0;

    for (int i = 0; i < REQUESTS; i++) {
        if (sorted_requests[i] < head) {
            left_requests[left_count++] = sorted_requests[i];
        } else if (sorted_requests[i] > head) {
            right_requests[right_count++] = sorted_requests[i];
        }
    }

    if (strcmp(direction, "LEFT") == 0) {
        // We move left from the head.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }

        // We move to cylinder 0.
        if (current_head != 0) {
            total_movement += current_head; // Movement from current_head to 0.
            current_head = 0;
        }

        // We jump from cylinder 0 to MAX_CYLINDERS -1 (299).
        total_movement += (MAX_CYLINDERS - 1 - current_head); // Movement from 0 to 299.
        current_head = MAX_CYLINDERS - 1;

        // Service requests greater than head, in decreasing order.
        for (int i = right_count -1; i >= 0; i--) {
            printf(", %d", right_requests[i]);
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }
    } else {
        // We move right from the head, servicing requests greater than head.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }

        // We move to the last cylinder.
        if (current_head != MAX_CYLINDERS - 1) {
            total_movement += (MAX_CYLINDERS - 1 - current_head);
            current_head = MAX_CYLINDERS - 1;
        }

        // We jump to the beginning (cylinder 0).
        total_movement += current_head; // Movement from MAX_CYLINDERS -1 to 0.
        current_head = 0;

        // Service requests less than head, in increasing order.
        for (int i = 0; i < left_count; i++) {
            printf(", %d", left_requests[i]);
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }
    }

    printf("\n\nC-SCAN - Total head movements = %d\n", total_movement);
}

// Implements the LOOK disk scheduling algorithm.
// The disk head moves in one direction, servicing all requests until it reaches the last request in that direction,
// then reverses direction without going to the end of the disk, servicing the remaining requests.
void look(int head, char *direction) {
    printf("\nLOOK DISK SCHEDULING ALGORITHM \n\n");
    int total_movement = 0;
    int current_head = head;

    // We print initial head position
    printf("%d", head);

    // We separate requests into left of head and right of head.
    int left_requests[REQUESTS];
    int right_requests[REQUESTS];
    int left_count = 0;
    int right_count = 0;

    for (int i = 0; i < REQUESTS; i++) {
        if (sorted_requests[i] < head) {
            left_requests[left_count++] = sorted_requests[i];
        } else if (sorted_requests[i] > head) {
            right_requests[right_count++] = sorted_requests[i];
        }
        // We exclude requests equal to the head position to avoid duplication.
    }

    int serviced_order[REQUESTS];
    int count = 0;

    if (strcmp(direction, "LEFT") == 0) {
        // Service requests to the left of head in decreasing order.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            serviced_order[count++] = left_requests[i];
        }
        // Then service requests to the right of head in increasing order.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            serviced_order[count++] = right_requests[i];
        }
    } else {
        // Service requests to the right of head in increasing order.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            serviced_order[count++] = right_requests[i];
        }
        // Then service requests to the left of head in decreasing order.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            serviced_order[count++] = left_requests[i];
        }
    }

    // Calculate total head movement.
    total_movement = 0;
    current_head = head;

    for (int i = 0; i < count; i++) {
        total_movement += abs(current_head - serviced_order[i]);
        current_head = serviced_order[i];
    }

    printf("\n\nLOOK - Total head movements = %d\n", total_movement);
}


// Implements the C-LOOK disk scheduling algorithm.
// The disk head moves in one direction, servicing all requests,
// then jumps to the furthest request on the opposite end without servicing any in between,
// and continues servicing in the same initial direction.
void c_look(int head, char *direction) {
    printf("\nC-LOOK DISK SCHEDULING ALGORITHM \n\n");
    int total_movement = 0;
    int current_head = head;

    // We print initial head position.
    printf("%d", head);

    // We separate requests into left of head and right of head.
    int left_requests[REQUESTS];
    int right_requests[REQUESTS];
    int left_count = 0;
    int right_count = 0;

    for (int i = 0; i < REQUESTS; i++) {
        if (sorted_requests[i] < head) {
            left_requests[left_count++] = sorted_requests[i];
        } else if (sorted_requests[i] > head) {
            right_requests[right_count++] = sorted_requests[i];
        }
        // We exclude requests equal to head to avoid duplication..
    }

    int serviced_order[REQUESTS];
    int count = 0;

    if (strcmp(direction, "LEFT") == 0) {
        // Service requests to the left of head in decreasing order.
        for (int i = left_count - 1; i >= 0; i--) {
            printf(", %d", left_requests[i]);
            serviced_order[count++] = left_requests[i];
        }

        // Jump to the highest request on the right.
        if (right_count > 0) {
            // Movement from last left request to highest right request is a jump.
            // Include this movement in total_movement but not in serviced requests.
            total_movement += abs(current_head - left_requests[0]);
            current_head = right_requests[right_count - 1];
            total_movement += abs(left_requests[0] - current_head);
        }

        // Service requests to the right of head in decreasing order.
        for (int i = right_count - 1; i >= 0; i--) {
            printf(", %d", right_requests[i]);
            serviced_order[count++] = right_requests[i];
        }

    } else {
        // Service requests to the right of head in increasing order.
        for (int i = 0; i < right_count; i++) {
            printf(", %d", right_requests[i]);
            serviced_order[count++] = right_requests[i];
        }

        // Jump to the lowest request on the left.
        if (left_count > 0) {
            total_movement += abs(current_head - right_requests[right_count - 1]);
            current_head = left_requests[0];
            total_movement += abs(right_requests[right_count - 1] - current_head);
        }

        // Service requests to the left of head in increasing order.
        for (int i = 0; i < left_count; i++) {
            printf(", %d", left_requests[i]);
            serviced_order[count++] = left_requests[i];
        }
    }

    // Calculate total head movement.
    total_movement = 0;
    current_head = head;

    if (strcmp(direction, "LEFT") == 0) {
        // Move left, servicing requests.
        for (int i = left_count - 1; i >= 0; i--) {
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }
        // Jump to highest request on the right.
        if (right_count > 0) {
            total_movement += abs(current_head - right_requests[right_count - 1]);
            current_head = right_requests[right_count - 1];
        }
        // Service right requests in decreasing order.
        for (int i = right_count - 2; i >= 0; i--) {
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }
    } else {
        // Move right, servicing requests.
        for (int i = 0; i < right_count; i++) {
            total_movement += abs(current_head - right_requests[i]);
            current_head = right_requests[i];
        }
        // Jump to lowest request on the left.
        if (left_count > 0) {
            total_movement += abs(current_head - left_requests[0]);
            current_head = left_requests[0];
        }
        // Service left requests in increasing order.
        for (int i = 1; i < left_count; i++) {
            total_movement += abs(current_head - left_requests[i]);
            current_head = left_requests[i];
        }
    }

    printf("\n\nC-LOOK - Total head movements = %d\n", total_movement);
}

// Main function that simulates the disk scheduling.
// It reads command-line arguments, initializes data, and calls each scheduling algorithm function.
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: ./assignment5 <Initial Head Position> <Direction>\n");
        return 1;
    }

    int head = atoi(argv[1]);      // Convert the initial head position from string to integer.
    char direction[10];
    strcpy(direction, argv[2]);    // Copy the direction argument ("LEFT" or "RIGHT") into 'direction'.

    // Validate the direction argument to ensure it is either "LEFT" or "RIGHT".
    if (strcmp(direction, "LEFT") != 0 && strcmp(direction, "RIGHT") != 0) {
        printf("Error: Direction must be either 'LEFT' or 'RIGHT'.\n");
        return 1;
    }

    read_requests_from_file();     // Read disk requests from the binary file into the 'requests' array.

    // Copy requests to sorted_requests for sorting.
    for (int i = 0; i < REQUESTS; i++) {
        sorted_requests[i] = requests[i];
    }

    // Sort the 'sorted_requests' array in ascending order using qsort.
    qsort(sorted_requests, REQUESTS, sizeof(int), compare);

    printf("Total requests = %d \n", REQUESTS);
    printf("Initial Head Position: %d \n", head);
    printf("Direction of Head: %s \n", direction);

    // We call each disk scheduling algorithm function with the initial head position and direction.
    fcfs(head);
    sstf(head);
    scan(head, direction);
    c_scan(head, direction);
    look(head, direction);
    c_look(head, direction);

    return 0;
}