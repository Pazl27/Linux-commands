#pragma once
#include <stdbool.h>

int wc(int argc, char *argv[]);
int doCount(bool countLines, bool countWords, bool countChars, int length, char **files);
