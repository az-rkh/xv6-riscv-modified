#include <kernel/types.h>
#include <kernel/stat.h>

int main(char *s, int *a, char *op, int *b) {
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