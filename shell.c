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
#include <getopt.h>
#include "peppa.h"

# define abort(code) do { err = (code); goto finalize; } while (0)

typedef struct p4_args_t p4_args_t;

struct p4_args_t {
    char* executable;
    char* subcommand;
    bool  help;
    bool  version;
    FILE* grammar_file;
    char* grammar_entry;
    size_t arguments_count;
    char** arguments;
};

static int subcommand_version(const char* name) {
    printf("%s version %s\n", name, P4_Version());
    return 0;
}

static int subcommand_usage(const char* name) {
    printf("usage: %s [SUBCOMMAND] [OPTION]...\n\n", name);
    printf(
        "SUBCOMMAND: ast [OPTIONS]... [FILE]...\n\n"
        "  --grammar-file/-g FILE\trequired, path to peg grammar file\n"
        "  --grammar-entry/-e NAME\trequired, entry rule name in peg grammar\n"
        "\n"
        "OPTION:\n\n"
        "  --help/-h\t\t\tprint help information\n"
        "  --version/-V\t\t\tprint version\n"
        "\n"
        "EXAMPLE:\n\n"
    );
    printf("    $ %s --version\n", name);
    printf("    $ %s ast -g json.peg -e entry data.json\n", name);
    return 0;
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

int init_args(p4_args_t* args, int argc, char* argv[]) {
    int c;

    args->executable = argv[0];

    while (1) {
        static struct option long_options[] = {
            {"version", no_argument, 0, 'V'},
            {"help", no_argument, 0, 'h'},
            {"grammar-entry", required_argument, 0, 'e'},
            {"grammar-file", required_argument, 0, 'g'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "Vhe:g:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 0:
                break;
            case 'V':
                args->version = true;
                break;
            case 'h':
                args->help = true;
                break;
            case 'e':
                args->grammar_entry = optarg;
                break;
            case 'g':
                if (!(args->grammar_file = fopen(optarg, "r"))) {
                    perror(optarg);
                    return -1;
                }
                break;
            default:
                fprintf(stderr, "for usage try: %s -h\n", args->executable);
                return -1;
        }
    }

    args->arguments_count = argc - optind;
    args->arguments = argv + optind;

    if (args->arguments_count != 0) {
        args->subcommand = args->arguments[0];
    }

    return 0;
}

int subcommand_ast(p4_args_t* args) {
    int err = 0;
    char *grammar_content = NULL, *input_content = NULL;
    FILE* input_file = NULL;

    if (args->arguments_count < 2) {
        fprintf(stderr, "error: argument FILE is required\n");
        abort(1);
    }

    if (args->grammar_file == NULL) {
        fprintf(stderr, "error: --grammar-file/-g is required\n");
        abort(1);
    }

    if (args->grammar_entry == NULL) {
        fprintf(stderr, "error: --grammar-entry/-e is required\n");
        abort(1);
    }

    if (!(input_file = fopen(args->arguments[1], "r"))) {
        perror(args->arguments[1]);
        abort(1);
    }

    grammar_content = read_file(args->grammar_file);
    input_content = read_file(input_file);
    err = print_ast(grammar_content, input_content, args->grammar_entry);

finalize:

    P4_FREE(input_content);
    P4_FREE(grammar_content);
    fclose(input_file);
    return err;
}

int run_cmd(p4_args_t* args) {
    if (args->help) {
        return subcommand_usage(args->executable);
    }

    if (args->version) {
        return subcommand_version(args->executable);
    }

    if (args->arguments_count == 0) {
        subcommand_usage(args->executable);
        return 1;
    }

    if (strcmp(args->subcommand, "ast") == 0) {
        return subcommand_ast(args);
    }

    fprintf(stderr, "error: invalid command: %s\n", args->subcommand);
    return 1;
}

void close_args(p4_args_t* args) {
    if (args->grammar_file)
        fclose(args->grammar_file);
}

int main(int argc, char* argv[]) {
    int         err = 0;
    p4_args_t*  args = &(p4_args_t){ 0 };

    if (init_args(args, argc, argv) != 0) {
        close_args(args);
        return 1;
    }

    err = run_cmd(args);
    close_args(args);
    return err;
}
