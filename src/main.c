#include <stdio.h>
#include <string.h>
#include <alloca.h>
#include <stdbool.h>

#include "ls.h"
#include "cat.h"
#include "grep.h"
#include "wc.h"
#include "sort.h"

bool endsWith(const char *arg, const char *command);

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

/*
 * Function that checks if the arg ends with the command
 * @param arg the argument
 * @param command the command
 * @return true if the arg ends with the command, false otherwise
 */
bool endsWith(const char *arg, const char *command) {
    char *lastSlash = strrchr(arg, '/');
    return lastSlash == NULL && strcmp(arg, command) == 0 ||
           lastSlash != NULL && strcmp(lastSlash + 1, command) == 0;
}