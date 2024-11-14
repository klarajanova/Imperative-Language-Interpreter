#ifndef IFJ15_INST_LIST_H
#define IFJ15_INST_LIST_H

#include <string.h>
#include "error.h"
#include "helper.h"

typedef enum
{
	I_STOP, 	//Konec seznamu
	I_READ_INT, 	//Nacti int do op1 a nastav typ!
	I_READ_DOUBLE, 	//Nacti double do op1 a nastav typ!
	I_READ_STRING, 	//Nacti string do op1 a nastav typ!
	I_READ_BOOL, 	//Nacti bool do op1 a nastav typ! - rozsireni
	I_WRITE,	//Vypis op1 na vystup
	I_MULTIPLY,  //op3 = op1 * op2
	I_DIVIDE,  //op3 = op1 / op2
	I_ADD,   //op3 = op1 + op2
	I_SUB,   //op3 = op1 - op2
	I_LESS, //T = 1 if op1 < op2 else T = 0
	I_MORE, //T = 1 if op1 > op2 else T = 0
	I_LESS_EQUAL, //T = 1 if op1 <= op2 else T = 0
	I_MORE_EQUAL, //T = 1 if op1 >= op2 else T = 0
	I_NOT_EQUAL, //T = 1 if op1 != op2 else T = 0
	I_EQUAL, //T = 1 if op1 == op2 else T = 0
	I_UNARY_MINUS, //rozsireni? op1--;
	I_UNARY_PLUS, //rozsireni? op1++;
	I_IF, //kontroluje globalni promennou T, if true - op1, else - op2 jump
	I_JUMP, //jump to op1
	I_ASSIGN, //op3 = op1
	I_LENGHT, //interni funkce - op3 = length(op1)
	I_SUBSTR, //interni funkce - returnValue = substr(op1, op2, op3)
	I_CONCATE,  //op3 = concat(op1,op2)
	I_FIND, //op3 = find(op1, op2)
	I_SORT, //op3 = sort(op1)
	I_STORE, //uloz na konec seznamu promennych, op1 = value, value_type, op2.value.s = nazev
	I_DELETE, //smaz ze seznamu promennych posledni promennou, jejiz nazev se shoduje s op1
	I_STOSUB, //op1 = *returnValue;
	I_TYPECHNG, //najdi op1 a zmen jeho typ na typ podle inst_type op1
	I_NOP,		//prazdna instrukce
	I_CALL,		//ulozi na zasobnik odkaz na dalsi instrukci a skoci na op1
	I_RET		//vrati se tam, kam odkazuje vrchol zasobniku a smaze ho
}inst_list;

//tříadresný kód, bude použit ve struktuře formátu: instrukce - operand - operand - místo uložení
//operandy a místo uložení budou implementovány jako struktura, kde bude nejdříve typ operandu a pak union

typedef enum //typy operandů
{
	TI_INT,
	TI_DOUBLE,
	TI_STRING,
	TI_BOOL, //rozsireni?
	TI_POINTER, //ukazatel na strukturu I_INSTRUCTION
	//TI_STACK  //promenna se nachazi na stacku, z value budu cist char
} type_list;

typedef union
{
	int i;
	double d;
	char *s;
	int b; //boolean
	struct I_INSTRUCTION *target;
}value_list;

typedef struct 
{
	type_list value_type;
	value_list value;
	int onStack;		//1 kdyz mam hodnotu hledat na zasobniku promennych, 0 kdyz je primo v instrukci
}I_item;

typedef struct I_INSTRUCTION
{
	inst_list inst_type;
	I_item op1;
	I_item op2;
	I_item op3;
	struct I_INSTRUCTION *next;
}I_instruction;

typedef struct I_VARIABLE
{
	type_list value_type;
	value_list value;
	int inicialized;		//1 if inicialized, 0 if not
	char *identificator;
	struct I_VARIABLE *next;
}I_variable;

typedef struct
{
	struct I_VARIABLE *first;
	struct I_VARIABLE *last; 
}I_ListOfVariables;

typedef struct I_POINTER
{
	I_instruction *pointer;
	struct I_POINTER *next;
}I_pointer;

typedef struct
{
	struct I_POINTER *first;
	struct I_POINTER *last;
}I_ListOfPointers;

int isEmpty(I_ListOfVariables *ListOfVariables);
void addToEnd(I_ListOfVariables *ListOfVariables, I_item *variable);
I_variable * findVariable(I_ListOfVariables *ListOfVariables, char *name);
void delVariable(I_ListOfVariables *ListOfVariables, char *name);

int isEmptyPointers(I_ListOfPointers *ListOfPointers);
void addToEndPointers(I_ListOfPointers *ListOfPointers, I_instruction *pointer);
I_instruction * popLastPointer(I_ListOfPointers *ListOfPointers);

extern I_instruction *instruction;
I_item *set_item (type_list type, void *value);
void add_instruction (inst_list inst, I_item *op1, I_item *op2, I_item *op3);

#endif // IFJ15_INST_LIST_H
