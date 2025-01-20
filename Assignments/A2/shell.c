#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80  /* The maximum length of  command is 80 */
#define HISTORY_SIZE 5  /* The size of the command history buffer is 5 */

char history[HISTORY_SIZE][MAX_LINE];  /* History buffer */
int history_count = 0;  /* This is a counter for the number of commands in history */
int total_command_count = 0;  /* Total number of commands entered will be shown by total_command_count */

/* Function to add a full command with arguments to history */
void add_to_history(char *command) {
    if (strcmp(command, "history") == 0 || strcmp(command, "!!") == 0) {
        return;  // Don't add 'history' or '!!' to history
    }

    total_command_count++;  // Increment total number of commands

    if (history_count < HISTORY_SIZE) {
        strcpy(history[history_count], command);
        history_count++;
    } else {
        // Shift commands up and replace the oldest to maintain correct order and numbering.
        for (int i = 1; i < HISTORY_SIZE; i++) {
            strcpy(history[i - 1], history[i]);
        }
        strcpy(history[HISTORY_SIZE - 1], command);
    }
}

/* The `display_history()` function checks if there are any commands in history,
 and if so, it prints the commands stored in the history array in reverse order,
starting from the most recent, with their corresponding command numbers based on
`total_command_count`. If the history is empty, it prints "No commands in history." */
void display_history() {
    if (history_count == 0) {
        printf("No commands in history.\n");
        return;
    }
    int history_number = total_command_count;  // Start number from total_command_count

    for (int i = history_count - 1; i >= 0; i--) {
        printf("%d %s\n", history_number--, history[i]);
    }
}



/* The `get_most_recent_command()` function returns the most recent command
 from the history by accessing the last entry in the `history` array. If no
 commands exist in the history (i.e., `history_count` is 0), it returns `NULL`.*/
char* get_most_recent_command() {
    if (history_count == 0) {
        return NULL;  // No commands in history
    }
    return history[history_count - 1];
}

int main(void) {
    char *args[MAX_LINE/2 + 1];  /* Command line arguments */
    int should_run = 1;          /* This is to determine when to exit program */

    while (should_run) {
        printf("osh> ");
        fflush(stdout);

        char input[MAX_LINE];
        fgets(input, MAX_LINE, stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline

        // If 'exit' command was entered
        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        // If 'history' command was entered
        if (strcmp(input, "history") == 0) {
            display_history();
            continue;
        }

        // If '!!' command was entered
        if (strcmp(input, "!!") == 0) {
            char *recent_command = get_most_recent_command();
            if (recent_command == NULL) {
                printf("No commands in history.\n");
                continue;
            }
            printf("%s\n", recent_command);  // Echo the recent command
            strcpy(input, recent_command);   // Set input to the recent command
        }

        // We tokenize input into arguments
        int i = 0;
        char *token = strtok(input, " ");
        int background = 0;
        char full_command[MAX_LINE] = "";  // To store the full command

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;  // Set background flag if '&' is found
            } else {
                args[i] = token;
                i++;

                // Append each token to full_command
                strcat(full_command, token);
                strcat(full_command, " ");
            }
            token = strtok(NULL, " ");
        }
        args[i] = NULL;  // Null-terminate the arguments array

        // Add the full command with arguments to history
        add_to_history(full_command);

        // Finally we fork child process
        pid_t pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork failed\n");
            return 1;
        } else if (pid == 0) {
            // Child process
            if (execvp(args[0], args) == -1) {
                perror("osh");
            }
            return 0;
        } else {
            // Parent process waits unless background process
            if (!background) {
                wait(NULL);
            }
        }
    }

    return 0;
}

