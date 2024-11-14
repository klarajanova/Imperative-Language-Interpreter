#ifndef IFJ15_SCANNER_H
#define IFJ15_SCANNER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #define true 1
    #define false 0
    typedef char bool;
#else
    #include <stdbool.h>
#endif

#include "error.h"
#include "helper.h"
#include "stack.h"
#include "token.h"

struct stack *scanner (char *filename);
Token *get_token (struct stack *stack);
void destroy_token (Token *token);

#endif //IFJ15_SCANNER_H
