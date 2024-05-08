#include "grep.h"
#include "regex.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


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
