#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

// Helper function to clean the content, preserving meaningful spaces and line breaks
void clean_content(char *str) {
    char *src = str;
    char *dst = str;
    
    // Skip leading whitespaces
    while (*src && isspace((unsigned char)*src)) {
        src++;
    }

    // Copy the content while keeping necessary spaces
    while (*src) {
        if (!isspace((unsigned char)*src) || *src == '\n' || *src == '\r' || *src == '\t') {
            *dst++ = *src;  // Copy non-whitespace characters or newlines
        }
        src++;
    }
    *dst = '\0';  // Null-terminate the cleaned string
}

// Function to split a string into lines and return each line separately
void get_lines(char *str, char lines[][BUFFER_SIZE], int *line_count) {
    char *line = strtok(str, "\n");
    while (line != NULL) {
        strcpy(lines[*line_count], line);
        (*line_count)++;
        line = strtok(NULL, "\n");
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <txt_file> <executable>\n", argv[0]);
        return 1;
    }

    char *txt_file = argv[1];
    char *executable = argv[2];

    // Open the .txt file
    FILE *file = fopen(txt_file, "r");
    if (!file) {
        perror("Error opening .txt file");
        return 1;
    }

    // Read the .txt file contents into a buffer
    char txt_content[BUFFER_SIZE];
    size_t txt_len = fread(txt_content, 1, BUFFER_SIZE - 1, file);
    txt_content[txt_len] = '\0'; // Null-terminate
    fclose(file);

    // Clean the .txt content to ignore unwanted whitespaces, but preserve formatting
    clean_content(txt_content);

    // Run the executable and capture its output
    char command[BUFFER_SIZE];
    snprintf(command, sizeof(command), "./%s", executable);

    FILE *process = popen(command, "r");
    if (!process) {
        perror("Error running executable");
        return 1;
    }

    // Read the executable output into a buffer
    char exec_output[BUFFER_SIZE];
    size_t exec_len = fread(exec_output, 1, BUFFER_SIZE - 1, process);
    exec_output[exec_len] = '\0'; // Null-terminate
    pclose(process);

    // Clean the executable output to ignore unwanted whitespaces, but preserve formatting
    clean_content(exec_output);

    // Split the cleaned contents into lines
    char txt_lines[BUFFER_SIZE][BUFFER_SIZE];
    char exec_lines[BUFFER_SIZE][BUFFER_SIZE];
    int txt_lines_count = 0;
    int exec_lines_count = 0;

    get_lines(txt_content, txt_lines, &txt_lines_count);
    get_lines(exec_output, exec_lines, &exec_lines_count);

    // If the contents match, print "true"
    if (txt_lines_count == exec_lines_count && strcmp(txt_content, exec_output) == 0) {
        printf("true\n");
        return 0;
    } else {
        printf("false\n");
        return 1;
    }
}
