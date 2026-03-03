# Linux Kernel Process Inspect Module

A Linux Kernel Module that exposes process information via /proc using the seq_file interface.

This project is built for learning:

- Kernel module structure
- procfs interface
- seq_file API
- Process traversal
- Userspace ↔ Kernel communication

------------------------------------------------------------
WHAT THIS MODULE DOES
------------------------------------------------------------

Creates:

    /proc/proc_inspect

User can:

1) Show all processes
   sudo cat /proc/proc_inspect

2) Show specific PID
   echo 1 | sudo tee /proc/proc_inspect
   sudo cat /proc/proc_inspect

3) Switch back to all
   echo all | sudo tee /proc/proc_inspect

------------------------------------------------------------
BUILD
------------------------------------------------------------

make

If Secure Boot enabled:

sudo /usr/src/linux-headers-$(uname -r)/scripts/sign-file sha256 \
~/kernel_keys/MOK.key \
~/kernel_keys/MOK.crt \
proc_inspect.ko

------------------------------------------------------------
LOAD MODULE
------------------------------------------------------------

sudo insmod proc_inspect.ko

Check:

lsmod | grep proc_inspect

------------------------------------------------------------
REMOVE MODULE
------------------------------------------------------------

sudo rmmod proc_inspect

------------------------------------------------------------
DEBUGGING
------------------------------------------------------------

Check kernel logs:
dmesg | tail

------------------------------------------------------------
UNDERSTANDING /proc
------------------------------------------------------------

/proc is a virtual filesystem.

It:
- Does NOT exist on disk
- Is generated dynamically by the kernel
- Exposes process and system information

Examples:
- /proc/<pid>/
- /proc/cpuinfo
- /proc/meminfo
- /proc/sys/

Header used:
#include <linux/proc_fs.h>

This header allows:
- Creating entries in /proc
- Removing entries
- Defining read/write handlers
- Working with struct proc_dir_entry

------------------------------------------------------------
/proc vs sysfs
------------------------------------------------------------

/proc:
- Originally for process information
- Flexible free-form text
- Not stable ABI
- Mostly legacy or debugging

sysfs:
- Designed for device model
- Strict hierarchy
- One value per file
- Stable userspace ABI
- Recommended for modern drivers

When to use sysfs:
- New device drivers
- Hardware configuration
- Stable userspace interface

When to use /proc:
- Debugging
- Statistics
- Quick testing
- Legacy support

------------------------------------------------------------
WHAT IS seq_file?
------------------------------------------------------------

seq_file is a kernel helper interface used to safely print large sequential output.

It handles:
- Buffering
- Partial reads
- Pagination
- Position tracking

Flow when user runs:

cat /proc/proc_inspect
        ↓
Kernel read triggered
        ↓
proc_open()
        ↓
seq_file allocates buffer
        ↓
proc_show() uses seq_printf()
        ↓
User sees output

------------------------------------------------------------
MACRO vs FUNCTION (Kernel Perspective)
------------------------------------------------------------

Macro:
- Defined using #define
- Compile-time substitution
- No type checking
- No runtime memory

Function:
- Has return type
- Executed at runtime
- Type checked
- Occupies text section memory

Rule of thumb:

If you see:
#define → macro

If you see:
static int / void / struct → function

------------------------------------------------------------
NEXT IMPROVEMENTS
------------------------------------------------------------

Planned upgrades:

[ ] Add memory usage (RSS)
[ ] Add CPU time (utime/stime)
[ ] Add thread listing
[ ] Add kernel logging
[ ] Convert from /proc to sysfs
[ ] Add module parameters
[ ] Add filtering by UID
[ ] Add pagination for very large systems

------------------------------------------------------------
EDUCATIONAL PURPOSE
------------------------------------------------------------

This project is intended for deep Linux kernel learning.

Tested on Ubuntu with standard kernel headers.

Author: Amit Joshi
License: GPL v2

