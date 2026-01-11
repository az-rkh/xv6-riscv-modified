#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

int readline(char *buf, int max) {
    int n = read(0, buf, max - 1);
    if (n <= 0) {
        return n;
    }

    buf[n] = 0;

    if (n > 0 && buf[n-1] == '\n') {
        buf[n-1] = 0;
    }
    return n;
}