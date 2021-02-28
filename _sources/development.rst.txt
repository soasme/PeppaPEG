Development
===========

Debug
-----

If you encounter segfaults or memory leaks, please check with your grammar implementation.
If you believe the issue origins from Peppa PEG and want to debug it, this document can be helpful.

Checking Segfaults
``````````````````

Assuming your operation system has gdb or lldb installed. Depending on your toolchain, use gdb if using gcc, use lldb if using llvm. After installing a debugger, you can create a simple program that recurs the segfault problem.

For example,

.. code-block:: c

    #include "peppapeg.h"
    int main() {
        P4_Grammar* grammar = NULL;
        printf("%lu\n", grammar->count);
    }

Compile the program with debugging option enabled:

.. code-block:: console

    $ gcc -g peppapeg.c debug.c

Run the program with a debugger. Gdb directive `run` can run the program, `where` can output where the program stops, and `p` can print the value of the variable.


.. code-block:: console

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

In this case, it's `0x0` so a segfault occurred. To fix it, create the grammar using `P4_CreateGrammar()`.

Checking Memory Leaks
`````````````````````

Assuming your operation system has gdb or lldb installed. Depending on your toolchain, use gdb if using gcc, use lldb if using llvm. After installing a debugger, you can create a simple program that recurs the segfault problem.

For example,

.. code-block:: c

    #include "peppapeg.h"
    int main() {
        P4_Grammar* grammar = NULL;
        printf("%lu\n", grammar->count);
    }

Compile the program with debugging option enabled:

.. code-block:: console

    $ gcc -g peppapeg.c debug.c

Run the program with a debugger. Gdb directive `run` can run the program, `where` can output where the program stops, and `p` can print the value of the variable.


.. code-block:: console

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

In this case, it's `0x0` so a segfault occurred. To fix it, create the grammar using `P4_CreateGrammar()`.

Conclusion
``````````

Programs are not always correct as human makes mistakes.
By crafting the debugging skills using tools like gdb/lldb/valgrind, we will follow the trace, pin point the problem, and fix the bug 🐛.


