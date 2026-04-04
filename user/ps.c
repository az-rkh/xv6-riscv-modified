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
    printf("PID\tNAME\t\tSTATE\t\tVRUNTIME\n");
    for (int i = 0; i < n; i++) {
        if (table[i].pid > 0)
        printf("%d\t%s\t\t%s\t\t%ld\n", table[i].pid, table[i].name, states[table[i].state], table[i].vruntime);
    }
    exit(0);
}
