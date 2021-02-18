/*
 * $ gcc -g benchmark/benchmark_nested_json.c peppapeg.c && valgrind --tool=callgrind ./a.out
 * $ callgrind_annotate callgrind.out.$PID > callgrind.out
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../peppapeg.h"
#include "../examples/json.h"

# define NESTING_DEPTH          1000

int main(int argc, char* argv[]) {
    char* input = malloc(sizeof(char) * (NESTING_DEPTH*2 + 1));
    int i;

    for (i = 0; i < NESTING_DEPTH; i++) {
        input[i] = '[';
        input[NESTING_DEPTH+i] = ']';
    }
    input[NESTING_DEPTH*2] = '\0';

    P4_Grammar* grammar = P4_CreateJSONGrammar();
    assert(grammar != NULL);

    P4_Source* source = P4_CreateSource(input, P4_JSONEntry);
    assert(source != NULL);

    assert(P4_Ok == P4_Parse(grammar, source));

    free(input);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);

    return 0;
}


