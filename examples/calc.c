#include "calc.h"

int main() {
    P4_Grammar* grammar = P4_CreateCalcGrammar();
    P4_Source*  source  = NULL;
    P4_Error    error   = P4_Ok;
    long        result  = 0;
    char        line[256];

    printf("Type statement to continue. Type ^C to quit.\n");

    while (fgets(line, sizeof(line), stdin)) {
        source = P4_CreateSource(line, "statement");
        if ((error = P4_Parse(grammar, source)) != P4_Ok) {
            printf("error: parse: %d\n", error);
        } else if ((error = P4_CalcEval(P4_GetSourceAst(source), &result)) != P4_Ok){
            printf("error: eval: %d\n", error);
        } else {
            printf("[Out] %ld\n\n", result);
        }
        P4_DeleteSource(source);
    }

    P4_DeleteGrammar(grammar);
}
