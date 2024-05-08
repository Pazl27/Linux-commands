#pragma once
#include <stdbool.h>

int sort(int argc, char *argv[]);
int doSort(bool sortReverse, int length, void *data);
int comparator(const void *p1, const void *p2);
