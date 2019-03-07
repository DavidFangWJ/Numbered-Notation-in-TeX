#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "noteList.h"

extern struct NoteNode* notes;

struct NoteNode* interpretAsNumbered(char* token)
{
    static uint8_t category;
    static int8_t pitch;
    static int8_t i;
    if (strlen(token) == 0)
    {
        puts("Illegal token: the token is empty.");
        return NULL;
    }
    if (token[0] == '-') // 增时线
    {
        return newNoteNode(NOTE_CAT_DASH, 0);
    }
    if (token[0] == '0')
    {
        category = NOTE_CAT_QUARTER_REST;
        i = 1;

        if (token[i] == '_') // 减时线
        {
            do
            {
                i++;
                category += 3;
            } while (token[i] == '_');
        }
        if (token[i] == '.') // 附点
        {
            do
            {
                i++;
                category += 1;
            } while (token[i] == '.');
        }
        return newNoteNode(category, 0);
    }
    if (token[0] >= '1' && token[0] <= '7')
    {
        category = NOTE_CAT_QUARTER_NOTE;
        pitch = token[0] - '1';
        i = 1;
        
        if (token[1] == '\'') // 高音点
        {
            do
            {
                i++;
                pitch += 7;
            } while (token[i] == '\'');
        }
        else if (token[1] == ',') // 低音点
        {
            do
            {
                i++;
                pitch -= 7;
            } while (token[i] == ',');
        }
        if (token[i] == '_') // 减时线
        {
            do
            {
                i++;
                category += 3;
            } while (token[i] == '_');
        }
        if (token[i] == '.') // 附点
        {
            do
            {
                i++;
                category += 1;
            } while (token[i] == '.');
        }
        return newNoteNode(category, pitch);
    }
    if (token[0] == '|')
        if (strlen(token) == 1)
            return NULL;
    return NULL;
}
