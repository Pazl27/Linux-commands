#include "cat.h"
#include <stdio.h>
#include <getopt.h>
#include <ctype.h>

int cat(int argc, char *argv[]) {
    int option = 0;
    bool showEol = false;
    bool lineNum = false;
    bool printCtrl = false;
    while ((option = getopt(argc, argv, "vEn")) != -1) {
        switch (option) {
            case 'n':
                lineNum = true;
                break;
            case 'v':
                printCtrl = true;
                break;
            case 'E':
                showEol = true;
                break;
        }
    }

    if (optind == argc) {
        doCat(lineNum, printCtrl, showEol, stdin);
    } else {
        for (int i = optind; i < argc; ++i) {
            FILE *fpInput = fopen(argv[i], "r");
            if (fpInput == NULL) {
                perror(argv[i]);
                continue;
            }
            doCat(lineNum, printCtrl, showEol, fpInput);
            fclose(fpInput);
        }
    }
    return 0;
}

int doCat(bool lineNum, bool printCtrl, bool showEol, FILE *fpInput) {
    int currentLine = 0;
    int currentChar;
    int lastChar = '\n';
    while ( (currentChar=fgetc(fpInput)) != -1) {
        if (lastChar == '\n') {
            if (lineNum) {
                printf("%5d ", currentLine++);
            }
        }
        if (currentChar == '\n') {
            if (showEol) {
                putc('$', stdout);
            }
            putc(currentChar, stdout);
        }
        else if ((currentChar >= 32 && currentChar <= 127) || isspace(currentChar)) {
            putc(currentChar, stdout);
        }
        else if ( printCtrl ) {
            if ( currentChar < 32 ) {
                putc('^', stdout);
                putc(currentChar+'@', stdout);
            }
            else {
                printf("\\%03o", currentChar);
            }
        }
        else {
            putc(currentChar, stdout);
        }
        lastChar = currentChar;
    }
    return 0;
}
