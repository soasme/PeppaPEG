#include <stdio.h>
#include <stdlib.h>
#include "../peppapeg.h"
#include "json.h"

int main(int argc, char* argv[]) {
    P4_Grammar* grammar = P4_CreateJSONGrammar();
    const P4_String input = "[1,2.0,3e1,true,false,null,\"xyz\",{},[]]";
    P4_Source* source = P4_CreateSource(input, P4_JSONEntry);
    P4_Error err = P4_Parse(grammar, source);
    P4_Token* root = P4_GetSourceAst(source);
    P4_JsonifySourceAst(stdout, root);
    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);
    return 0;
}
