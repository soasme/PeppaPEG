/*
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
 * Example: Write an INI Parser using Peppa PEG.
 *
 *      $ gcc peppapeg.c examples/ini.c && ./a.out
 *      Section=user
 *      Value name=Ju Lin
 *      Section=project
 *      Value name=Peppa PEG
 *      Value url=https://github.com/soasme/PeppaPEG
 *      Value license=MIT
 *      Value website=
 *      Success!
 */

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
url=https://github.com/soasme/PeppaPEG\n\
license=MIT\n\
website=\n";

    P4_Grammar* ini = P4_CreateINIGrammar();
    if (ini == NULL) {
        printf("Error: CreateINIGrammar: MemoryError\n");
        return 1;
    }

    P4_Source*  src = P4_CreateSource(input, P4_INI_Entry);

    if (P4_Ok != P4_Parse(ini, src)) {
        printf("Error: Parse: (%u) %s\n", P4_GetError(src), P4_GetErrorMessage(src));
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
                if (node->tail == node->head)
                    valuestr = strdup("");
                else
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
