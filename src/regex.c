#include "regex.h"

/*
 * match: search for regexp anywhere in text
 * @param regexp: regular expression
 * @param text: text to search
 * @return 1 if found, 0 if not found
 *
 * This function is the entry point for the regular expression matching.
 * It checks if the regular expression starts with '^' and calls match_here
 * to check if the regular expression matches the text.
 * If the regular expression does not start with '^', it calls match_here
 * for each character in the text.
 * If match_here returns 1, it means the regular expression is found in the text.
 */
int match(char *regexp, char *text) {
    if (regexp[0] == '^')
        return match_here(regexp + 1, text);
    do {
        if (match_here(regexp, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}

/*
 * match_here: search for regexp at beginning of text
 * @param regexp: regular expression
 * @param text: text to search
 * @return 1 if found, 0 if not found
 *
 * This function is called by match to check if the regular expression
 * matches the text.
 * It checks if the regular expression is empty, if it is, it returns 1.
 * If the regular expression contains a '*', it calls match_star to handle
 * the '*' operator.
 * If the regular expression contains a '?', it calls match_question to handle
 * the '?' operator.
 * If the regular expression contains a '$' at the end, it checks if the text
 * is empty, if it is, it returns 1.
 * If the regular expression contains a '.' or the first character of the regular
 * expression matches the first character of the text, it calls match_here with
 * the next character of the regular expression and the next character of the text.
 * If none of the above conditions are met, it returns 0.
 */
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

/*
 * match_star: search for c*regexp at beginning of text
 * @param c: character to match
 * @param regexp: regular expression
 * @param text: text to search
 * @return 1 if found, 0 if not found
 *
 * This function is called by match_here to handle the '*' operator.
 * It calls match_here to check if the regular expression matches the text.
 * If it does, it returns 1.
 * If the text is not empty and the first character of the text matches c or c is '.',
 * it calls match_here with the same regular expression and the next character of the text.
 * If none of the above conditions are met, it returns 0.
 */
int match_star(int c, char *regexp, char *text) {
    do {
        if (match_here(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}

/*
 * match_question: search for c?regexp at beginning of text
 * @param c: character to match
 * @param regexp: regular expression
 * @param text: text to search
 * @return 1 if found, 0 if not found
 *
 * This function is called by match_here to handle the '?' operator.
 * It calls match_here to check if the regular expression matches the text.
 * If it does, it returns 1.
 * If the text is not empty and the first character of the text matches c or c is '.',
 * it calls match_here with the same regular expression and the next character of the text.
 * If none of the above conditions are met, it returns 0.
 */
int match_question(char c, char *regexp, char *text) {
    if (match_here(regexp, text))
        return 1;
    if (*text != '\0' && (*text++ == c || c == '.'))
        return match_here(regexp, text);
    return 0;
}
