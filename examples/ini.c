#include <stdio.h>
#include "../peppapeg.h"
#include "ini.h"

int
main(int argc, char* argv[]) {
    char* input = "[user]\n\
name=Ju Lin\n\
\n\
[project]\n\
name=Peppa PEG\n\
url=https://github.com/soasme/PeppaPEG\n";

    P4_Grammar* ini = P4_CreateINIGrammar();
    if (ini == NULL) {
        printf("Error: CreateINIGrammar: MemoryError\n");
        return 1;
    }

    P4_Source*  src = P4_CreateSource(input, P4_INI_Entry);

    if (P4_Ok != P4_Parse(ini, src)) {
        printf("Error: Parse: (%lu) %s\n", P4_GetError(src), P4_GetErrorMessage(src));
        return 1;
    }

    P4_Token* root = P4_GetSourceAst(src);
    if (root == NULL) {
        printf("Error: root is NULL\n");
        return 1;
    }

    P4_Token* node = root->head;
    P4_String namestr = NULL,
              valuestr = NULL;

    while (node != NULL) {
        switch(node->expr->id) {
            case P4_INI_Section:
                namestr = P4_CopyTokenString(node->head);
                printf("Section=%s\n", namestr);
                free(namestr);
                break;
            case P4_INI_Property:
                namestr = P4_CopyTokenString(node->head);
                valuestr = P4_CopyTokenString(node->tail);
                printf("Value %s=%s\n", namestr, valuestr);
                free(namestr);
                free(valuestr);
                break;
        }
        node = node->next;
    }

    P4_DeleteSource(src);
    P4_DeleteGrammar(ini);

    printf("Success!\n");

    return 0;
}
