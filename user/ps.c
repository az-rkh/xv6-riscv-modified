#include <kernel/types.h>
#include <kernel/stat.h>
#include <user/user.h>
#include <kernel/prinfo.h>

char *states[] = {
    "UNUSED",
    "SLEEP",
    "RUNNABLE",
    "RUNNING",
    "ZOMBIE"
};

int main() {
    struct prinfo table[64];
    int n = getprocs(table);
    printf("PID\tNAME\t\tSTATE\n");
    for (int i = 0; i < n; i++) {
        if (table[i].pid > 0)
        printf("%d\t%s\t\t%s\n", table[i].pid, table[i].name, states[table[i].state]);
    }
    exit(0);
}