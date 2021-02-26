# Debug

If you encounter segfaults or memory leaks, please check with your grammar implementation.
If you believe the issue origins from Peppa PEG and want to debug it, this document can be helpful.

## Checking Segfaults

Assuming your operation system has gdb or lldb installed. Depending on your toolchain, use gdb if using gcc, use lldb if using llvm. After installing a debugger, you can create a simple program that recurs the segfault problem.

For example,

```c
#include "peppapeg.h"
int main() {
    P4_Grammar* grammar = NULL;
    printf("%lu\n", grammar->count);
}
```

Compile the program with debugging option enabled:

```bash
$ gcc -g peppapeg.c debug.c
```

Run the program with a debugger. Gdb directive `run` can run the program, `where` can output where the program stops, and `p` can print the value of the variable.


```bash
$ gdb ./a.out
(gdb) run
Starting program: /app/a.out
warning: Error disabling address space randomization: Operation not permitted

Program received signal SIGSEGV, Segmentation fault.
0x000055c62cc11e6e in main () at debug.c:4
4	    printf("count: %u\n", grammar->count);
(gdb) where
#0  0x000055c62cc11e6e in main () at debug.c:4
(gdb) p grammar
$1 = (P4_Grammar *) 0x0
(gdb) quit
```

In this case, it's `0x0` so a segfault occurred. To fix it, create the grammar using `P4_CreateGrammar()`.

## Checking Memory Leaks

You can check memory leaks using Valgrind. For example, the below code is still not about right:

```c
#include "peppapeg.h"
int main() {
    P4_Grammar* grammar = P4_CreateGrammar();
    printf("%lu\n", grammar->count);
}
```

Compile the program again with debugging option enabled and run it with valgrind:

```bash
$ gcc -g peppapeg.c debug.c
$ valgrind --leak-check=yes --track-origins=yes ./a.out
==8253== Memcheck, a memory error detector
==8253== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==8253== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==8253== Command: ./a.out
==8253==
==8253==
==8253== HEAP SUMMARY:
==8253==     in use at exit: 64 bytes in 1 blocks
==8253==   total heap usage: 1 allocs, 0 frees, 64 bytes allocated
==8253==
==8253== 64 bytes in 1 blocks are definitely lost in loss record 1 of 1
==8253==    at 0x483B7F3: malloc (in /usr/lib/x86_64-linux-gnu/valgrind/vgpreload_memcheck-amd64-linux.so)
==8253==    by 0x10BEF0: P4_CreateGrammar (peppapeg.c:1242)
==8253==    by 0x10DE46: main (debug.c:3)
==8253==
==8253== LEAK SUMMARY:
==8253==    definitely lost: 64 bytes in 1 blocks
==8253==    indirectly lost: 0 bytes in 0 blocks
==8253==      possibly lost: 0 bytes in 0 blocks
==8253==    still reachable: 0 bytes in 0 blocks
==8253==         suppressed: 0 bytes in 0 blocks
==8253==
==8253== For lists of detected and suppressed errors, rerun with: -s
==8253== ERROR SUMMARY: 1 errors from 1 contexts (suppressed: 0 from 0)
```

The message is verbose but contains all the info for debugging. `1 allocs, 0 frees` indicates a malloc has no corresponding free. `64 bytes in 1 blocks are definitely lost in loss ... by 0x10DE46: main (debug.c:3)` tells us the exact location that creates a memory leak.

## Conclusion

Programs are not always correct as human makes mistakes.
By crafting the debugging skills using tools like gdb/lldb/valgrind, we will follow the trace, pin point the problem, and fix the bug 🐛.
