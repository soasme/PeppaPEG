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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include "peppa.h"

# define read_arg(argc, argv, opts, c) (-1 != ((c) = getopt((argc), (argv), (opts))))

typedef struct p4_args_t p4_args_t;

struct p4_args_t {
    char* executable;
    bool  help;
    bool  verbose;
    FILE* grammar_file;
    char* grammar_entry;
    size_t arguments_count;
    char** arguments;
};

static void version(const char* name) {
    printf("%s version %s\n", name, P4_Version());
}

static void usage(const char* name) {
    printf("usage: %s [OPTION]... [FILE]...\n\n", name);
    printf(
        "OPTION:\n"
        "  -h\tprint help information\n"
        "  -V\tprint version\n"
        "  -g\trequired, path to peg grammar file\n"
        "  -e\trequired, entry rule name in peg grammar\n"
        "\n"
        "EXAMPLE:\n\n"
    );
    printf("    $ %s -g /path/to/grammar.peg -e entry /path/to/input.txt\n", name);
}

static char* read_file(FILE* f) {
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fseek (f, 0, SEEK_SET);
    char* buf = P4_MALLOC((length+2) * sizeof(char));
    fread(buf, sizeof(char), length, f);
    buf[length] = '\0';
    return buf;
}

static int print_ast(
        const char* grammar_content,
        const char* input_content,
        const char* entry) {
    int code = 0;
    P4_Grammar* grammar = P4_LoadGrammar((char*)grammar_content);
    P4_Source*  source = P4_CreateSource((char*)input_content, (char*)entry);
    if (source == NULL) {
        code = 1;
        fprintf(stderr, "out of memory");
        goto finalize;
    }
    if (P4_Parse(grammar, source) != P4_Ok) {
        code = 1;
        fprintf(stderr, "%s: %s\n", P4_GetErrorString(P4_GetError(source)), P4_GetErrorMessage(source));
        goto finalize;
    }
    P4_Node*    root = P4_GetSourceAst(source);
    P4_JsonifySourceAst(stdout, root, NULL);
    fprintf(stdout, "\n");
finalize:
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
    return code;
}

int exec_args(p4_args_t* args) {
    return 0;
}

int main(int argc, char* argv[]) {
    int c, err;
    p4_args_t* args = &(p4_args_t){ 0 };

    args->executable = argv[0];

    while (read_arg(argc, argv, "Vhg:e:", c)) {
        switch (c) {
            case 'V':
                args->verbose = true;
                version(basename(args->executable));
                err = 0; goto finalize;
            case 'h':
                args->help = true;
                version(basename(args->executable));
                usage(basename(args->executable));
                err = 0; goto finalize;
            case 'e':
                args->grammar_entry = optarg;
                break;
            case 'g':
                if (!(args->grammar_file = fopen(optarg, "r"))) {
                    perror(optarg);
                    err = 1; goto finalize;
                }
                break;
            default:
                fprintf(stderr, "for usage try: %s -h\n", args->executable);
                err = 1; goto finalize;
        }
    }

    if (optind == 1) {
        usage(basename(args->executable));
        err = 1; goto finalize;
    }

    args->arguments_count = argc - optind;
    args->arguments = argv + optind;

    if (args->arguments_count == 0) {
        fprintf(stderr, "error: argument FILE is required\n");
        usage(basename(args->executable));
        err = 1; goto finalize;
    }

    if (args->grammar_file == NULL) {
        fprintf(stderr, "error: option -g is required\n");
        usage(basename(args->executable));
        err = 1; goto finalize;
    }

    if (args->grammar_entry == NULL) {
        fprintf(stderr, "error: option -e is required\n");
        usage(basename(args->executable));
        err = 1; goto finalize;
    }

    FILE* input_file = NULL;

    if (strcmp(args->arguments[0], "-") == 0) {
        input_file = stdin;
    } else {
        if (!(input_file = fopen(args->arguments[0], "r"))) {
            perror(args->arguments[0]);
            err = 1; goto finalize;
        }
    }

    char* grammar_content = read_file(args->grammar_file);
    char* input_content = read_file(input_file);

    err = print_ast(grammar_content, input_content, args->grammar_entry);

    P4_FREE(input_content);
    P4_FREE(grammar_content);
    fclose(input_file);

finalize:
    fclose(args->grammar_file);
    return err;
}
