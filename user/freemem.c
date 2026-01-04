#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int main(void) {
    uint64 mem = freemem();
    printf("Free memory: %ld bytes\n", mem);
    exit(0);
}