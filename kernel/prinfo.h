#ifndef _PRINFO_H
#define _PRINFO_H

struct prinfo {
  int pid;
  char name[16];
  int state;
  uint64 vruntime;
};

#endif
