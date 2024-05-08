#pragma once
#include <stdbool.h>

void str_to_lower(char *str);
int grep(int argc, char *argv[]);
int doGrep(bool caseInsensitive, bool invertMatch, char *pattern, const char *file);
