#include <stdio.h>   // Standard Input/Output library for functions like printf and scanf.
#include <stdlib.h>   // Standard library for memory allocation, process control, conversions, etc.
#include <sys/mman.h>   
#include <string.h>   // String handling library for functions like strcpy and strcmp.  
#include <fcntl.h>      

#define MAX_TRACK_COUNT 20    
#define MEMORY_ALLOCATION_SIZE MAX_TRACK_COUNT * 4  

int trackRequests[MAX_TRACK_COUNT];  
signed char *memoryFilePointer;    

// Comparison function for sorting the requests in ascending order
int order_comparator(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// First-Come-First-Serve (FCFS) disk scheduling algorithm
void FCFS(int requests[], int start_position) {
  printf("\nFIRST COME FIRST SERVE DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    printf("%d, ", requests[i]);
    total_disk_movement += abs(requests[i] - current_head_location);
    current_head_location = requests[i];
  }

  printf("\n\nFCFS - Total head movements = %d\n", total_disk_movement);
}

// Shortest-Seek-Time-First (SSTF) disk scheduling algorithm
void SSTF(int sorted_track_requests[], int start_position) {
  printf("\nSHORTEST SEEK TIME FIRST DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  int processed_requests[MAX_TRACK_COUNT];
  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    processed_requests[i] = 0;
  }

  for (int count = 0; count < MAX_TRACK_COUNT; count++) {
    int nearest_request_index = -1;
    int nearest_distance = 300;

    for (int i = 0; i < MAX_TRACK_COUNT; i++) {
      if (processed_requests[i] == 0) {
        int track_distance = abs(sorted_track_requests[i] - current_head_location);
        if (track_distance < nearest_distance) {
          nearest_distance = track_distance;
          nearest_request_index = i;
        }
      }
    }

    processed_requests[nearest_request_index] = 1;
    printf("%d, ", sorted_track_requests[nearest_request_index]);
    total_disk_movement += nearest_distance;
    current_head_location = sorted_track_requests[nearest_request_index];
  }

  printf("\n\nSSTF - Total head movements = %d\n", total_disk_movement);
}

// SCAN disk scheduling algorithm
void SCAN(int sorted_track_requests[], int start_position, char *movement_direction) {
  printf("\nSCAN DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  if (strcmp(movement_direction, "RIGHT") == 0) {
    for (int i = 0; i < MAX_TRACK_COUNT; i++) {
      if (sorted_track_requests[i] >= current_head_location) {
        for (int j = i; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        total_disk_movement += abs(299 - current_head_location);
        current_head_location = 299;
        for (int j = i - 1; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  } else {
    for (int i = MAX_TRACK_COUNT - 1; i >= 0; i--) {
      if (sorted_track_requests[i] <= current_head_location) {
        for (int j = i; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        total_disk_movement += abs(current_head_location - 0);
        current_head_location = 0;
        for (int j = i + 1; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  }

  printf("\n\nSCAN - Total head movements = %d\n", total_disk_movement);
}

// C-SCAN disk scheduling algorithm
void CSCAN(int sorted_track_requests[], int start_position, char *movement_direction) {
  printf("\nCIRCULAR SCAN DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  if (strcmp(movement_direction, "RIGHT") == 0) {
    for (int i = 0; i < MAX_TRACK_COUNT; i++) {
      if (sorted_track_requests[i] >= current_head_location) {
        for (int j = i; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        total_disk_movement += abs(299 - current_head_location);
        current_head_location = 0;
        total_disk_movement += 299;
        for (int j = 0; j < i; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  } else {
    for (int i = MAX_TRACK_COUNT - 1; i >= 0; i--) {
      if (sorted_track_requests[i] <= current_head_location) {
        for (int j = i; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        total_disk_movement += abs(current_head_location - 0);
        current_head_location = 299;
        total_disk_movement += 299;
        for (int j = MAX_TRACK_COUNT - 1; j > i; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  }

  printf("\n\nC-SCAN - Total head movements = %d\n", total_disk_movement);
}

// LOOK disk scheduling algorithm
void LOOK(int sorted_track_requests[], int start_position, char *movement_direction) {
  printf("\nLOOK DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  if (strcmp(movement_direction, "RIGHT") == 0) {
    for (int i = 0; i < MAX_TRACK_COUNT; i++) {
      if (sorted_track_requests[i] >= current_head_location) {
        for (int j = i; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        for (int j = i - 1; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  } else {
    for (int i = MAX_TRACK_COUNT - 1; i >= 0; i--) {
      if (sorted_track_requests[i] <= current_head_location) {
        for (int j = i; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        for (int j = i + 1; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  }

  printf("\n\nLOOK - Total head movements = %d\n", total_disk_movement);
}

// C-LOOK disk scheduling algorithm
void CLOOK(int sorted_track_requests[], int start_position, char *movement_direction) {
  printf("\nC-LOOK DISK SCHEDULING\n\n");

  int total_disk_movement = 0;
  int current_head_location = start_position;

  if (strcmp(movement_direction, "RIGHT") == 0) {
    for (int i = 0; i < MAX_TRACK_COUNT; i++) {
      if (sorted_track_requests[i] >= current_head_location) {
        for (int j = i; j < MAX_TRACK_COUNT; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        for (int j = 0; j < i; j++) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  } else {
    for (int i = MAX_TRACK_COUNT - 1; i >= 0; i--) {
      if (sorted_track_requests[i] <= current_head_location) {
        for (int j = i; j >= 0; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        for (int j = MAX_TRACK_COUNT - 1; j > i; j--) {
          printf("%d, ", sorted_track_requests[j]);
          total_disk_movement += abs(sorted_track_requests[j] - current_head_location);
          current_head_location = sorted_track_requests[j];
        }
        break;
      }
    }
  }

  printf("\n\nC-LOOK - Total head movements = %d\n", total_disk_movement);
}

int main(int argc, char* argv[]) {

  int initial_track_position = atoi(argv[1]);
  char* movement_path = argv[2];

  int mapping_file_descriptor = open("request.bin", O_RDONLY);                          
  memoryFilePointer = mmap(0, MEMORY_ALLOCATION_SIZE, PROT_READ, MAP_PRIVATE, mapping_file_descriptor, 0);  
  
  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    memcpy(trackRequests + i, memoryFilePointer + 4*i, 4);
  }

  munmap(memoryFilePointer, MEMORY_ALLOCATION_SIZE); 

  int sorted_track_sequence[MAX_TRACK_COUNT];
  for (int i = 0; i < MAX_TRACK_COUNT; i++) {
    sorted_track_sequence[i] = trackRequests[i];
  }

  qsort(sorted_track_sequence, MAX_TRACK_COUNT, sizeof(int), order_comparator);

  printf("Total requests = %d\n", MAX_TRACK_COUNT);
  printf("Initial Head Position: %d\n", initial_track_position);
  printf("Direction of Head: %s\n", movement_path);

  FCFS(trackRequests, initial_track_position);
  SSTF(sorted_track_sequence, initial_track_position);
  SCAN(sorted_track_sequence, initial_track_position, movement_path);
  CSCAN(sorted_track_sequence, initial_track_position, movement_path);
  LOOK(sorted_track_sequence, initial_track_position, movement_path);
  CLOOK(sorted_track_sequence, initial_track_position, movement_path);
}