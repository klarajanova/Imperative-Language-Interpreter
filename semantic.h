#ifndef IFJ15_SEMANTIC
#define IFJ15_SEMANTIC

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "stack.h"
#include "token.h"

#define VAL_INT 1
#define VAL_DOUBLE 2
#define VAL_STRING 3

#define FX_DECLARED 1
#define FX_DEFINED 2

#define SEMANTIC_DATA_TYPE 5 // jest TYPE_DERIVATIVE_ERROR v error.h
#define SEMANTIC_OTHER 6

struct fx_tab_itm {
    int rettype;
    char* identificator;
    struct stack* var_stack;
    int argc;
    struct fx_arg* arguments;
    int defined;
    struct fx_tab_itm* next;
};

struct fx_arg {
    int type;
    char* identificator;
//     struct fx_arg *next;
};

struct fx_tab
{
    struct fx_tab_itm* tab[10];
};

struct var_tab_itm
{
    char* identificator;
    int type;
    struct var_tab_itm* next;
};

struct var_tab* var_tab_init ();

struct var_tab
{
    struct var_tab_itm* tab[10];
};

int generate_hash(char* string);    //generates index into hashed table of symbols from string

int seek_fx(char* identificator, struct fx_tab_itm** ret_itm);
int declare_fx(char* identificator, int ret_type);
void end_declare_fx();
int define_fx(); //Prepne do rezimu definice FX z rezimu deklarace
int add_fx_arg(int arg_datatype, char* arg_identificator);

int seek_var(char* identificator, struct var_tab_itm** data_type);
int seek_var_all_stack(char* identificator, struct var_tab_itm** ret_itm);
int add_var(int datatype, char* identificator);
int var_tab_body_push();        //pushes var symtab to stack of the currently defined function
int var_tab_body_pop();         //pops var symtab from stack of the currently defined function
int var_by_type(char *identificator);

struct var_tab* var_tab_init ();



void print_fx_tab(); //TODO tuto funkci ostranit
void print_last_fx_args(); //TODO tuto funkci ostranit
void print_var_tab();


#endif