#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>

struct prinfo {
  int pid;
  char name[16];
  char state[16];
};

int main() {
    struct prinfo table[64];
    int n = getprocesses();
    printf("PID\tNAME\t\tSTATE\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%s\t\t%s\n", table[i].pid, table[i].name, table[i].state);
    }
    exit(0);
}