#include "regex.h"

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
