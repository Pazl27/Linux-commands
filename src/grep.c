#include "grep.h"
#include "regex.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*
 * This function is used to search for a pattern in a file.
 * @param argc: number of arguments
 * @param argv: array of arguments
 * @return: 0 if successful, 1 if failed
 *
 * The function first checks if the number of arguments is less than 3, if so, it prints "Wrong Input" and returns 1.
 * The function then checks for the options in the arguments and sets the case_invert and case_insensitive flags accordingly.
 * The function then sets the pattern and file variables to the last two arguments.
 * The function then calls the doGrep function with the case_invert, case_insensitive, pattern and file variables.
 */
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

/*
 * This function is used to search for a pattern in a file.
 * @param caseInsensitive: flag to indicate if the search should be case-insensitive
 * @param invertMatch: flag to indicate if the search should be inverted
 * @param pattern: the pattern to search for
 * @param file: the file to search in
 * @return: 0 if successful, 1 if failed
 *
 * The function first opens the file and checks if it is NULL, if so, it prints an error message and returns 1.
 * The function then reads the file line by line and checks if the case_invert and case_insensitive flags are set.
 * If the case_invert and case_insensitive flags are set, the function converts the pattern and line to lower case and checks if the pattern is not in the line.
 * If the case_invert flag is set, the function checks if the pattern is not in the line.
 * If the case_insensitive flag is set, the function converts the pattern and line to lower case and checks if the pattern is in the line.
 * If none of the flags are set, the function checks if the pattern is in the line.
 * If the pattern is in the line, the function prints the line.
 * The function then closes the file and returns 0.
 */
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

/*
 * This function is used to convert a string to lower case.
 * @param str: the string to convert to lower case
 *
 * The function iterates through the string and converts each character to lower case.
 */
void str_to_lower(char *str) {
    while (*str) {
        *str = (char) tolower(*str);
        str++;
    }
}
