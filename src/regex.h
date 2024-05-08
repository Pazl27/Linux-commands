#pragma once

int match(char *regexp, char *text);
int match_here(char *regexp, char *text);
int match_star(int c, char *regexp, char *text);
int match_question(char c, char *regexp, char *text); // New function
