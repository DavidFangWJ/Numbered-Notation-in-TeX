#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "noteList.h"
#include "globalConsts.h"
#include "fileOutput.h"
#include "tokenInterpretation.h"

#define TOKEN_BUFFER_SIZE 16

extern char inputBuffer[INPUT_BUFFER_LENGTH];

struct {
    char title[TITLE_MAX_LENGTH];
} globalVariables;

static FILE* inputFile;
static char tokenBuffer[TOKEN_BUFFER_SIZE];

struct NoteNode* notes;
struct NoteNode* lastPos;

int8_t readLine(void)
{
    static char currentChar;

    for (int i = 0; i < INPUT_BUFFER_LENGTH; i++)
        inputBuffer[i] = 0;

    // 读取直到第一个非空字符
    while (1)
    {
        currentChar = fgetc(inputFile);
        if (currentChar == EOF) // 文档已经结束
            return -1;
        if (currentChar == '\n') // 行已经结束
            return 0;
        if (currentChar != ' ')
        {
            inputBuffer[0] = currentChar;
            break;
        }
    }

    for (int i = 1; i < INPUT_BUFFER_LENGTH; i++)
    {
        currentChar = fgetc(inputFile);
        if (currentChar == EOF) // 文档已经结束
            return -1;
        if (currentChar == '\n') // 一行结束了
            return 0; // 行末
        inputBuffer[i] = currentChar;
    }
    // 检测是否在词中截断
    for (int i = INPUT_BUFFER_LENGTH - 1; i >= 0; i--)
    {
        if (inputBuffer[i] == ' ')
        {
            fseek(inputFile, -INPUT_BUFFER_LENGTH + 1 + i, SEEK_CUR);
            inputBuffer[i] = '\0';
            return 1; // 行中
        }
    }
    // 词的长度超过缓冲区——出错了
    printf("Error: Extra long token (longer than %d bytes)",
           INPUT_BUFFER_LENGTH);
    exit(1);
}

// 解析分数
int8_t parseFraction(const char* value)
{
}

// 解析键-值对
void parseKeyValuePair(const char* line)
{
    static char* key;
    static char* value;
    static int8_t offset1 = 0, offset2 = 0;

    for (; offset1 < strlen(line); offset1++)
        if (line[offset1] == ':')
            break;
    offset1--;
    offset2 = offset1 + 2; // 现在都是第一个冒号的位置
    // 找到冒号前、后各自最近的非空格字符
    for (; offset1 >= 0; offset1--)
        if (line[offset1] != ' ')
            break;
    for (; offset2 < strlen(line); offset2++)
        if (line[offset2] != ' ')
            break;

    // 构造字符串
    offset1++;
    key = malloc(offset1 + 1);
    strncpy(key, line, offset1);
    key[offset1] = '\0';
    value = malloc(strlen(line + offset2));
    strcpy(value, line + offset2);

    // 赋值
    if (strcmp(key, "Title") == 0) // 标题
        strncpy(globalVariables.title, value, TITLE_MAX_LENGTH);
    else if (strcmp(key, "Meter") == 0) // 设置拍号
    {/* TO BE IMPLEMENTED */}

    //析构
    free(key);
    key = NULL;
    free(value);
    value = NULL;
}

// 从已知指针中读取下一个词语，source指针会更新；返回需要偏移的字节数
int8_t getNextToken(char* dest, char* source)
{
    static uint8_t offset;

    // 清除目标指针的内容
    memset(dest, 0, TOKEN_BUFFER_SIZE);
    offset = 0;

    // 内容读取完毕，不存在Token
    if (*source == '\0') return 0;

    // 特殊字符（括号）等直接处理
    if (*source == '(' || *source == ')'
            || *source == '[' || *source == ']')
    {
        dest[0] = *source;
        dest[1] = '\0';

        offset = 1;
        while (source[offset] == ' ') // 跳过空格部分
            offset++;
        return offset;
    }

    // 读取到下一个空格或字符串结尾
    while (source[offset] != ' ' && source[offset] != '\0'
            && source[offset] != '(' && source[offset] != ')'
            && source[offset] != '[' && source[offset] != ']')
        offset++;
    strncpy(dest, source, offset);

    // 跳过空格部分——这保证了上一条可以直接以token开始
    while (source[offset] == ' ')
        offset++;
    return offset;
}

void interpretFile(const char* fileName)
{
    char* outputFileName;
    char* tmp;

    notes = NULL;

    inputFile = fopen(fileName, "r");
    if (inputFile == NULL)
    {
        puts("Error: file not found.");
        exit(1);
    }

    tmp = strrchr(fileName, '.');
    if (tmp != NULL && tmp > fileName)
    {
        outputFileName = malloc((tmp - fileName + 5) * sizeof(char));
        strncpy(outputFileName, fileName, tmp - fileName);
        strcpy(outputFileName + (tmp - fileName), ".tex");
    }
    else
    {
        outputFileName = malloc((strlen(fileName) + 5) * sizeof(char));
        strcpy(outputFileName, fileName);
        strcpy(outputFileName + strlen(outputFileName), ".tex");
    }
    int i = 1;
    while (true)
    {
        int8_t state = readLine();
        if (state == -1 && strlen(inputBuffer) == 0)
        {
            fclose(inputFile);
            outputToFile(outputFileName, globalVariables.title);
            deleteNoteNode(notes);
            notes = NULL;
            free(outputFileName);
            return;
        }
        // 跳过空行
        if (strlen(inputBuffer) == 0) continue;
        if (inputBuffer[0] >= 'A' && inputBuffer[0] <= 'Z')
            parseKeyValuePair(inputBuffer);
        else
        {
            int8_t shiftValue = 1;
            char* currentStream = inputBuffer;
            while (shiftValue)
            {
                shiftValue = getNextToken(tokenBuffer, currentStream);
                if (shiftValue == 0) break; // 读取到行末了
                currentStream += shiftValue;
                if (notes == NULL) // 链表为空
                {
                    notes = interpretAsNumbered(tokenBuffer);
                    lastPos = notes;
                }
                else
                {
                    lastPos->next = interpretAsNumbered(tokenBuffer);
                    if (lastPos->next)
                        lastPos = lastPos->next;
                }
            }
        }
        i++;
    }
}
