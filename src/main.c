#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <alloca.h>
#include <stdlib.h>
#include <stdbool.h>

#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int cat(int argc, char *argv[]);
int doCat(bool lineNum, bool printCtrl, bool showEol, FILE *fpInput);
int ls(int argc, char *argv[]);
int doLs(bool allFiles, bool longList, bool listDirs, char** entry_list, int size);
int grep(int argc, char *argv[]);
int doGrep(bool caseInsensitive, bool invertMatch, char *pattern, const char *file);
int wc(int argc, char *argv[]);
int doCount(bool countLines, bool countWords, bool countChars, int length, char **files);
int sort(int argc, char *argv[]);
int doSort(bool sortReverse, int length, void *data);

bool endsWith(const char *arg, const char *command);

void str_to_lower(char *str);
int sort_string(const void *str1, const void *str2);
void sort_util(char* line[], int counter);

int match(char *regexp, char *text);
int match_here(char *regexp, char *text);
int match_star(int c, char *regexp, char *text);
int match_question(char c, char *regexp, char *text); // New function

void print_stats(const char *item);
const char* get_file_name(const char* name);
const char *month_name(int month);

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

int cat(int argc, char *argv[]) {
    int option = 0;
    bool showEol = false;
    bool lineNum = false;
    bool printCtrl = false;
    while ((option = getopt(argc, argv, "vEn")) != -1) {
        switch (option) {
            case 'n':
                lineNum = true;
                break;
            case 'v':
                printCtrl = true;
                break;
            case 'E':
                showEol = true;
                break;
        }
    }

    if (optind == argc) {
        doCat(lineNum, printCtrl, showEol, stdin);
    } else {
        for (int i = optind; i < argc; ++i) {
            FILE *fpInput = fopen(argv[i], "r");
            if (fpInput == NULL) {
                perror(argv[i]);
                continue;
            }
            doCat(lineNum, printCtrl, showEol, fpInput);
            fclose(fpInput);
        }
    }
    return 0;
}

int doCat(bool lineNum, bool printCtrl, bool showEol, FILE *fpInput) {
    int currentLine = 0;
    int currentChar;
    int lastChar = '\n';
    while ( (currentChar=fgetc(fpInput)) != -1) {
        if (lastChar == '\n') {
            if (lineNum) {
                printf("%5d ", currentLine++);
            }
        }
        if (currentChar == '\n') {
            if (showEol) {
                putc('$', stdout);
            }
            putc(currentChar, stdout);
        }
        else if ((currentChar >= 32 && currentChar <= 127) || isspace(currentChar)) {
            putc(currentChar, stdout);
        }
        else if ( printCtrl ) {
            if ( currentChar < 32 ) {
                putc('^', stdout);
                putc(currentChar+'@', stdout);
            }
            else {
                printf("\\%03o", currentChar);
            }
        }
        else {
            putc(currentChar, stdout);
        }
        lastChar = currentChar;
    }
    return 0;
}

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


void sort_util(char* lines[], int count) {
    qsort(lines, count, sizeof(*lines), sort_string);
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

const char *month_name(int month) {
    static const char *names[] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    return names[month];
}