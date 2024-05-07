#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdbool.h>

#include "ls.h"
#include "cat.h"

int grep(int argc, char *argv[]);
int doGrep(bool caseInsensitive, bool invertMatch, char *pattern, const char *file);
int wc(int argc, char *argv[]);
int doCount(bool countLines, bool countWords, bool countChars, int length, char **files);
int sort(int argc, char *argv[]);
int doSort(bool sortReverse, int length, void *data);

bool endsWith(const char *arg, const char *command);

void str_to_lower(char *str);

int match(char *regexp, char *text);
int match_here(char *regexp, char *text);
int match_star(int c, char *regexp, char *text);
int match_question(char c, char *regexp, char *text); // New function

int main(int argc, char *argv[]) {
    if ( argc==0 ) {
        fprintf(stderr, "Syntax: BusyBox <args>\n");
        return 1;
    }
    if ( endsWith(argv[0], "cat") ) {
        return cat(argc, argv);
    }
    if ( endsWith(argv[0], "ls") ) {
        return ls(argc, argv);
    }
    if ( endsWith(argv[0], "grep") ) {
        return grep(argc, argv);
    }
    if ( endsWith(argv[0], "wc") ) {
        return wc(argc, argv);
    }
    if ( endsWith(argv[0], "sort") ) {
        return sort(argc, argv);
    }
    fprintf(stderr, "Unsupported command: %s\n", argv[0]);

    return 0;
}

bool endsWith(const char *arg, const char *command) {
    char *lastSlash = strrchr(arg, '/');
    return lastSlash == NULL && strcmp(arg, command) == 0 ||
           lastSlash != NULL && strcmp(lastSlash + 1, command) == 0;
}


int grep(int argc, char *argv[]) {
    if ( argc < 3) {
        printf("Wrong Input");
        return 1;
    }

    bool case_invert = false;
    bool case_insensitive = false;

    for (int i = 1; i < argc - 2; i++) {

        if (argv[i][0] == '-') {

            const char *option = argv[i];
            if( strchr(option, 'v') != NULL ) {
               case_invert = true;
            }
            if( strchr(option, 'i') != NULL ) {
               case_insensitive = true;
            }

            for (int j = 1; option[j] != '\0'; j++ ) {
                if ( option[j] != 'v' && option[j] != 'i' ) {
                    printf("Wrong input: %s\n", option);
                    return 1;
                }
            }
        }
    }

    char *pattern = argv[argc - 2];
    const char *file = argv[argc -1];
    doGrep(case_insensitive, case_invert, pattern, file);
    return 0;
}

int doGrep(bool caseInsensitive, bool invertMatch, char *pattern, const char *file) {
    FILE *f;
    char line[1024];

    // Open the file
    f = fopen(file, "r");
    if (f == NULL) {
        perror("fopen");
        return 1;
    }

    while (fgets(line, sizeof(line), f) != NULL) {
        if (invertMatch && caseInsensitive) {
            str_to_lower(pattern);
            char temp[1024];
            strcpy(temp, line);
            str_to_lower(temp);
            if (!match(pattern, line)) {
                printf("%s", line);
            }
        }
        else if (caseInsensitive) {
            str_to_lower(pattern);
            char temp[1024];
            strcpy(temp, line);
            str_to_lower(temp);
            if (match(pattern, line)) {
                printf("%s", line);
            }
        }
        else if (invertMatch) {
            if (!match(pattern, line)) {
                printf("%s", line);
            }
        }
        else {
            // Standard case-sensitive search
            if (match(pattern, line)) {
                printf("%s", line);
            }
        }
    }
    fclose(f);
    return 0;
}

void str_to_lower(char *str) {
    while (*str) {
        *str = (char) tolower(*str);
        str++;
    }
}

int wc(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Wrong Input.");
        return 1;
    }

    bool count_lines = false;
    bool count_words = false;
    bool count_chars = false;
    int count_options = 0;

    for (int i = 1; i < argc; i++) {

        if (argv[i][0] == '-') {
            count_options++;
            const char *option = argv[i];

            if (strchr(option, 'l') != NULL) {
                count_lines = true;
            }
            if (strchr(option, 'w') != NULL) {
                count_words = true;
            }
            if (strchr(option, 'c') != NULL) {
                count_chars = true;
            }

            // checking all chars if they are l, w, c
            for (int j = 1; option[j] != '\0'; j++ ) {
                if (option[j] != 'l' && option[j] != 'w' && option[j] != 'c') {
                    printf("Wrong input: %s\n", option);
                    return 1;
                }
            }
        }
    }

    int numFiles = argc - 1 - count_options;
    char **fileNames = (char **)malloc((numFiles) * sizeof(char *));

    if (fileNames == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    int temp = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            fileNames[temp] = argv[i];
            temp++;
        }
    }

    if(!count_lines && !count_words && !count_chars) {
        count_lines = true;
        count_words = true;
        count_chars = true;
    }

    doCount(count_lines, count_words, count_chars, numFiles, fileNames);
    free(fileNames);
    return 0;
}

int doCount(bool countLines, bool countWords, bool countChars, int length, char **files)
{
    int ins_line = 0;
    int ins_word = 0;
    int ins_char = 0;

    for(int j = 0; j < length; j++) {
        FILE *f;
        char line[1024];
        const char  *file_path = files[j];

        int line_counter = 0;
        int word_counter = 0;
        int char_counter = 0;

        f = fopen(file_path, "r");
        if (f == NULL) {
            perror("File not found");
            return 1;
        }

        while (fgets(line, sizeof(line), f) != NULL) {
            line_counter++;
            char_counter += (int) strlen(line);
            char *token = strtok(line, " \t\n");
            while (token != NULL) {
                word_counter++;
                token = strtok(NULL, " \t\n");
            }
        }

        ins_line += line_counter;
        ins_word += word_counter;
        ins_char += char_counter;

        int counts[3] = {line_counter, word_counter, char_counter};
        int flags[3] = {countLines, countWords, countChars};

        for (int k = 0; k < 3; k++) {
            if (flags[k]) {
                printf("%d\t", counts[k]);
            } else {
                printf("\t");
            }
        }
        printf("%s\n", file_path);
        fclose(f);
    }

    int counts[3] = {ins_line, ins_word, ins_char};
    int flags[3] = {countLines, countWords, countChars};

    if (length > 1) {
        for (int i = 0; i < 3; i++) {
            if (flags[i]) {
                printf("%d\t", counts[i]);
            }
        }
        printf("%s\n", "insgesamt");
    }
    return 0;
}

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

int match(char *regexp, char *text) {
    if (regexp[0] == '^')
        return match_here(regexp + 1, text);
    do {
        if (match_here(regexp, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}

int match_here(char *regexp, char *text) {
    if (regexp[0] == '\0')
        return 1;
    if (regexp[1] == '*')
        return match_star(regexp[0], regexp + 2, text);
    if (regexp[1] == '?') // Check if the next character is '?'
        return match_question(regexp[0], regexp + 2, text); // Handle '?' operator
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return match_here(regexp + 1, text + 1);
    return 0;
}

int match_star(int c, char *regexp, char *text) {
    do {
        if (match_here(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

int match_question(char c, char *regexp, char *text) {
    if (match_here(regexp, text))
        return 1;
    if (*text != '\0' && (*text++ == c || c == '.'))
        return match_here(regexp, text);
    return 0;
}
