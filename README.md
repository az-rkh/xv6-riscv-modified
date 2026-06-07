# xv6-riscv (Extended)

A modified version of MIT's xv6 operating system for RISC-V, extended with custom system calls, user utilities, and kernel features.

## Overview

xv6 is a teaching operating system developed by MIT, based on Dennis Ritchie's and Ken Thompson's Unix Version 6. This fork extends the base xv6-riscv implementation with additional functionality demonstrating kernel development, system call implementation, and user-space programming.

## Custom Extensions

### System Calls

| Syscall | Number | Description |
|---------|--------|-------------|
| `freemem` | 24 | Returns available physical memory in MiB |
| `getprocs` | 27 | Retrieves process table information for process listing |

### User Programs

| Program | Description |
|---------|-------------|
| `freemem` | Displays available system memory by querying the kernel's free page allocator |
| `ps` | Lists all processes with PID, name, state, and vruntime |
| `uptime` | Shows system uptime in seconds |
| `calc` | Interactive calculator supporting basic arithmetic operations |
| `pingpong` | Demonstrates inter-process communication using pipes |
| `occup` | Memory stress-testing utility that allocates and holds specified pages |

### Kernel Features

**Buddy Allocator**
- Replaces xv6's default free-list allocator with a buddy system allocator
- Manages physical memory in power-of-two sized blocks (order 0 = 1 page up to order 15)
- `kfree` coalesces freed blocks with their buddies upward until no further merging is possible
- `kalloc` scans free lists from order 0 upward, splits larger blocks down to the needed size
- Per-page metadata (`is_free`, `is_head`, `order`) tracks block state for coalescing decisions

**CFS-like Scheduler**
- Replaces xv6's default round-robin scheduler with a fairness-based policy inspired by Linux's Completely Fair Scheduler (CFS)
- Each process tracks a `vruntime` field (virtual runtime) that increments on every timer tick
- New processes inherit the minimum `vruntime` among all existing processes to prevent starvation
- Scheduler always picks the `RUNNABLE` process with the lowest `vruntime`

**Shell Command History**
- Navigate previous commands using up/down arrow keys
- Circular buffer storing up to 16 commands
- Implemented via escape sequence parsing in the console driver

## Technical Implementation

### Memory Information (`freemem`)

The `freemem` syscall traverses the kernel's free page list to calculate available memory:

```c
uint64 freemem(void) {
  int free_pages = 0;
  struct run *r;
  acquire(&kmem.lock);
  r = kmem.freelist;
  while (r != 0) {
    free_pages++;
    r = r->next;
  }
  release(&kmem.lock);
  return (uint64)free_pages * PGSIZE / 1024 / 1024;
}
```

### Process Listing (`getprocs`)

The `getprocs` syscall iterates the process table and copies process information to user space:

- Acquires per-process locks to ensure consistency
- Populates `struct prinfo` with PID, name, and state
- Uses `copyout()` for safe kernel-to-user data transfer

### Command History

Arrow key navigation implemented in the kernel console driver:
- Parses ANSI escape sequences (ESC [ A/B) for arrow key detection
- Maintains history buffer with circular indexing
- Handles line editing and display refresh

## Building and Running

### Prerequisites

- RISC-V GNU toolchain (`riscv64-unknown-elf-gcc`)
- QEMU compiled for `riscv64-softmmu`

### Build

```bash
make clean
make qemu
```

### Usage Examples

```bash
$ freemem
Free memory: 127 MiB

$ ps
PID     NAME            STATE           VRUNTIME
1       init            SLEEP           42
2       sh              RUNNING         39

$ uptime
Uptime: 42,7 seconds

$ calc
calc> 15 * 4
Result: 60
calc> exit
```

## Project Structure

```
kernel/
├── kalloc.c      # Buddy allocator (kfree coalescing, kalloc splitting)
├── proc.c        # Process management (getprocs, CFS scheduler, vruntime)
├── trap.c        # Trap handler (vruntime increment on timer interrupt)
├── console.c     # Console driver (command history)
├── sysproc.c     # System call implementations
└── prinfo.h      # Process info structure definition

user/
├── freemem.c     # Free memory display utility
├── ps.c          # Process listing utility
├── uptime.c      # System uptime utility
├── calc.c        # Interactive calculator
├── pingpong.c    # IPC demonstration
└── occup.c       # Memory occupation utility
```

## Acknowledgments

Based on [xv6-riscv](https://github.com/mit-pdos/xv6-riscv) by MIT PDOS.

## References

- [MIT 6.1810 Operating System Engineering](https://pdos.csail.mit.edu/6.1810/)
- [xv6: a simple, Unix-like teaching operating system](https://pdos.csail.mit.edu/6.828/2023/xv6/book-riscv-rev3.pdf)
