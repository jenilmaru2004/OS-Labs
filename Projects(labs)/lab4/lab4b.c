#include <stdio.h>      // Standard I/O functions

/*They enable you to use the open() system call which is used to open a new file and obtain its file descriptor.*/

#include <stdlib.h>     // Standard library functions
#include <fcntl.h>      // For open()

/*To be able to use mmap() system call and the memcpy() function you need to add the
below header files.*/

#include <sys/mman.h>   // For mmap() and munmap()
#include <string.h>     // For memcpy()
#include <unistd.h>     // For close()

/* define ‘MEMORY_SIZE’ as a macro definition. It is the total number of bytes you
will be copying from numbers.bin file.*/

#define INT_SIZE 4                     // Size of an integer in bytes
#define INT_COUNT 10                   // Number of integers in the file
#define MEMORY_SIZE (INT_COUNT * INT_SIZE) // Total memory size to map

/*Define  integer array and a signed character pointer to store the
starting address of the memory mapped file.*/

int intArray[INT_COUNT];               // Array to store integers
signed char *mmapfptr;                 // Pointer to the mapped memory

int main() {
    // Step 3: Open the binary file
    /* Open the file (numbers.bin) using the open() system call. Since we  will be simply
        reading this file use the O_RDONLY option.*/
    int mmapfile_fd = open("numbers.bin", O_RDONLY);
    if (mmapfile_fd == -1) {
        perror("Error opening numbers.bin");
        exit(EXIT_FAILURE);
    }

    // Step 4: Memory map the file
    /* Use the mmap() system call to memory map this file. */
    mmapfptr = mmap(0, MEMORY_SIZE, PROT_READ, MAP_PRIVATE, mmapfile_fd, 0);
    if (mmapfptr == MAP_FAILED) {
        perror("Error mapping the file");
        close(mmapfile_fd);
        exit(EXIT_FAILURE);
    }

    // Step 5: Copy data from mapped memory to intArray using memcpy() function.
    /* Retrieve the contents of the memory mapped file (using a loop) and store it in the integer
        array using memcpy() function. */
    for (int i = 0; i < INT_COUNT; i++) {
        memcpy(&intArray[i], mmapfptr + INT_SIZE * i, INT_SIZE);
    }

    // Step 6: Unmap the memory mapped file
    /* Unmap the memory mapped file using the unmap() system call. */
    if (munmap(mmapfptr, MEMORY_SIZE) == -1) {
        perror("Error unmapping the file");
        close(mmapfile_fd);
        exit(EXIT_FAILURE);
    }

    // Step 7: Close the file descriptor
    close(mmapfile_fd);

    // Step 8: Sum the integers and print the result
    /* Loop through intArray to add all numbers in the array and output this sum to the
console. */
    int sum = 0;
    for (int i = 0; i < INT_COUNT; i++) {
        sum += intArray[i];
    }
    printf("Sum of numbers = %d\n", sum);

    return 0;
}

