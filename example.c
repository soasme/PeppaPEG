#include <stdio.h>
#include <peppa.h>

int main(int argc, char* argv[]) {
    P4_Grammar* grammar = P4_LoadGrammar("entry = i\"hello\\nworld\";");
    if (grammar == NULL) {
        printf("Error: CreateGrammar: Error.\n");
        return 1;
    }

    P4_Source*  source = P4_CreateSource("Hello\nWORLD", "entry");
    if (source == NULL) {
        printf("Error: CreateSource: MemoryError.\n");
        return 1;
    }

    if (P4_Parse(grammar, source) != P4_Ok) {
        printf("Error: Parse: ErrCode[%u] Err[%s] Message[%s]\n",
            P4_GetError(source),
            P4_GetErrorString(P4_GetError(source)),
            P4_GetErrorMessage(source)
        );
        return 1;
    }

    P4_Node*    root = P4_GetSourceAst(source);
    char*       text = P4_CopyNodeString(root);

    printf("root span: [%lu %lu]\n", root->slice.start.pos, root->slice.stop.pos);
    printf("root start: line=%lu offset=%lu\n", root->slice.start.lineno, root->slice.start.offset);
    printf("root stop: line=%lu offset=%lu\n", root->slice.stop.lineno, root->slice.stop.offset);
    printf("root next: %p\n", root->next);
    printf("root head: %p\n", root->head);
    printf("root tail: %p\n", root->tail);
    printf("root text: %s\n", text);

    P4_FREE(text);

    P4_JsonifySourceAst(stdout, root, NULL);

    P4_DeleteSource(source);
    P4_DeleteGrammar(grammar);

    return 1;
}
