#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>


int main(void) {
    int ticks = uptime();

    int seconds = ticks / 10;

    int decimal = ticks % 10;

    printf("Uptime: %d,%d seconds\n", seconds, decimal);
    exit(0);
}