#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "noteList.h"

#define SIXTEENTH_SPACE 1
#define DOTTED_SIXTEENTH_SPACE 2
#define EIGHTH_SPACE 3
#define DOTTED_EIGHTH_SPACE 4
#define QUARTER_SPACE 5
#define DOTTED_QUARTER_SPACE 6
#define HALF_SPACE 7

extern struct NoteNode* notes;

void outputHeader(FILE* outputFile)
{
    fputs("\\input musixtex\n\\input musixjnp\n\n"
          "\\font\\ssten = shusong\n"
          "\\font\\xbsseventeen = xiaobiaosong scaled \\magstep3\n"
          "\\special{pdf:mapline fzss UniGB-UTF16-H fzssk.ttf}\n"
          "\\special{pdf:mapline fzxbs UniGB-UTF16-H fzxbsk.ttf}\n"
          "\\nostartrule\n"
          "\\setclef18\\setlines10\\setmeter1{{\\inijfrac{4}{4}}}\n"
          "\\smallmusicsize\\geometricskipscale\n"
          "\\ssten\n\n",
          outputFile);
}

inline static void outputTitle(FILE* outputFile, char* title)
{
    fprintf(outputFile, "\\centerline{\\xbsseventeen %s}\\bigskip\n\n", title);
}

uint8_t getSpaceIndex(int8_t catCode)
{
    // 音符
    if (catCode >= NOTE_CAT_NOTE_START && catCode <= NOTE_CAT_NOTE_END)
    {
        uint8_t spaceIndex = QUARTER_SPACE - (catCode - NOTE_CAT_NOTE_START) * 2
                + (catCode - NOTE_CAT_NOTE_START) % NOTE_CAT_DOT_CATEGORY;
        return (spaceIndex < 0) ? 0 : spaceIndex;
    }
    //休止符
    if (catCode >= NOTE_CAT_REST_START && catCode <= NOTE_CAT_REST_END)
    {
        uint8_t spaceIndex = QUARTER_SPACE - (catCode - NOTE_CAT_REST_START) * 2
                + (catCode - NOTE_CAT_REST_START) % NOTE_CAT_DOT_CATEGORY;
        return (spaceIndex < 0) ? 0 : spaceIndex;
    }
    // 其他情况
    switch(catCode)
    {
        case NOTE_CAT_DASH:
            return QUARTER_SPACE;
        default:
            break;
    }
    return -1;
}

bool addTime(uint8_t catCode, uint16_t* time)
{
    const static uint16_t BASIC_TIMES[] = {240, 120, 60, 30, 15};

    // 音符
    if (catCode >= NOTE_CAT_NOTE_START && catCode <= NOTE_CAT_NOTE_END)
    {
        uint16_t timeAdded = BASIC_TIMES[(catCode - NOTE_CAT_NOTE_START)
                                         / NOTE_CAT_DOT_CATEGORY];
        switch ((catCode - NOTE_CAT_NOTE_START) % NOTE_CAT_DOT_CATEGORY)
        {
            case 1:
                timeAdded += timeAdded / 2;
                break;
            case 2:
                timeAdded += timeAdded * 3 / 4;
                break;
        }
        *time += timeAdded;
    }
    //休止符
    else if (catCode >= NOTE_CAT_REST_START && catCode <= NOTE_CAT_REST_END)
    {
        uint16_t timeAdded = BASIC_TIMES[(catCode - NOTE_CAT_REST_START)
                                         / NOTE_CAT_DOT_CATEGORY];
        switch ((catCode - NOTE_CAT_REST_START) % NOTE_CAT_DOT_CATEGORY)
        {
            case 1:
                timeAdded += timeAdded / 2;
                break;
            case 2:
                timeAdded += timeAdded * 3 / 4;
                break;
        }
        *time += timeAdded;
    }
    else switch(catCode)
    {
        case NOTE_CAT_DASH:
            *time += 240;
            break;
        default:
            break;
    }

    if (*time >= 960)
    {
        *time -= 960;
        return true;
    }
    return false;
}

void writeInitialSeq(FILE* outputFile, uint8_t spaceIndex)
{
    switch(spaceIndex)
    {
        case SIXTEENTH_SPACE:
            fputs("\\notes", outputFile);
            break;
        case DOTTED_SIXTEENTH_SPACE:
            fputs("\\notesp", outputFile);
            break;
        case EIGHTH_SPACE:
            fputs("\\Notes", outputFile);
            break;
        case DOTTED_EIGHTH_SPACE:
            fputs("\\Notesp", outputFile);
            break;
        case QUARTER_SPACE:
            fputs("\\NOtes", outputFile);
            break;
        case DOTTED_QUARTER_SPACE:
            fputs("\\NOtesp", outputFile);
            break;
        case HALF_SPACE:
            fputs("\\NOTes", outputFile);
            break;
    }
}

char getPitchChar(int8_t pitch)
{
    if (pitch >= -2 && pitch < 24) // 直接用小写字母
        return pitch + 'c';
    if (pitch >= -16 && pitch < -2)
        return pitch + 14 + 'C';
    return 0;
}

void writeNumberedNote(FILE* outputFile, uint8_t catCode, int8_t pitch)
{

    switch(catCode)
    {
        case NOTE_CAT_QUARTER_NOTE:
            fputs("\\jq ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_QUARTER_REST:
            fputs("\\jq R", outputFile);
            break;
        case NOTE_CAT_DASH:
            fputs("\\jq Z", outputFile);
            break;

        case NOTE_CAT_DOTTED_QUARTER_NOTE:
            fputs("\\jqp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_DOTTED_QUARTER_REST:
            fputs("\\jqp R", outputFile);
            break;

        case NOTE_CAT_DOUBLE_DOTTED_QUARTER_NOTE:
            fputs("\\jqpp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_DOUBLE_DOTTED_QUARTER_REST:
            fputs("\\jqpp R", outputFile);
            break;

        case NOTE_CAT_EIGHTH_NOTE:
            fputs("\\jc ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_EIGHTH_REST:
            fputs("\\jc R", outputFile);
            break;

        case NOTE_CAT_DOTTED_EIGHTH_NOTE:
            fputs("\\jcp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_DOTTED_EIGHTH_REST:
            fputs("\\jcp R", outputFile);
            break;

        case NOTE_CAT_DOUBLE_DOTTED_EIGHTH_NOTE:
            fputs("\\jcpp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_DOUBLE_DOTTED_EIGHTH_REST:
            fputs("\\jcpp R", outputFile);
            break;

        case NOTE_CAT_SIXTEENTH_NOTE:
            fputs("\\jcc ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_SIXTEENTH_REST:
            fputs("\\jcc R", outputFile);
            break;

        case NOTE_CAT_DOTTED_SIXTEENTH_NOTE:
            fputs("\\jccp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
            break;
        case NOTE_CAT_DOTTED_SIXTEENTH_REST:
            fputs("\\jccp R", outputFile);
            break;

        case NOTE_CAT_DOUBLE_DOTTED_SIXTEENTH_NOTE:
            fputs("\\jccpp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
        case NOTE_CAT_DOUBLE_DOTTED_SIXTEENTH_REST:
            fputs("\\jccpp R", outputFile);
            break;

        case NOTE_CAT_32ND_NOTE:
            fputs("\\jccc ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
        case NOTE_CAT_32ND_REST:
            fputs("\\jccc R", outputFile);
            break;

        case NOTE_CAT_DOTTED_32ND_NOTE:
            fputs("\\jcccp ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
        case NOTE_CAT_DOTTED_32ND_REST:
            fputs("\\jcccp R", outputFile);
            break;

        case NOTE_CAT_64TH_NOTE:
            fputs("\\jcccc ", outputFile);
            fputc(getPitchChar(pitch), outputFile);
        case NOTE_CAT_64TH_REST:
            fputs("\\jcccc R", outputFile);
            break;

        default:
            break;
    }
}

void outputNotes(FILE* outputFile, struct NoteNode* notes)
{
    uint8_t currentCat, nextCat, currentSpace, nextSpace;
    uint16_t offsetInMeasure = 0;

    fputs("\\startpiece\n", outputFile);
    writeInitialSeq(outputFile, getSpaceIndex(notes->category));
    while (notes->next != NULL)
    {
        currentCat = notes->category;
        nextCat = notes->next->category;

        writeNumberedNote(outputFile, currentCat, notes->pitch);

        // 处理宽度不一致的问题
        currentSpace = getSpaceIndex(currentCat);
        nextSpace = getSpaceIndex(nextCat);
        if (currentSpace != nextSpace) // 在宽度不一的时候重写
        {
            fputs("\\en\n", outputFile);
            writeInitialSeq(outputFile, nextSpace);
        }

        // 处理跨小节问题
        if (addTime(currentCat, &offsetInMeasure))
        {
            fputs("\\en\\bar\n", outputFile);
            writeInitialSeq(outputFile, nextSpace);
        }
        notes = notes->next;
    }

    // 处理最后一个音符
    writeNumberedNote(outputFile, nextCat, notes->pitch);
    fputs("\\en\\Endpiece\\bye", outputFile);
}

void outputToFile(char* outputFileName, char* title)
{
    FILE* outputFile = fopen(outputFileName, "w");
    outputHeader(outputFile);
    outputTitle(outputFile, title);
    outputNotes(outputFile, notes);
    fclose(outputFile);
}
