#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <ctype.h>
#include <grp.h>
#include <time.h>

/*
 * function to process the ls command
 * @param argc: number of arguments
 * @param argv: list of arguments
 * @return: 0 if successful, 1 if failed
 *
 * goes through the arguments and sets the flags for the ls command
 * creates an array of strings for the entries (files or directories)
 * if no entries are given, it creates an array with the current directory
 * calls doLs to process the ls command
 */
int ls(int argc, char *argv[]) {
    bool list_all = false;
    bool list_dir = false;
    bool list_long = false;
    int option_counter = 0;

    if (argc == 1) ;

    else {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                const char *option = argv[i];
                option_counter ++;

                if( strchr(option, 'a') != NULL ) {
                    list_all = true;
                }
                if( strchr(option, 'l') != NULL ) {
                    list_long = true;
                }
                if( strchr(option, 'd') != NULL ) {
                    list_dir = true;
                }

                for (int j = 1; option[j] != '\0'; j++ ) {
                    if (option[j] != 'l' && option[j] != 'a' && option[j] != 'd') {
                        printf("Wrong input: %s\n", option);
                        return 1;
                    }
                }
            }
        }
    }
    int number_of_items = argc - 1 - option_counter;
    char **entry_list;
    if (number_of_items == 0) {
        number_of_items = 1;
        entry_list = (char**) malloc(sizeof(char*) * number_of_items);
        if (entry_list == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        entry_list[0] = ".";
    }
    else {
        entry_list = (char**) malloc(sizeof(char*) * number_of_items);
        if (entry_list == NULL) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }
        for (int i = 0; i < number_of_items; i++) {
            entry_list[i] = argv[i + 1 + option_counter];
        }
    }

    doLs(list_all, list_long, list_dir, entry_list, number_of_items);
    free(entry_list);
    return 0;
}

/*
 * function to process the ls command
 * @param allFiles: flag to list all files
 * @param longList: flag to list files in long format
 * @param listDirs: flag to list directories
 * @param entry_list: list of entries (files or directories)
 * @param size: number of entries
 * @return: 0 if successful, 1 if failed
 *
 * goes through the list of entries and processes them
 * it checks if the entry is a file or a directory file=1, file=0
 *
 * if it is a file (true) it prints the file name or the file stats if longList is true
 * calls print_stats to print the file stats
 *
 * if it is a directory (false) it creates an array of strings with the names of the files in the directory
 * it sorts the array of strings with the sort_util function
 *
 * if listDirs is true, it prints the directory name or the directory stats if longList is true
 *
 * if longList is true it creates a stat struct to get the file stats
 * it iterates through the array of files/dirs in the directory
 * creates a string full_path with the path of the file/dir
 * gets the file stats checks if it is a dir or a regular file
 * and calls the print_stats function to print the stats
 *
 * if allFiles is true, it prints all the files in the directory
 *
 * if allFiles is false, it prints all the files in the directory that do not start with '.'
 */
int doLs(bool allFiles, bool longList, bool listDirs, char** entry_list, int size) {
    for(int j = 0; j < size; j++) {
        char *name = entry_list[j];
        bool file = false;
        struct stat path_stat;

        if (stat(name, &path_stat) != 0) {
            perror("Error in stat");
            return 1;
        }

        // Check if it's a regular file
        if (S_ISREG(path_stat.st_mode)) {
            file = true;
        }
        // Check if it's a directory
        else if (S_ISDIR(path_stat.st_mode)) {
            file = false;
        }
        // Otherwise, it's some other type of file (e.g., symlink, socket, pipe)
        else {
            exit(1);
        }

        if (file) {
            if (longList) {
                print_stats(name);
            }
            else {
                printf("%s\n", get_file_name(name));
            }
        }

        else {
            struct dirent **dir_list;
            int n;

            n = scandir(name, &dir_list, 0, alphasort);
            if(n < 0) {
                perror("scandir");
                return 1;
            }
            char **dir_names = (char **)malloc(n * sizeof(char *));
            if (dir_names == NULL) {
                perror("Memory allocation failed");
                return 1;
            }

            for (int i = 0; i < n; i++) {
                dir_names[i] = NULL;
            }

            int count = 0;
            while (n--) {
                dir_names[count] = strdup(dir_list[n]->d_name);
                if (dir_names[count] == NULL) {
                    perror("Memory allocation failed");
                    return 1;
                }
                count++;
                free(dir_list[n]);
            }
            free(dir_list);

            sort_util(dir_names, count);

            if (listDirs) {
                if (longList) {
                    print_stats(name);
                }
                else {
                    printf("%s", name);
                }
            }
            else if (longList) {
                struct stat file_stat;
                for (int i = 0; i < count; i++) {
                    char *full_path = malloc(strlen(name) + strlen(dir_names[i]) + 2);
                    if (full_path == NULL) {
                        perror("Memory allocation failed");
                        return 1;
                    }
                    snprintf(full_path, strlen(name) + strlen(dir_names[i]) + 2, "%s/%s", name, dir_names[i]);

                    if (stat(full_path, &file_stat) < 0) {
                        perror("Error getting file status");
                        free(full_path);
                        return 1;
                    }

                    if (S_ISREG(file_stat.st_mode) || S_ISDIR(file_stat.st_mode)) {
                        if (allFiles || (dir_names[i][0] != '.')) {
                            print_stats(full_path);
                        }
                    }

                    free(full_path);
                }
            }
            else if (allFiles) {
                for (int i = 0; i < count; i++) {
                    printf("%s\n", dir_names[i]);
                }
            }
            else {
                for (int i = 0; i < count; i++) {
                    if(dir_names[i][0] != '.')
                        printf("%s\n", dir_names[i]);
                }
            }

            for (int i = 0; i < count; i++) {
                free(dir_names[i]);
            }
            free(dir_names);
        }
    }
    return 0;
}

/*
 * function to sort two strings
 * @param str1: first string
 * @param str2: second string
 * @return: 0 if the strings are equal, <0 if str1 is less than str2, >0 if str1 is greater than str2
 */
int sort_string( const void *str1, const void *str2 ) {
    char *const *pp1 = str1;
    char *const *pp2 = str2;

    char *copy2 = strdup(*pp2);
    char *copy1 = strdup(*pp1);
    if (copy1 == NULL || copy2 == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; copy1[i]; i++) {
        copy1[i] = (char) tolower(copy1[i]);
    }
    for (int i = 0; copy2[i]; i++) {
        copy2[i] = (char) tolower(copy2[i]);
    }

    int result = strcmp(copy1, copy2);
    free(copy1);
    free(copy2);

    return result;
}

/*
 * function to sort an array of strings
 * @param lines: array of strings
 * @param count: number of strings
 *
 * sorts the array of strings using the sort_string function
 */
void sort_util(char* lines[], int count) {
    qsort(lines, count, sizeof(*lines), sort_string);
}

/*
 * function to print the stats of a file
 * @param item: file name
 *
 * prints the file type, permissions, number of hard links, owner name, group name, file size, last modification time, and file name
 * checks also the time of the last modification and prints the year if it is more than 6 months ago
 */
void print_stats(const char *item) {
    struct stat file_stat;
    if(stat(item, &file_stat) < 0) {
        perror("stat");
        exit(1);
    }

    // File type
    printf( (S_ISDIR(file_stat.st_mode)) ? "d" : "-");

    // Owner permissions
    printf( (file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (file_stat.st_mode & S_IXUSR) ? "x" : "-");

    // Group permissions
    printf( (file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (file_stat.st_mode & S_IXGRP) ? "x" : "-");

    // Other permissions
    printf( (file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf( (file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (file_stat.st_mode & S_IXOTH) ? "x" : "-");

    // Number of hard links
    printf(" %d", (int)file_stat.st_nlink);

    // Owner name
    struct passwd *pw = getpwuid(file_stat.st_uid);
    printf(" %s", pw ? pw->pw_name : "unknown");

    // Group name
    struct group *gr = getgrgid(file_stat.st_gid);
    printf(" %s", gr ? gr->gr_name : "unknown");

    // File size
    printf(" %d", (int)file_stat.st_size);

    // Last modification time
    time_t current_time;
    time(&current_time);
    struct tm *tm_info = localtime(&file_stat.st_mtime);
    if (difftime(current_time, file_stat.st_mtime) > 6L * 30L * 24L * 60L * 60L) { // More than 6 months
        printf(" %s %d %d", month_name(tm_info->tm_mon), tm_info->tm_mday, tm_info->tm_year + 1900);
    } else { // Within 6 months
        printf(" %s %d %02d:%02d", month_name(tm_info->tm_mon), tm_info->tm_mday, tm_info->tm_hour, tm_info->tm_min);
    }

    printf(" %s\n", get_file_name(item));
}

/*
 * function to get the file name from a path
 * @param name: path
 * @return: file name
 *
 * finds the last occurrence of '/' or '\\' in the path
 * if it finds it, it returns the string after the last occurrence
 * otherwise, it returns the entire path
 */
const char* get_file_name(const char* name) {
    const char *filename;

    // Find the last occurrence of '/' or '\\' in the path
    filename = strrchr(name, '/');
    if (filename == NULL) {
        filename = strrchr(name, '\\');
    }

    if (filename != NULL) {
        filename++; // Move past the '/' or '\\'
    }
        // Otherwise, the entire path is the file name
    else {
        filename = name;
    }
    return filename;
}

/*
 * function to get the month name
 * @param month: month number
 * @return: month name
 *
 * returns the month name based on the month number
 */
const char *month_name(int month) {
    static const char *names[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    return names[month];
}
