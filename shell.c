/**
 * Peppa PEG -  Ultra lightweight PEG Parser in ANSI C.
 *
 * MIT License
 *
 * Copyright (c) 2021 Ju
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file       shell.c
 * @brief      Peppa PEG command-line interface
 * @author     Ju Lin
 * @copyright  MIT
 * @date       2021
 * @see        https://github.com/soasme/PeppaPEG
*/

# ifndef P4_SHELL_H
# define P4_SHELL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <peppa.h>

# define read_arg(argc, argv, opts, c) (-1 != ((c) = getopt((argc), (argv), (opts))))

static void version(const char* name) {
    printf("%s version %s\n", name, P4_Version());
}

static void usage(const char* name) {
    version(name);
    printf("usage: %s [OPTION]... [FILE]...\n", name);
    printf(
        "OPTION:\n"
        "  -h\tprint help information\n"
        "  -V\tprint version\n"
    );
}

int main(int argc, char* argv[]) {
    int c;
    while (read_arg(argc, argv, "Vhg:", c)) {
        switch (c) {
            case 'V':
                version(basename(argv[0]));
                exit(0);
            case 'h':
                usage(basename(argv[0]));
                exit(0);
            default:
                fprintf(stderr, "for usage try: %s -h\n", argv[0]);
                exit(1);
        }
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

# endif
