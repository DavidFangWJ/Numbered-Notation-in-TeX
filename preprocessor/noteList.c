#include <stdlib.h>
#include <stdint.h>
#include "noteList.h"

struct NoteNode* newNoteNode(uint8_t c, int8_t p)
{
    struct NoteNode* tmp = malloc(sizeof(struct NoteNode));
    tmp->category = c;
    tmp->pitch = p;
    tmp->next = NULL;
    return tmp;
}

void deleteNoteNode(struct NoteNode* node)
{
    if (node->next != NULL)
        deleteNoteNode(node->next);
    free(node);
    node = NULL;
}
