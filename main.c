#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "expr.h"
#include "helper.h"
#include "inst_list.h"
#include "interpret.h"
#include "parser.h"
#include "scanner.h"
#include "semantic.h"
#include "stack.h"

#define _CRT_SECURE_NO_WARNINGS
#define FILENAME argv[1]


struct stack *stack_token;
struct stack *stack_instruction;
I_instruction *instruction;

int main (int argc, char **argv)
{
    I_item *item;

    if (argc != 2)
        error (INTERNAL_ERROR);

    instruction = NULL;

    stack_token = scanner (FILENAME);
    parseAll ();
#ifdef _DEBUG
   print_fx_tab(); 
#endif
    add_instruction (I_STOP, NULL, NULL, NULL);
    interpret ();

    stack_delete (stack_token);

#ifdef _WIN32
	system ("PAUSE");
#endif
	return (EXIT_SUCCESS);
}
