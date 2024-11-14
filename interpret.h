#ifndef IFJ15_INTERPRET_H
#define IFJ15_INTERPRET_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ial.h"
#include "inst_list.h"

int findInt(I_item *operand, I_ListOfVariables *stack);
double findDouble(I_item *operand, I_ListOfVariables *stack);
char* findString(I_item *operand, I_ListOfVariables *stack);
void interpret();

#endif // IFJ15_INTERPRET_H
