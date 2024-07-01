#include "wc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * wc - counts lines, words and characters in a file
 * @param argc - number of arguments
 * @param argv - arguments
 * @return 0 if successful, 1 if failed
 *
 * this function scans the arguments for flags and files
 * sets the flags for lines, words and characters
 * saves all the file inputs into an array
 * calls doCount function to count the lines, words and characters
 */
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

/*
 * this function counts the lines, words and characters in a file
 * @param countLines - flag for counting lines
 * @param countWords - flag for counting words
 * @param countChars - flag for counting characters
 * @param length - number of files
 * @param files - array of file names
 * @return 0 if successful, 1 if failed
 *
 * goes through the array of file names
 * opens the file and reads the lines, words and characters
 * prints the counts for each file
 * prints the total counts if there are more than one file
 */
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
