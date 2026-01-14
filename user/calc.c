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
    // int res;
    printf("======= Calculator ========\n");
    printf("Type |calculate| to perform mathematical calculation, or |exit| to exit this program\n");
    read(0, type, sizeof(type));


    // if (type[0] == ' ') {
    //     printf("Type something\n");
    //     exit(0);
    // }
    if (!strcmp(type, "exit")) {
        printf("Exiting program.\n");
        exit(0);
    }

    parse(type, &a, &op, &b);
    printf("Parsed: %d, %c, %d\n", a, op, b);
}