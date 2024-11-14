#ifndef IFJ15_PARSER_H
#define IFJ15_PARSER_H
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "expr.h"
#include "helper.h"
#include "inst_list.h"
#include "parser.h"
#include "scanner.h"
#include "stack.h"
#include "token.h"
#include "semantic.h"

void parseAll();
void parseBody();
void parseCin();
void parseCout();
void parseCommand();
void parseCondition();
void parseDataType();
void parseDefinition();
void parseFunction(Token *dt_token, Token *id_token);
void parseFunctionArguments(Token *token);
void parseFor();
void parseReturn();
void parseVariable();
void parseWhile();

#endif //IFJ15_PARSER_H