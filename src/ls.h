#pragma once
#include <stdbool.h>

int ls(int argc, char *argv[]);
int doLs(bool allFiles, bool longList, bool listDirs, char** entry_list, int size);
int sort_string(const void *str1, const void *str2);
void sort_util(char* line[], int counter);
void print_stats(const char *item);
const char* get_file_name(const char* name);
const char *month_name(int month);
