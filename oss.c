#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void print_help() {
    printf("Usage: oss [-h] [-n proc] [-s simul] [-t iter]\n");
    printf("-h: Display this help message\n");
    printf("-n proc: Number of total children to launch\n");
    printf("-s simul: Maximum number of children to allow to run simultaneously\n");
    printf("-t iter: Number of iterations to pass to the user process\n");
}

int main(int argc, char *argv[]) {
    int proc = 0, simul = 0, iter = 0;
    int opt;

    while ((opt = getopt(argc, argv, "hn:s:t:")) != -1) {
        switch (opt) {
            case 'h':
                print_help();
                return EXIT_SUCCESS;
            case 'n':
                proc = atoi(optarg);
                break;
            case 's':
                simul = atoi(optarg);
                break;
            case 't':
                iter = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid option\n");
                print_help();
                return EXIT_FAILURE;
        }
    }

    
    if (proc <= 0 || simul <= 0 || iter <= 0) {
        fprintf(stderr, "Invalid parameters\n");
        print_help();
        return EXIT_FAILURE;
    }

    
    for (int i = 0; i < proc; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (pid == 0) { // Child process
            char iter_arg[10];
            sprintf(iter_arg, "%d", iter);
            execl("./user", "./user", iter_arg, NULL);
            perror("execl");
            return EXIT_FAILURE;
        } else {
            while (simul <= 0) {
                int status;
                wait(&status);
                simul++;
            }
            simul--;
        }
    }

    while (wait(NULL) > 0);

    return EXIT_SUCCESS;
}

