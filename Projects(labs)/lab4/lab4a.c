#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 10
#define OFFSET_MASK 0xFFF   // Mask for 12 bits (offset)
#define OFFSET_BITS 12      // Number of bits for the offset
#define PAGE_SIZE 4096      // Page size (2^12)
#define PAGES 8             // Total number of pages

int main() {
    // Part I.1 - Reading from a file
    FILE *fptr = fopen("labaddr.txt", "r");
    if (fptr == NULL) {
        printf("Error opening labaddr.txt\n");
        return -1;
    }

    char buff[BUFFER_SIZE];

    // Part I.3 - Define the page table
    int page_table[PAGES] = {6, 4, 3, 7, 0, 1, 2, 5};

    // Read from labaddr.txt till end of file
    while (fgets(buff, BUFFER_SIZE, fptr) != NULL) {
        // Convert the string read into an integer (logical address)
        int virtual_address = atoi(buff);

        // Part I.2 - Compute page number and offset
        int page_number = virtual_address >> OFFSET_BITS;
        int offset = virtual_address & OFFSET_MASK;

        // Part I.3 - Compute physical address
        int frame_number = page_table[page_number];
        int physical_address = (frame_number << OFFSET_BITS) | offset;

        // Output results
        printf("Virtual addr is %d: Page# = %d & Offset = %d. Physical addr = %d.\n",
               virtual_address, page_number, offset, physical_address);
    }

    // Close the file
    fclose(fptr);

    return 0;
}
