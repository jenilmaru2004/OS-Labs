#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define TLB_SIZE 16
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 32768 // 32,768 bytes = 128 frames * 256 bytes/frame
#define NUMBER_OF_FRAMES (PHYSICAL_MEMORY_SIZE / FRAME_SIZE)

// Struct for TLB entries
typedef struct {
    int page_number;
    int frame_number;
} TLBEntry;

// Global variables
signed char *backing_store;
TLBEntry tlb[TLB_SIZE];

// Function prototypes
int search_TLB(int page_number);
void TLB_Add(int page_number, int frame_number, int *tlb_index);
void update_TLB_on_page_replacement(int old_page_number);
void handle_page_fault(int page_number, signed char physical_memory[][FRAME_SIZE], int *page_table, int *frame_table, int *next_frame);

int main(int argc, char *argv[]) {
    FILE *address_file;
    int logical_address;
    int page_number;
    int offset;
    int frame_number;
    int physical_address;
    int tlb_hits = 0;
    int page_faults = 0;
    int total_addresses = 0;

    int page_table[PAGE_TABLE_SIZE];
    int tlb_index = 0; // Points to the next TLB entry to replace (FIFO)

    // Physical memory
    signed char physical_memory[NUMBER_OF_FRAMES][FRAME_SIZE];
    int frame_table[NUMBER_OF_FRAMES]; // Keeps track of which page is stored in each frame
    int next_frame = 0; // Points to the next frame to replace in physical memory (FIFO)

    // Initialize page table, TLB, and frame table
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].page_number = -1;
        tlb[i].frame_number = -1;
    }
    for (int i = 0; i < NUMBER_OF_FRAMES; i++) {
        frame_table[i] = -1;
    }

    // Open addresses.txt
    address_file = fopen("addresses.txt", "r");
    if (address_file == NULL) {
        fprintf(stderr, "Error opening addresses.txt\n");
        return EXIT_FAILURE;
    }

    // Open BACKING_STORE.bin
    int backing_store_fd = open("BACKING_STORE.bin", O_RDONLY);
    if (backing_store_fd == -1) {
        fprintf(stderr, "Error opening BACKING_STORE.bin\n");
        fclose(address_file);
        return EXIT_FAILURE;
    }

    // Memory-map the backing store
    backing_store = mmap(0, PAGE_TABLE_SIZE * FRAME_SIZE, PROT_READ, MAP_PRIVATE, backing_store_fd, 0);
    if (backing_store == MAP_FAILED) {
        fprintf(stderr, "Error mapping BACKING_STORE.bin\n");
        fclose(address_file);
        close(backing_store_fd);
        return EXIT_FAILURE;
    }

    // Read logical addresses
    while (fscanf(address_file, "%d", &logical_address) != EOF) {
        total_addresses++;

        // Extract page number and offset
        page_number = (logical_address >> 8) & 0xFF; // Upper 8 bits
        offset = logical_address & 0xFF;             // Lower 8 bits

        // TLB lookup
        frame_number = search_TLB(page_number);
        if (frame_number != -1) {
            // TLB hit
            tlb_hits++;
            // Optional: Debug statement
            // printf("TLB Hit: Page %d found in Frame %d\n", page_number, frame_number);
        } else {
            // TLB miss, check page table
            frame_number = page_table[page_number];
            if (frame_number == -1) {
                // Page fault handling
                page_faults++;
                handle_page_fault(page_number, physical_memory, page_table, frame_table, &next_frame);
                frame_number = page_table[page_number];
            }
            // Add the page and frame number to the TLB
            TLB_Add(page_number, frame_number, &tlb_index);
        }

        // Calculate physical address
        physical_address = (frame_number << 8) | offset;

        // Get the signed byte value from physical memory
        signed char value = physical_memory[frame_number][offset];

        // Optionally, print the addresses and value
        printf("Logical Address: %d, Physical Address: %d, Value: %d\n", logical_address, physical_address, value);
    }

    printf("\nStatistics:\n");
    printf("Total addresses: %d\n", total_addresses);
    printf("TLB Hits: %d\n", tlb_hits);
    printf("TLB Hit Rate: %.2f%%\n", (tlb_hits / (float)total_addresses) * 100);
    printf("Page Faults: %d\n", page_faults);
    printf("Page Fault Rate: %.2f%%\n", (page_faults / (float)total_addresses) * 100);

    // Clean up
    fclose(address_file);
    munmap(backing_store, PAGE_TABLE_SIZE * FRAME_SIZE);
    close(backing_store_fd);

    return 0;
}

// Function to search the TLB for a page number
int search_TLB(int page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page_number == page_number) {
            return tlb[i].frame_number;
        }
    }
    return -1; // Not found
}

// Function to add an entry to the TLB using FIFO replacement
void TLB_Add(int page_number, int frame_number, int *tlb_index) {
    tlb[*tlb_index % TLB_SIZE].page_number = page_number;
    tlb[*tlb_index % TLB_SIZE].frame_number = frame_number;
    (*tlb_index)++;
    // Optional: Debug statement
    // printf("TLB Add: Page %d mapped to Frame %d\n", page_number, frame_number);
}

// Function to update the TLB when a page is replaced in physical memory
void update_TLB_on_page_replacement(int old_page_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].page_number == old_page_number) {
            // Invalidate the TLB entry
            tlb[i].page_number = -1;
            tlb[i].frame_number = -1;
            // Optional: Debug statement
            // printf("TLB Update: Invalidated Page %d from TLB\n", old_page_number);
            return;
        }
    }
}

// Function to handle a page fault
void handle_page_fault(int page_number, signed char physical_memory[][FRAME_SIZE], int *page_table, int *frame_table, int *next_frame) {
    // Determine the frame number using FIFO replacement policy
    int frame_number = *next_frame % NUMBER_OF_FRAMES;

    // Check if the frame is already occupied
    if (frame_table[frame_number] != -1) {
        int old_page_number = frame_table[frame_number];

        // Optional: Debug statement
        // printf("Page Fault: Replacing Page %d with Page %d in Frame %d\n", old_page_number, page_number, frame_number);

        // Invalidate the old page's entry in the page table
        page_table[old_page_number] = -1;

        // Update TLB only if the old page was present in the TLB
        update_TLB_on_page_replacement(old_page_number);
    } else {
        // Optional: Debug statement
        // printf("Page Fault: Loading Page %d into Frame %d (Empty Frame)\n", page_number, frame_number);
    }

    // Copy the page from backing store to physical memory
    memcpy(physical_memory[frame_number], backing_store + (page_number * FRAME_SIZE), FRAME_SIZE);

    // Update the page table
    page_table[page_number] = frame_number;

    // Update the frame table
    frame_table[frame_number] = page_number;

    // Increment the frame pointer for FIFO
    (*next_frame)++;
}
