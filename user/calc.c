#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int parse(char *s, int *a, char *op, int *b) {
    *a = atoi(s);

    if (*s == ' ')
        s++;

    while (*s >= '0' && *s <= '9' && *s != '\0') {
        s++;
    }

    while (*s == ' ' && *s != '\0') {
        s++;
    }
    *op = *s;
    s++;


    if (*s != '\0') {
        s++;
    }

    while (*s == ' ' && *s != '\0') {
        s++;
    }

    *b = atoi(s);

    return 0;
}

int main(void) {
    char type[99] = {0};
    int a = 0, b = 0;
    char op = 0; 
    int res = 0;
    printf("======= Calculator ========\n");
    printf("Type |calculate| to perform mathematical calculation,\n or |exit| to exit this program\n");
    read(0, type, sizeof(type));
    
    if (!strcmp(type, 'exit')) {
        printf("Exiting program.\n");
        exit(0);
    }

    parse(type, &a, &op, &b);

    int abort = 0;

    while (abort == 0) {
        if (op == '+') {
            res = a + b;
        }
        else if (op == '-') {
            res = a - b;
        }
        else if (op == '*') {
            res = a * b;
        }
        else if (op == '/') {
            if (b == 0) {
                printf("cannot divide by 0");
                exit(0);
            }
            res = a / b;
        }
    }

    printf("Result: %d\n", res);
}