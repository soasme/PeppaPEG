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
#include <glob.h>
#include "peppa.h"

# define abort(code) do { err = (code); goto finalize; } while (0)

typedef struct p4_args_t p4_args_t;

struct p4_args_t {
    char* executable;
    char* subcommand;
    bool  help;
    bool  version;
    char* grammar_content;
    FILE* grammar_file;
    char* grammar_entry;
    size_t arguments_count;
    char** arguments;
    bool quiet;
    bool json;
    bool json2;
    bool text;
    bool debug;
};

static int subcommand_version(const char* name) {
    printf("%s version %s\n", name, P4_Version());
    return 0;
}

static int subcommand_usage(const char* name) {
    printf("usage: %s [SUBCOMMAND] [OPTION]...\n\n", name);
    printf(
        "SUBCOMMAND: parse [OPTIONS]... [FILE]...\n\n"
        "  --grammar-str/-g FILE\tpeg grammar string\n"
        "  --grammar-file/-G FILE\tpath to peg grammar file\n"
        "  --grammar-entry/-e NAME\tentry rule name in peg grammar\n"
        "  --json/-j\t\tjson ast output\n"
        "  --json2/-J\t\tjson ast output with only arrays\n"
        "  --text/-t\t\ttext ast output [default]\n"
        "  --quiet/-q\t\tno ast output\n"
        "\n"
        "OPTION:\n\n"
        "  --help/-h\t\t\tprint help information\n"
        "  --version/-V\t\t\tprint version\n"
        "\n"
        "EXAMPLE:\n\n"
    );
    printf("    $ %s --version\n", name);
    printf("    $ %s parse -G json.peg -e entry data.json\n", name);
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
        const char* file_path,
        const char* grammar_content,
        const char* input_content,
        p4_args_t *args) {
    int code = 0;
    P4_Grammar* grammar = P4_LoadGrammar(grammar_content);
    P4_Source*  source = P4_CreateSource(input_content, args->grammar_entry);
    if (source == NULL) {
        code = 1;
        fprintf(stderr, "out of memory");
        goto finalize;
    }
    if (P4_Parse(grammar, source) != P4_Ok) {
        code = 1;
        if (strcmp(file_path, "") != 0)
            fprintf(stderr, "%s:\n", file_path);
        fprintf(stderr, "%s: %s\n", P4_GetErrorString(P4_GetError(source)), P4_GetErrorMessage(source));
        goto finalize;
    }
    if (!args->quiet) {
        P4_Node*    root = P4_GetSourceAst(source);
        if(args->json2)
            P4_Jsonify2SourceAst(stdout, root, 0);
        else if(args->json)
            P4_JsonifySourceAst(stdout, root, 0);
        else
            P4_TxtSourceAst(stdout, root, 0);
        fprintf(stdout, "\n");
    }
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
            {"quiet", no_argument, 0, 'q'},
            {"json", no_argument, 0, 'j'},
            {"json2", no_argument, 0, 'J'},
            {"text", no_argument, 0, 't'},
            {"debug", no_argument, 0, 'd'},
            {"grammar-entry", required_argument, 0, 'e'},
            {"grammar-file", required_argument, 0, 'G'},
            {"grammar-str", required_argument, 0, 'g'},
            {0, 0, 0, 0}
        };
        int option_index = 0;
        c = getopt_long (argc, argv, "VhqjJtde:g:G:", long_options, &option_index);
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
            case 'q':
                args->quiet = true;
                break;
            case 'd':
                args->debug = true;
                break;
            case 'j':
                args->json = true;
                break;
            case 'J':
                args->json2 = true;
                break;
            case 't':
                args->text = true;
                break;
            case 'e':
                args->grammar_entry = optarg;
                break;
            case 'g':
                args->grammar_content = optarg;
                break;
            case 'G':
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

int subcommand_parse(p4_args_t* args) {
    int err = 0;
    char *grammar_content = NULL, *input_content = NULL;
    FILE* input_file = NULL;

    if (args->grammar_file == NULL && args->grammar_content == NULL) {
        fprintf(stderr, "error: --grammar-file/-G or --grammar-str/-g is required\n");
        abort(1);
    }

    if (args->grammar_entry == NULL) {
        fprintf(stderr, "error: --grammar-entry/-e is required\n");
        abort(1);
    }

    if (args->grammar_content) {
        grammar_content = strdup(args->grammar_content);
    } else {
        grammar_content = read_file(args->grammar_file);
    }

    if (args->arguments_count == 1) {
        input_content = P4_MALLOC(sizeof(char) * 1024 * 1024);
        char* s = input_content;
        size_t size = 0;
        while ((*(s++) = getchar()) != EOF && (++size) < 1024 * 1024 - 1) {
        }
        *s = '\0';

        err = print_ast("", grammar_content, input_content, args);
    } else if (strstr(args->arguments[1], "*") == NULL){
        if (!(input_file = fopen(args->arguments[1], "r"))) {
            perror(args->arguments[1]);
            abort(1);
        }
        input_content = read_file(input_file);
        err = print_ast(args->arguments[1], grammar_content, input_content, args);
    } else {
        glob_t globbuf = {0};
        glob(args->arguments[1], 0, 0, &globbuf);
        size_t i = 0;
        for (i = 0; i < globbuf.gl_pathc; i++) {
            char* file_path = globbuf.gl_pathv[i];
            if (!args->quiet) fprintf(stdout, "%s:\n", file_path);
            if (!(input_file = fopen(file_path, "r"))) {
                perror(args->arguments[1]);
                abort(1);
            }
            input_content = read_file(input_file);

            int ferr = print_ast(file_path, grammar_content, input_content, args);
            if (err == 0 && ferr != 0)
                err = ferr;

            P4_FREE(input_content);
            fclose(input_file);
            input_content = NULL;
            input_file = NULL;

            if (!args->quiet) fprintf(stdout, "\n");
        }
        globfree(&globbuf);
    }

finalize:

    if (input_content) P4_FREE(input_content);
    if (grammar_content) P4_FREE(grammar_content);
    if (input_file) fclose(input_file);
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

    if (strcmp(args->subcommand, "parse") == 0) {
        return subcommand_parse(args);
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
