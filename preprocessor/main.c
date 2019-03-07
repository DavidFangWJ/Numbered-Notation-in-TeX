#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "globalConsts.h"
#include "fileInput.h"

char inputBuffer[INPUT_BUFFER_LENGTH];

void parseShortOption(char);

int main(int argc, char* argv[])
{
    interpretFile("test");
    return 0;
    if (argc < 2)
    {
        puts("Input file name not found. Please retype it, or type q to quit");
        //fgets(inputBuffer, 50, stdin);
        //if (strcmp(inputBuffer, "Q") == 0 || strcmp(inputBuffer, "q") == 0)
        //    return 0;
    }
    else if (argv[1][0] == '-')
        parseShortOption(argv[1][1]);
    else
    {
        interpretFile(argv[1]);
    }
    return 0;
}

void parseShortOption(char option)
{
    if (option == 'h')
    {
        puts("Preprocessor for Jianpu typesetting  alpha 0.1");
        puts("Usage: jpmx [input filename]");
        puts("-h\tPrint out this help");
    }
    else
        printf("Unrecognized option -%c.\n", option);
}
