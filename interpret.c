#include "interpret.h"

void interpret()
{
    int opInt1, opInt2;					//promenne pro ulozeni operandu, pro prehlednost
	double opDouble1, opDouble2, pom;
	
	int T; //globalni promenna pro skoky 
	char * returnValue; //promenna pro ulozeni retezce z funkce substring

    I_variable * stackPointer;			//pomocny ukazatel nekam do seznamu promennych

	I_instruction *next1;		//dalsi instrukce na seznamu
	I_ListOfVariables ListOfVariables;	//struktura obsahujici informace o jednosmerne vazanem seznamu promennych
	
	ListOfVariables.first = NULL;		//a jeji inicializace
	ListOfVariables.last = NULL;
	
	I_ListOfPointers ListOfPointers;	//struktura obsahujici informace o jednosmerne vazanem seznamu ukazatelu na instrukce
	
	ListOfPointers.first = NULL;		//a jeji inicializace
	ListOfPointers.last = NULL;
	
	/*char *name1 = "foo1";		//kus testovaciho kodu, muze se hodit pro budouci casy
	char *name2 = "foo2";
	char *name3 = "foo3";
	char *name4 = "foo4";
	
	I_item op1;
	I_item op2;
	I_item op3;
	I_item op4;
	
	op1.value_type = TI_INT;
	op2.value_type = TI_INT;
	op3.value_type = TI_INT;
	op4.value_type = TI_INT;
	
	op1.value.i = 1;
	op2.value.i = 2;
	op3.value.i = 3;
	op4.value.i = 4;
	
	
	
	addToEnd(&ListOfVariables, name1, &op1);
	addToEnd(&ListOfVariables, name2, &op2);
	addToEnd(&ListOfVariables, name3, &op3);
	addToEnd(&ListOfVariables, name4, &op4);
	
	printf("\n%d\n",ListOfVariables.first->next->value.i);
	delVariable(&ListOfVariables, name1);
	printf("\n%d\n",ListOfVariables.first->next->value.i);*/


	
	
	while(instruction->inst_type != I_STOP)
	{
		next1 = instruction->next;
		
		switch ((*instruction).inst_type)
		{
			case I_READ_INT: 	//Nacti int do op1 a nastav typ!
					stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
					stackPointer->value_type = TI_INT;
					if(scanf("%d", &(stackPointer->value.i)) == 0)
						error(READ_ERROR);
					if(stackPointer->value.i < 0)
						error(READ_ERROR);
						
					stackPointer->inicialized = 1;
			break;

			case I_READ_DOUBLE: 	//Nacti double do op1 a nastav typ!
	
					stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
					stackPointer->value_type = TI_DOUBLE;
					if(scanf("%lf", &(stackPointer->value.d)) == 0)
						error(READ_ERROR);
					if(stackPointer->value.d < 0.0)
						error(READ_ERROR);
						
					stackPointer->inicialized = 1;
	
			break;

			case I_READ_STRING: 	//Nacti string do op1 a nastav typ!

					stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
					stackPointer->value_type = TI_STRING;
					if(scanf("\"%s\"", stackPointer->value.s) == 0)
						error(READ_ERROR);
						
					stackPointer->inicialized = 1;
					
			break;

			case I_READ_BOOL: 	//Nacti bool do op1 a nastav typ!
						
			break;

			case I_WRITE:	//Vypis op1 na vystup
						
				switch(instruction->op1.value_type)
				{

					case TI_INT:
						printf("%d", findInt(&(instruction->op1), &ListOfVariables));
					break;
					
					case TI_DOUBLE:
						printf("%f", findDouble(&(instruction->op1), &ListOfVariables));
					break;
					
					case TI_STRING:
						printf("%s", findString(&(instruction->op1), &ListOfVariables));
					break;	
					
					default: break;				
				}
				
			break;

			case I_MULTIPLY:  //op3 = op1 * op2
										
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				
				switch(instruction->op3.value_type)
				{
					case TI_INT:										//vysledek je int, vsechno budu zpracovava v pomocne double a pak oriznu
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								pom = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								pom *= (double)findInt(&(instruction->op2),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom *= findDouble(&(instruction->op2),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						stackPointer->value.i = (int)pom;
					break;
					
					case TI_DOUBLE:										//vysledek je double, vsechno si postupne prevadim do op3 na double
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.d = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								stackPointer->value.d *= (double)findInt(&(instruction->op2),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d *= findDouble(&(instruction->op2),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
				stackPointer->inicialized = 1;
				
			break;

			case I_DIVIDE:  //op3 = op1 / op2
																							
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				
				switch(instruction->op3.value_type)
				{
					case TI_INT:										//vysledek je int, vsechno budu zpracovava v pomocne double a pak oriznu
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								pom = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								opDouble2 = (double)findInt(&(instruction->op2),&ListOfVariables);
								
								if(opDouble2 == 0)
									error(DIVISION_BY_ZERO);
								
								pom /= opDouble2;
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2),&ListOfVariables);
								
								if(opDouble2 == 0)
									error(DIVISION_BY_ZERO);
									
								pom /= opDouble2;
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						stackPointer->value.i = (int)pom;
					break;
					
					case TI_DOUBLE:										//vysledek je double, vsechno si postupne prevadim do op3 na double
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.d = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								opDouble2 = (double)findInt(&(instruction->op2),&ListOfVariables);
								
								if(opDouble2 == 0)
									error(DIVISION_BY_ZERO);
									
								stackPointer->value.d /= opDouble2;
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2),&ListOfVariables);
								
								if(opDouble2 == 0)
									error(DIVISION_BY_ZERO);
									
								stackPointer->value.d /= opDouble2;
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
				stackPointer->inicialized = 1;
								
			break;

			case I_ADD:   //op3 = op1 + op2
														
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				
				switch(instruction->op3.value_type)
				{
					case TI_INT:										//vysledek je int, vsechno budu zpracovava v pomocne double a pak oriznu
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								pom = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								pom += (double)findInt(&(instruction->op2),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom += findDouble(&(instruction->op2),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						stackPointer->value.i = (int)pom;
					break;
					
					case TI_DOUBLE:										//vysledek je double, vsechno si postupne prevadim do op3 na double
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.d = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								stackPointer->value.d += (double)findInt(&(instruction->op2),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d += findDouble(&(instruction->op2),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
				stackPointer->inicialized = 1;
				
			break;

			case I_SUB:   //op3 = op1 - op2
														
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				
				switch(instruction->op3.value_type)
				{
					case TI_INT:										//vysledek je int, vsechno budu zpracovava v pomocne double a pak oriznu
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								pom = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								pom -= (double)findInt(&(instruction->op2),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								pom -= findDouble(&(instruction->op2),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						if(pom < 0)
							error(OTHER);
							
						stackPointer->value.i = (int)pom;
					break;
					
					case TI_DOUBLE:										//vysledek je double, vsechno si postupne prevadim do op3 na double
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.d = (double)findInt(&(instruction->op1),&ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d = findDouble(&(instruction->op1),&ListOfVariables);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
						
						switch(instruction->op2.value_type)
						{
							case TI_INT:
								stackPointer->value.d -= (double)findInt(&(instruction->op2),&ListOfVariables);
								if(stackPointer->value.d < 0)
									error(OTHER);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d -= findDouble(&(instruction->op2),&ListOfVariables);
								if(stackPointer->value.d < 0)
									error(OTHER);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
				stackPointer->inicialized = 1;
				
			break;

			case I_LESS: //T = 1 if op1 < op2 else T = 0
																		
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 < opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 < opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 < opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 < opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_MORE: //T = 1 if op1 > op2 else T = 0
																								
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 > opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 > opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 > opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 > opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
			break;

			case I_LESS_EQUAL: //T = 1 if op1 <= op2 else T = 0
																								
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 <= opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 <= opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 <= opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 <= opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_MORE_EQUAL: //T = 1 if op1 >= op2 else T = 0
																								
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 >= opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 >= opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 >= opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 >= opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_NOT_EQUAL: //T = 1 if op1 != op2 else T = 0
																								
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 != opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 != opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 != opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 != opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_EQUAL: //T = 1 if op1 == op2 else T = 0
																								
				switch(instruction->op1.value_type)
				{
					case TI_INT:
						opInt1 = findInt(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opInt1 == opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opInt1 == opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						opDouble1 = findDouble(&(instruction->op1), &ListOfVariables);
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								opInt2 = findInt(&(instruction->op2), &ListOfVariables);
								opDouble1 == opInt2 ? (T = 1) : (T = 0);
							break;
							
							case TI_DOUBLE:
								opDouble2 = findDouble(&(instruction->op2), &ListOfVariables);
								opDouble1 == opDouble2 ? (T = 1) : (T = 0);
							break;
							
							default:
								error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					default:
						error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_UNARY_MINUS: //rozsireni? op1--;
				
				stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
				
				if(stackPointer->inicialized == 0)
					error(UNINITIALIZED_VARIABLE);
															
				switch(stackPointer->value_type)
				{
					case TI_INT:
						stackPointer->value.i--;
					break;
					
					case TI_DOUBLE:
						stackPointer->value.d--;
					break;
					
					default:
					error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_UNARY_PLUS: //rozsireni? op1++;
																		
				stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
				
				if(stackPointer->inicialized == 0)
					error(UNINITIALIZED_VARIABLE);
																		
				switch(stackPointer->value_type)
				{
					case TI_INT:
						stackPointer->value.i++;
					break;
					
					case TI_DOUBLE:
						stackPointer->value.d++;
					break;
					
					default:
					error(SEMANTIC_EXPRESSION);
					break;
				}
				
			break;

			case I_IF: //kontroluje globalni promennou T: if true - op1: else - op2 jump
				if(T)
				{
					next1 = instruction->op1.value.target;
				}
				else
				{
					next1 = instruction->op2.value.target;
				}
			break;

			case I_JUMP: //jump to op1
				next1 = instruction->op1.value.target;
			break;

			case I_ASSIGN: //op3 = op1;
				
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				switch(stackPointer->value_type)
				{
					case TI_INT:
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.i = findInt(&(instruction->op1), &ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.i = (int)findDouble(&(instruction->op1), &ListOfVariables);
							break;
							
							default:
							error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_DOUBLE:
						switch(instruction->op1.value_type)
						{
							case TI_INT:
								stackPointer->value.d = (double)findInt(&(instruction->op1), &ListOfVariables);
							break;
							
							case TI_DOUBLE:
								stackPointer->value.d = findDouble(&(instruction->op1), &ListOfVariables);
							break;
							
							default:
							error(SEMANTIC_EXPRESSION);
							break;
						}
					break;
					
					case TI_STRING:
						if(instruction->op1.value_type != TI_STRING)
						{
							error(SEMANTIC_EXPRESSION);
						}
					break;
					
					default:
					error(SEMANTIC_EXPRESSION);
					break;
				}
				
				stackPointer->inicialized = 1;
				
			break;

			case I_LENGHT: //interni funkce - op3 = length(op1)
			
				if(instruction->op1.value_type != TI_STRING)
					error(SEMANTIC_EXPRESSION);
					
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				stackPointer->value.i = length(findString(&(instruction->op1),&ListOfVariables));
				stackPointer->inicialized = 1;
			break;

			case I_SUBSTR: //interni funkce - returnValue = substr(op1, op2, op3)
			
				if(instruction->op1.value_type != TI_STRING || instruction->op2.value_type != TI_INT || instruction->op3.value_type != TI_INT)
					error(SEMANTIC_EXPRESSION);
					
				returnValue = (char *)substr(findString(&(instruction->op1),&ListOfVariables), findInt(&(instruction->op2),&ListOfVariables), findInt(&(instruction->op3),&ListOfVariables));
			break;

			case I_CONCATE:  //op3 = concat(op1:op2)
			
				if(instruction->op1.value_type != TI_STRING || instruction->op2.value_type != TI_STRING)
					error(SEMANTIC_EXPRESSION);
			
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				stackPointer->value.s = (char *)concat(findString(&(instruction->op1),&ListOfVariables), findString(&(instruction->op2),&ListOfVariables));
				stackPointer->inicialized = 1;
			break;

			case I_FIND: //op3 = find(op1: op2)
			
				if(instruction->op1.value_type != TI_STRING || instruction->op2.value_type != TI_STRING)
					error(SEMANTIC_EXPRESSION);
					
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				stackPointer->value.i = find(findString(&(instruction->op1),&ListOfVariables), findString(&(instruction->op2),&ListOfVariables));
				stackPointer->inicialized = 1;
			break;

			case I_SORT: //op3 = sort(op1)
			
				if(instruction->op1.value_type != TI_STRING)
					error(SEMANTIC_EXPRESSION);
					
				stackPointer = findVariable(&ListOfVariables, instruction->op3.value.s);
				stackPointer->value.s = (char *)sort(findString(&(instruction->op1),&ListOfVariables));
				stackPointer->inicialized = 1;
			break;
			
			case I_STORE: //uloz na konec seznamu promennych, op1 = value.s = nazev, value_type
				addToEnd(&ListOfVariables, &instruction->op1);
			break;
			
			case I_DELETE: //smaz ze seznamu promennych posledni promennou, jejiz nazev se shoduje s op1
				delVariable(&ListOfVariables, instruction->op1.value.s);
			break;
			
			case I_STOSUB: //op1 = *returnValue;
				instruction->op1.value.s = returnValue;
			break;
			
			case I_TYPECHNG: //najdi op1 a zmen jeho typ na typ podle inst_type op1
				stackPointer = findVariable(&ListOfVariables, instruction->op1.value.s);
				stackPointer->value_type = instruction->op1.value_type;
			break;
			
			case I_NOP:		//prazdna instrukce
			
			break;			
			
			case I_CALL:		//ulozi na zasobnik odkaz na dalsi instrukci a skoci na op1
				addToEndPointers(&ListOfPointers, next1);
				next1 = instruction->op1.value.target;
			break;
			
			case I_RET:		//vrati se tam, kam odkazuje vrchol zasobniku a smaze ho
				next1 = popLastPointer(&ListOfPointers);
			break;
			default: break;
		}
		instruction = next1;
	}
}

int findInt(I_item *operand, I_ListOfVariables *stack)
{
	I_variable *result;
	if(operand->onStack)
	{
		result = findVariable(stack, operand->value.s);
		
		if(result->inicialized == 0)
			error(UNINITIALIZED_VARIABLE);
		
		return result->value.i;
	}
	else
	{
		return operand->value.i;
	}
	return 0;
}

double findDouble(I_item *operand, I_ListOfVariables *stack)
{
	I_variable *result;
	if(operand->onStack)
	{
		result = findVariable(stack, operand->value.s);
		
		if(result->inicialized == 0)
			error(UNINITIALIZED_VARIABLE);
			
		return result->value.d;
	}
	else
	{
		return operand->value.d;
	}
	return 0;
}

char* findString(I_item *operand, I_ListOfVariables *stack)
{
	I_variable *result;
	if(operand->onStack)
	{
		result = findVariable(stack, operand->value.s);
		
		if(result->inicialized == 0)
			error(UNINITIALIZED_VARIABLE);
			
		return result->value.s;
	}
	else
	{
		return operand->value.s;
	}
	return 0;
}
