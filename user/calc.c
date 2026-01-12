#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int main(void) {
    char type[99] = {0};
    int a = 0, b = 0;
    char op = 0; 
    // int res;
    printf("======= Calculator ========");
    printf("Type |calculate| to perform mathematical calculation, or |exit| to exit this program");
    read(0, type, sizeof(type));

    parser(type, &a, &op, &b);

    printf("Parsed: %d, %c, %d", a, op, b);

    // if (type[0] == ' ') {
    //     printf("Type something\n");
    //     exit(0);
    // }
}