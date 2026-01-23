#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int main(int argc, char *argv[]) {
        if (argc < 2) {
        fprintf(2, "Usage: occup {number of pages}...\n");
        exit(1);
    }

    int num_pages = atoi(argv[1]);
    if(num_pages <= 0) {
        fprintf(2, "Error, enter positive number of pages!");
        exit(1);
    }

    int bytes = num_pages * 4096;

    int rc = fork();

    if (rc < 0) {
        printf("fork failed");
    }
    else if (rc == 0) {
        char *p = sbrk(bytes);
        if (*p == (char) - 1) {
            printf("sbrk failed. exiting\n");
            exit(1);
        }
        printf("Occupied memory: %d bytes.\n", bytes);
    }
    else {
        exit(0);
    }
    while (1) {
        pause(1000);
    }
    exit(0);
}