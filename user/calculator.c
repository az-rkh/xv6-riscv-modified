#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int main(void) {
    char buffer[20];
    int abort = 0;
    printf("======= Calculator ========");
    printf("Type |calculate| to perform mathematical calculation, or |exit| to exit this program");

    if (readline(strcmp(buffer, sizeof(buffer))) <= 0) {
        abort = 1;
        exit(0);
    }

    if (strcmp(buffer, "exit") == 0) {
        printf("Program terminated.\n");
        abort = 1;
        exit(0);
    }

    if (strcmp(buffer, "calculate") != 0) {
        printf("Unknown command\n");
        abort = 1;
        exit(1);
    } 
    
    while (abort == 0) {
        int num1, num2;
        int operator;
        
    }

    while (abort == 0) {
        double a, b;
        char op;
        int sum;
        
    }
}