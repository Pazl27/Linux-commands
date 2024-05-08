#include "sort.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

int sort(int argc, char *argv[]) {
    bool reverse = false;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            const char *option = argv[i];

            if (strchr(option, 'r') != NULL) {
                reverse = true;
            }

            // checking all chars if they are l, w, c
            for (int j = 1; option[j] != '\0'; j++ ) {
                if (option[j] != 'r' ) {
                    printf("Wrong input: %s\n", option);
                    return 1;
                }
            }
        }
    }
    int fd=open(argv[argc - 1], O_RDONLY);
    if ( fd == -1 ) {
        perror(argv[1]);
        return 1;
    }
    struct stat st;
    stat(argv[argc - 1 ], &st);
    void *data = mmap(NULL, st.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if ( data == NULL ) {
        perror(argv[1]);
        return 1;
    }
    doSort(reverse, (int) st.st_size, data);
    munmap(data, st.st_size);
    close(fd);
    return 0;
}

int comparator(const void *p1, const void *p2) {
//    const char **c1=p1;
//    const char **c2=p2;
    char *const *c1 = p1;
    char *const *c2 = p2;
    // reason for error first test
//    return strcmp(*c1, *c2);
    return strcasecmp(*c1, *c2);
}

int doSort(bool sortReverse, int length, void *data) {
    int lines = 0;
    char *cData = data;
    char *nData;
    while ( (nData = strchr(cData, '\n')) != NULL ){
        lines++;
        cData = nData+1;
    }
    const char **aData = alloca(lines*sizeof(const char *));
    int line = 0;
    cData = data;
    while ( (nData = strchr(cData, '\n')) != NULL ){
        aData[line++]=cData;
        *nData = '\0';
        cData = nData+1;
    }
    qsort(aData, lines, sizeof(const char *), comparator);

    if (sortReverse) {
        for (int i = lines - 1; i >= 0; i--) {
            printf( "%s\n", aData[i]);
        }
    }
    else {
        for (int i = 0; i < lines; ++i) {
            printf( "%s\n", aData[i]);
        }
    }
    return 0;
}
