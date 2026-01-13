#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int parse(char *s, int *a, char *op, int *b) {
    *a = atoi(s);

    while (*s && *s != ' ') {
        s++;
    }
    while (*s == ' ') {
        s++;
    }
    
    *op = *s;
    s++;

    while (*s == ' ') {
        s++;
    }

    *b = atoi(s);
    s++;
    return 0;
}

int main(void) {
    char type[99] = {0};
    int a = 0, b = 0;
    char op = 0; 
    // int res;
    printf("======= Calculator ========");
    printf("Type |calculate| to perform mathematical calculation, or |exit| to exit this program\n");
    read(0, type, sizeof(type));

    parse(type, &a, &op, &b);

    printf("Parsed: %d, %c, %d", a, op, b);

    // if (type[0] == ' ') {
    //     printf("Type something\n");
    //     exit(0);
    // }
    if (strcmp(type, "exit")) {
        printf("Exiting program.\n");
        exit(0);
    }
}