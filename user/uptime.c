#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>


int main(void) {
    int time = uptime();
    printf("Uptime: %d\n", time);
    exit(0);
}