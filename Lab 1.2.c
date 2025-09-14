#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define NUM_COMMANDS 5

int main() {
    // Comand List
    char *commands[NUM_COMMANDS][3] = {
        {"ls", "-l", NULL},
        {"date", NULL, NULL},
        {"whoami", NULL, NULL},
        {"pwd", NULL, NULL},
        {"ps", NULL, NULL}
    };

    pid_t pids[NUM_COMMANDS];
    int status;
    struct timeval start, end;
    double total_time = 0.0;

    printf("===== Process Management Project =====\n");
    printf("Parent PID: %d\n\n", getpid());

    gettimeofday(&start, NULL); // Total Time

    for (int i = 0; i < NUM_COMMANDS; i++) {
        struct timeval child_start, child_end;
        gettimeofday(&child_start, NULL);

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            // Proceso hijo
            printf("[Child %d] Starting command: %s\n", getpid(), commands[i][0]);
            if (execvp(commands[i][0], commands[i]) == -1) {
                fprintf(stderr, "[Child %d] Exec failed: %s\n", getpid(), strerror(errno));
                exit(EXIT_FAILURE);
            }
        }
        else {
            // Saves PID
            pids[i] = pid;
            waitpid(pid, &status, 0);//waits the child process
            gettimeofday(&child_end, NULL);

            double elapsed = (child_end.tv_sec - child_start.tv_sec) * 1000.0;
            elapsed += (child_end.tv_usec - child_start.tv_usec) / 1000.0;
            total_time += elapsed;

            printf("[Parent] Child PID: %d | Command: %s | Exit status: %d | Time: %.2f ms\n",
                   pid, commands[i][0], WEXITSTATUS(status), elapsed);
        }
    }

    gettimeofday(&end, NULL); // Total Time
    double elapsed_total = (end.tv_sec - start.tv_sec) * 1000.0;
    elapsed_total += (end.tv_usec - start.tv_usec) / 1000.0;

    printf("\n===== Summary =====\n");
    printf("Total execution time: %.2f ms\n", elapsed_total);

    return 0;
}
/*Process Creation (fork()) - The parent spawns child processes. Each child executes a command, while the parent tracks them.

Command Execution (execvp()) - Children run external programs (ls, date, whoami, etc.). If execution fails, errors are reported.

Process Synchronization (waitpid()) - The parent waits for each child, ensuring orderly completion and capturing exit statuses.

Execution Time (gettimeofday()) - The program measures both individual child execution times and total runtime.

Information Display - Outputs parent PID, each child’s PID, command, exit status, execution time, and total execution time.

Error Handling - Handles fork and exec failures gracefully with meaningful messages.*/