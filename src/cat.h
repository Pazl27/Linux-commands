#pragma once
#include <stdbool.h>
#include <stdio.h>

int cat(int argc, char *argv[]);
int doCat(bool lineNum, bool printCtrl, bool showEol, FILE *fpInput);
