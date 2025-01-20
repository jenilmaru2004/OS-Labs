#include <stdio.h> //for standard input/output operations like printf, scanf, fopen, etc.
#include <stdlib.h> //for memory allocation (malloc, free), program control (exit).
#include <fcntl.h> //for low-level file operations like open, O_RDONLY, O_WRONLY, etc.
#include <sys/mman.h> //for mapping files into memory, such as mmap and munmap.
#include <unistd.h> //for file descriptors and system calls like read, write, close, and lseek.
#include <string.h> //for memory manipulation functions like memcpy, memset, strcmp, and strlen.

#define TLB_SIZE 16 //the TLB can store up to 16 page-frame mappings at any given time.
#define PAGE_TABLE_SIZE 256
#define FRAME_SIZE 256
#define PHYSICAL_MEMORY_SIZE 32768 // 32,768 bytes = 128 frames * 256 bytes/frame.
#define NUMBER_OF_FRAMES (PHYSICAL_MEMORY_SIZE / FRAME_SIZE)

// TLBEntry stores a page number and its corresponding frame number in the TLB.
// Represents an entry in the TLB for fast address translation.
typedef struct {
    int pageNumber;
    int frameNumber;
} TLBEntry;

// Global variables.
signed char *backing_store;
TLBEntry tlb[TLB_SIZE];

// Function prototypes
int searchTLB(int pageNumber); // Searches the TLB for the given page number; returns frame number if found, else -1.
void TLB_Add(int pageNumber, int frameNumber, int *tlb_index); // Adds a page-frame mapping to the TLB.
void updateTLB(int old_pageNumber); // Function to update the TLB when a page is replaced in physical memory.
void handlePageFault(int pageNumber, signed char physical_memory[][FRAME_SIZE], int *page_table, int *frame_table, int *next_frame); // Handles a page fault by loading the page into physical memory and updating tables.

// We initialize main function and set up variables to handle logical-to-physical address translation, track TLB hits, page faults, process address mappings from an input file, etc.
int main(int argc, char *argv[]) {
    FILE *address_file;
    int logical_address;
    int pageNumber;
    int offset;
    int frameNumber;
    int physical_address;
    int tlb_hits = 0;
    int page_faults = 0;
    int total_addresses = 0;

    int page_table[PAGE_TABLE_SIZE];
    int tlb_index = 0; // Points to the next TLB entry to replace (FIFO).


    signed char physical_memory[NUMBER_OF_FRAMES][FRAME_SIZE]; // represents the total physical memory available.
    int frame_table[NUMBER_OF_FRAMES]; // Keeps track of which page is stored in each frame.
    int next_frame = 0; // Points to the next frame to replace in physical memory (FIFO).

    // Initialize page table, TLB, and frame table.
    for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
        page_table[i] = -1;
    }
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].pageNumber = -1;
        tlb[i].frameNumber = -1;
    }
    for (int i = 0; i < NUMBER_OF_FRAMES; i++) {
        frame_table[i] = -1;
    }

    // Open addresses.txt, but we show an error if it doesn't.
    address_file = fopen("addresses.txt", "r");
    if (address_file == NULL) {
        fprintf(stderr, "Error opening addresses.txt\n");
        return EXIT_FAILURE;
    }

    // Open BACKING_STORE.bin, but we show an error if it doesn't.
    int backing_store_fd = open("BACKING_STORE.bin", O_RDONLY);
    if (backing_store_fd == -1) {
        fprintf(stderr, "Error opening BACKING_STORE.bin\n");
        return EXIT_FAILURE;
    }

    // Memory-map the backing store, but we show an error if it doesn't.
    backing_store = mmap(0, PAGE_TABLE_SIZE * FRAME_SIZE, PROT_READ, MAP_PRIVATE, backing_store_fd, 0);
    if (backing_store == MAP_FAILED) {
        fprintf(stderr, "Error mapping BACKING_STORE.bin\n");
        return EXIT_FAILURE;
    }

    // Read logical addresses.
    while (fscanf(address_file, "%d", &logical_address) != EOF) {
        total_addresses++;

        // Extract page number and offset.
        pageNumber = (logical_address >> 8) & 0xFF;
        offset = logical_address & 0xFF;

        // We lookup TLB using searchTLB.
        frameNumber = searchTLB(pageNumber);
        if (frameNumber != -1) {
            tlb_hits++; // TLB hit.
        } else {
            // TLB miss then we check page table.
            frameNumber = page_table[pageNumber];
            if (frameNumber == -1) {
                page_faults++; // Page fault handling.
                handlePageFault(pageNumber, physical_memory, page_table, frame_table, &next_frame);
                frameNumber = page_table[pageNumber];
            }
            TLB_Add(pageNumber, frameNumber, &tlb_index); // Add the page and frame number to the TLB.
        }

        physical_address = (frameNumber << 8) | offset; // We calculate physical address.
        signed char value = physical_memory[frameNumber][offset]; // We get the signed byte value from physical memory.
        printf("Virtual address: %d Physical address = %d Value=%d\n", logical_address, physical_address, value); // Finally, we print the addresses and value.
    }

    // Print statistics about the total address, page faults and tlb hits in the final output. 
    printf("Total addresses = %d\n", total_addresses);
    printf("Page_faults = %d\n", page_faults);
    printf("TLB Hits = %d\n", tlb_hits);



    // Close address file and unmap memory.
    fclose(address_file);
    munmap(backing_store, PAGE_TABLE_SIZE * FRAME_SIZE);
    close(backing_store_fd);

    return 0;
}

/*This function searches the TLB for a given pageNumber. 
If a matching entry is found, it returns the corresponding frameNumber. 
Otherwise, it returns -1 to indicate that the page is not in the TLB. */
int searchTLB(int pageNumber) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].pageNumber == pageNumber) {
            return tlb[i].frameNumber;
        }
    }
    return -1; // Not found
}

/* This function adds a new page-to-frame mapping to the TLB at the current index, 
using FIFO based on the TLB size. It then increments the tlb_index to prepare for 
the next addition.*/
void TLB_Add(int pageNumber, int frameNumber, int *tlb_index) {
    tlb[*tlb_index % TLB_SIZE].pageNumber = pageNumber;
    tlb[*tlb_index % TLB_SIZE].frameNumber = frameNumber;
    (*tlb_index)++;
}

// Function to update the TLB when a page is replaced in physical memory.
/* This function updates the TLB by invalidating any entry that matches the given `old_pageNumber`. 
It sets both the page number and frame number of the matching TLB entry to -1, effectively removing it from the TLB.*/
void updateTLB(int old_pageNumber) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].pageNumber == old_pageNumber) {
            tlb[i].pageNumber = -1; //Invalidate.
            tlb[i].frameNumber = -1; //Invalidate.
            return;
        }
    }
}

/* This function handles a page fault by loading the requested page into physical memory. It determines the frame to use ,
updates the page table and frame table accordingly, and copies the page from the backing store to memory. Additionally, 
it ensures consistency by invalidating the old page's entry and updating the TLB.*/
void handlePageFault(int pageNumber, signed char physical_memory[][FRAME_SIZE], int *page_table, int *frame_table, int *next_frame) {
    
    int frameNumber = *next_frame % NUMBER_OF_FRAMES; // Determine the frame number.

    // If the frame is occupied, we need to replace the page.
    if (frame_table[frameNumber] != -1) {
        int old_pageNumber = frame_table[frameNumber];
        page_table[old_pageNumber] = -1; // Invalidate the old page's entry in the page table.
        updateTLB(old_pageNumber); // To update TLB if old page exists in TLB.
    }

    memcpy(physical_memory[frameNumber], backing_store + pageNumber * FRAME_SIZE, FRAME_SIZE); // We copy the page from backing store to physical memory.

    page_table[pageNumber] = frameNumber; // To update the page table.
    frame_table[frameNumber] = pageNumber; // To update the frame table.
    (*next_frame)++;  // To update the next frame pointer.
}
