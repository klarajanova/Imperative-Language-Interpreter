#include "parser.h"

extern struct stack *stack_token;

//------------------------------------------------------------------------------pomocne funkce
int checkDataType(Token *token)
{
	if (token->type == KEYWORD_INT || token->type == KEYWORD_DOUBLE ||
		token->type == KEYWORD_STRING)
		return 1;
	else
		return 0;
}

//------------------------------------------------------------------------------hlavni funkce
/**
* parseVariable is called under the condition that switch finds tokens in this sequence: "dataType identifier"
* continues checking for assignment[variable definition] or semicolon[variable declaration]
*/
void parseVariable(Token *token)
{
	if (token->type != ASSIGNMENT)
	{
		if (token->type != SEMICOLON)
		{
#ifdef _DEBUG
			printf("parseVariable(): a semicolon missing\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
		else //datTyp ID;
		{
			token = get_token(stack_token);

			if (token->type == LEFT_BRACE)
			{
				parseBody();
			}
			else
			{
#ifdef _DEBUG
				printf("parseVariable(): variable declaration successfully parsed\n\n");
#endif
				stack_push(stack_token, token);
			}
		}
	}
	else //dattyp Id = <expr>;
	{
#ifdef _DEBUG
		printf("parseVariable(): assignment: now an expression expected\n\n");
#endif
		precedence();
		token = get_token(stack_token);

		if (token->type == SEMICOLON) //typ ID = expr; 
		{
#ifdef _DEBUG
			printf("parseVariable(): variable definition successfully parsed\n\n");
#endif
		}
		else
		{
#ifdef _DEBUG
			printf("parseVariable(): semicolon missing after assignment\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
}

/**
* parseBody() checks the validity of bodies of every kind.
* {loops, functions, variables,...}
*/
void parseBody()
{
	Token *token = get_token(stack_token);
        
        var_tab_body_push();


	while (token->type != RIGHT_BRACE)
	{
		stack_push(stack_token, token);
		parseCommand();
		token = get_token(stack_token);
	}
	
	var_tab_body_pop();
}

/**
* parseCondition is called under the condition that switch finds tokens in this sequence: "if("
* continues checking for "condition body" of type: example (i<3)
* if that's valid, parseBody() is called and the action is repeated with "else{}"
*/
void parseCondition()
{
	Token *token;
	token = get_token(stack_token);

	if (token->type != LEFT_PARENTHESES) // if(
	{
#ifdef _DEBUG
		printf("parseCondition(): first brace of the if-condition part is missing\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
	else
	{
		precedence();
		token = get_token(stack_token);
		if (token->type != RIGHT_PARENTHESES) // if(*
		{
#ifdef _DEBUG
			printf("parseCondition(): closing brace of the if-condition part's missing\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
		else // if()
		{
			token = get_token(stack_token);
			if (token->type != LEFT_BRACE) //if() 
			{
#ifdef _DEBUG
				printf("parseCondition(): a left parenthesis's missing-no if-command body\n\n");
#endif
				destroy_token(token);
				error(SYNTAX_ERROR);
			}
			else
			{
				parseBody();

				token = get_token(stack_token);
				if (token->type == KEYWORD_ELSE)
				{
					token = get_token(stack_token);
					if (token->type != LEFT_BRACE)
					{
#ifdef _DEBUG
						printf("parseCondition(): a left parenthesis expected-no command body\n\n");
#endif
						destroy_token(token);
						error(SYNTAX_ERROR);
					}
					else
					{
#ifdef _DEBUG
						printf("parseCondition(): check the body of else condition\n\n");
#endif
						parseBody();
					}

				}
				else
				{
#ifdef _DEBUG
					printf("parseCondition(): else part is missing\n\n");
#endif
					destroy_token(token);
					error(SYNTAX_ERROR);
				}
			}
		}
	}
}

/**
* parseFunctionArguments() is called under the condition that switch finds tokens in this sequence: "dataType identifier("
* continues checking for function arguments in this sequence "dataType identifier, dataType identifier2,...,dataType2 identifierN)"
* FUNCTION CALLING is not supported yet, because checking symbol table is needed for that
* in case of different input, SYNTAX_ERROR is on the output
*/
void parseFunctionArguments(Token *token)       //v token je datovy typ nacteneho argumentu
{
	Token *arg_type_token = token;  //ulozime dat.typ nacteneho argumentu

	if (checkDataType(token)) //typ ID(typ
	{
		token = get_token(stack_token);
		if (token->type != IDENTIFIER)
		{
#ifdef _DEBUG
			printf("parseFunctionArguments(): identifier's missing\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
		else //typ ID(typ ID
		{
			add_fx_arg(arg_type_token->type, token->data); //TODO osetrit navratovku
			token = get_token(stack_token);
			if (token->type == RIGHT_PARENTHESES)
			{
#ifdef _DEBUG
				printf("parseFunctionArguments(): parsing of arguments done\n\n");
#endif
			}
			else if (token->type == COMMA)
			{
				token = get_token(stack_token);
#ifdef _DEBUG
				printf("parseFunctionArguments(): recursive calling of this function\n\n");
#endif
				parseFunctionArguments(token);
			}
			else
			{
#ifdef _DEBUG
				printf("parseFunctionArguments(): a closing brace or a comma's missing among the arguments\n\n");
#endif
				destroy_token(token);
				error(SYNTAX_ERROR);
			}
		}
	}
	else //typ ID(
	{
#ifdef _DEBUG
		printf("parseFunctionArguments(): syntax error in the data type of args\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}

/**
* parseFor is called under the condition that switch finds tokens in this sequence: "for"
* continues checking for "condition body" of type: example (i; i<3;i++)
* if that's valid, parseBody() is called and checks the body of the for loop
*/
void parseFor()
{
	Token *token;
	token = get_token(stack_token);

	if (token->type == LEFT_PARENTHESES) //for(
	{
		token = get_token(stack_token);

		if (checkDataType(token)) //for(type 
		{
			token = get_token(stack_token);
			if (token->type == IDENTIFIER) //for(type i 
			{
				token = get_token(stack_token);

				if (token->type == SEMICOLON) //for(type i;
				{
#ifdef _DEBUG
					printf("parseFor(): sequence: for(type i; \n\n");
#endif
					stack_push(stack_token, token);
				}
				else if (token->type == ASSIGNMENT) //for(type i=
				{
#ifdef _DEBUG
					printf("parseFor(): sequence: for(type i= \n\n");
#endif
					precedence();
				}
				else //for(type i*
				{
#ifdef _DEBUG
					printf("parseFor(): syntax error: sequence: for(type i* \n\n");
#endif
					error(SYNTAX_ERROR);
				}
			}
			else
			{
#ifdef _DEBUG
				printf("parseFor(): syntax error: sequence: for(type * \n\n");
#endif
				error(SYNTAX_ERROR);
			}
		}
		else
		{
#ifdef _DEBUG
			printf("parseFor(): syntax error: for(* \n\n");
#endif
			error(SYNTAX_ERROR);
		}

		//left (
		token = get_token(stack_token);

		if (token->type == SEMICOLON)
		{
			precedence();//2.cast podminky
			token = get_token(stack_token);
			if (token->type == SEMICOLON)
			{
				token = get_token(stack_token);
				if (token->type == IDENTIFIER) //for(;...;i
				{
					token = get_token(stack_token);
					if (token->type == ASSIGNMENT)//for(;...;i=
					{
						precedence();//3.cast podminky
						token = get_token(stack_token);
						if (token->type == RIGHT_PARENTHESES)
						{
							token = get_token(stack_token);
							if (token->type == LEFT_BRACE)
							{
#ifdef _DEBUG
								printf("parseFor(): the body of a for loop\n\n");
#endif
								parseBody();
							}
							else
							{
#ifdef _DEBUG
								printf("parseFor(): the body of a for loop's missing\n\n");
#endif
								destroy_token(token);
								error(SYNTAX_ERROR);
							}
						}
						else
						{
#ifdef _DEBUG
							printf("parseFor(): ending brace of the condition missing\n\n");
#endif
							destroy_token(token);
							error(SYNTAX_ERROR);
						}
					}
					else
					{
#ifdef _DEBUG
						printf("parseFor(): assignment by the third condition missing\n\n");
#endif
						destroy_token(token);
						error(SYNTAX_ERROR);
					}
				}
				else
				{//id missing
#ifdef _DEBUG
					printf("parseFor(): identifier missing\n\n");
#endif
					error(SYNTAX_ERROR);
				}
			}
			else
			{
#ifdef _DEBUG
				printf("parseFor(): second semicolon in the loops condition part's missing\n\n");
#endif
				destroy_token(token);
				error(SYNTAX_ERROR);
			}
		}
		else
		{
#ifdef _DEBUG
			printf("parseFor(): first semicolon in the loops condition part's missing\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
	else
	{
#ifdef _DEBUG
		printf("parseFor(): the condition part of the for loop's missing\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}

/**
 * parseDoWhile() is called under the condition that switch finds tokens in this sequence: "do"
 * continues checking for condition body {}
 * continues checking for keyword "while", loop condition, ending with semicolon
 */
void parseDoWhile()
{
	Token *token;
	token = get_token(stack_token);

	if (token->type == LEFT_BRACE) //do{
	{
#ifdef _DEBUG
		printf("parseDoWhile(): body of do\n\n");
#endif
		parseBody();

		token = get_token(stack_token);
		if (token->type == KEYWORD_WHILE) //do{}while
		{
			token = get_token(stack_token);
			if (token->type == LEFT_PARENTHESES)//do {} while(
			{
				precedence();
				token = get_token(stack_token);

				if (token->type == RIGHT_PARENTHESES)
				{
					token = get_token(stack_token);

					if (token->type == SEMICOLON)
					{
#ifdef _DEBUG
						printf("parseDoWhile(): successfully parsed\n\n");
#endif
					}
					else
					{
#ifdef _DEBUG
						printf("parseDoWhile(): syntax error missing semicolon\n\n");
#endif
						error(SYNTAX_ERROR);
					}
				}
				else
				{
#ifdef _DEBUG
					printf("parseDoWhile(): syntax error missing while\n\n");
#endif
					error(SYNTAX_ERROR);
				}
			}
			else
			{
#ifdef _DEBUG
				printf("parseDoWhile(): syntax error missing while condition\n\n");
#endif
				error(SYNTAX_ERROR);
			}
		}
		else
		{
#ifdef _DEBUG
			printf("parseDoWhile(): syntax error missing while\n\n");
#endif
			error(SYNTAX_ERROR);
		}
	}
	else
	{
#ifdef _DEBUG
		printf("parseDoWhile(): error body do\n\n");
#endif
		error(SYNTAX_ERROR);
	}
}

/**
* parseWhile() is called under the condition that switch finds tokens in this sequence: "while"
* continues checking for "condition body" of type: example (i<3)
* if that's valid, parseBody() is called and checks the body of the while loop
*/
void parseWhile()
{
	Token *token;
	token = get_token(stack_token);

	if (token->type == LEFT_PARENTHESES)
	{
		precedence();
		token = get_token(stack_token);
		if (token->type == RIGHT_PARENTHESES)
		{
			token = get_token(stack_token);
			if (token->type == LEFT_BRACE) //while(){
			{
#ifdef _DEBUG
				printf("parseWhile(): the body of a while loop\n\n");
#endif
				parseBody();
			}
			else
			{
#ifdef _DEBUG
				printf("parseWhile(): body's missing\n\n");
#endif
				destroy_token(token);
				error(SYNTAX_ERROR);
			}
		}
		else
		{
#ifdef _DEBUG
			printf("parseWhile(): ending brace in the loop condition's missing\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
	else
	{
#ifdef _DEBUG
		printf("parseWhile(): the condition part of the loop's missing\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}

/**
* parseReturn is called under the condition that switch finds tokens in this sequence: "return"
* continues checking for semicolon
*/
void parseReturn()
{
	Token *token;
	token = get_token(stack_token);
	if (token->type == SEMICOLON)
	{
#ifdef _DEBUG
		printf("parseReturn(): return command successfully parsed\n\n");
#endif
	}
	else
	{
#ifdef _DEBUG
		printf("parseReturn(): semicolon missing\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}


void parseCin()
{
	Token *token;
	token = get_token(stack_token);

	if (token->type == ISTREAM) //cin >> ID >>..>>IDn;
	{
		token = get_token(stack_token);

		if (token->type == IDENTIFIER)
		{
#ifdef _DEBUG
			printf("parseCin(): recursive calling \n\n");
#endif
			parseCin();
		}
		else
		{
#ifdef _DEBUG
			printf("parseCin(): syntax error after '>>' character\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
	else if (token->type == SEMICOLON)
	{
#ifdef _DEBUG
		printf("parseCin(): the end of cin command\n\n");
#endif
	}
	else
	{
#ifdef _DEBUG
		printf("parseCin(): syntax error\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}


void parseCout()
{
	//I_item *op1;
	Token *token;
	token = get_token(stack_token);

	if (token->type == OSTREAM) //cout >> 123 >>"string">>... ;
	{
		token = get_token(stack_token);

	//	op1 = set_item(TI_STRING, token->data);
//		add_instruction(I_WRITE, op1, NULL, NULL);

		if (token->type == STRING || token->type == INT || token->type == DOUBLE)
		{
#ifdef _DEBUG
			printf("parseCout(): recursive calling \n\n");
#endif
			parseCout();
		}
		else
		{
#ifdef _DEBUG
			printf("parseCout(): syntax error after >>\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
	else if (token->type == SEMICOLON)
	{
#ifdef _DEBUG
		printf("parseCout(): konec prikazu cout\n\n");
#endif
	}
	else
	{
#ifdef _DEBUG
		printf("parseCout(): syntax error\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}

}

/**
* parseFunction() is called under the condition that switch finds tokens in this sequence: "dataType identifier("
* continues checking for existing arguments with parseFunctionArguments() calling
* when arguments are valid, it returns to this function and checks semicolon [function declaration]
* unless the semicolon is missing and the token is "{" - then checks functions body [function definition]
*/
void parseFunction(Token *dt_token, Token *id_token) //ze switche pride int id(
{
	int retVal;

	Token *token = get_token(stack_token);
	if (token->type == RIGHT_PARENTHESES) //typ ID()
	{
#ifdef _DEBUG
		printf("parseFunction(): void function arguments\n\n");
#endif

		retVal = declare_fx(id_token->data, dt_token->type);
		if (retVal > 0)
			error(retVal);
		end_declare_fx();

	}
	else
	{
		retVal = declare_fx(id_token->data, dt_token->type);
		if (retVal > 0)
			error(retVal);
		parseFunctionArguments(token);
#ifdef _DEBUG
                print_last_fx_args();
#endif
		end_declare_fx();
	}

	token = get_token(stack_token);
	if (token->type == SEMICOLON)
	{
#ifdef _DEBUG
		printf("parseFunction(): parsed function prototype\n\n");
#endif
	}
	else if (token->type == LEFT_BRACE) // dalsi token je {
	{
		retVal = define_fx();
		if (retVal > 0)
			error(retVal);

		parseBody();
	}
	else
	{
#ifdef _DEBUG
		printf("parseFunction(): not a function definition\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
}

/**
* parseDataType() helps to differentiate between a variable and a function
* according to the result calls parseVariable() or parseFunction()
*/
void parseDataType(Token *token)        //token contains datatype of fuction/variable
{
	Token *token2;                  //token2 contains identifier of function/variable
	Token *token3;                  //token3 contains next lexem after identificator for decision if function/variable
	token2 = get_token(stack_token);

	if (token2->type != IDENTIFIER)
	{
#ifdef _DEBUG
		printf("parseDataType(): case data type: identifier missing\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
	}
	else// typ id
	{
		token3 = get_token(stack_token);

		if (token3->type != LEFT_PARENTHESES)//typ id*
		{
#ifdef _DEBUG
			printf("parseDataType(): case data type: calling parseVariable\n\n");
#endif
                        add_var(token->type, token2->data);
                        
                        fprintf(stderr, "datatype: %d\n", var_by_type(token2->data));
                        
                        parseVariable(token3);
		}
		else // a function detected typ id(
		{
#ifdef _DEBUG
			printf("parseDataType(): case data type: calling parseFunction\n\n");
#endif
			parseFunction(token, token2);
		}
	}
}

/**
* parseDefinition() is a function for VARIABLE definition of auto modificator
*								   and variable definition with previously defined data type
* is called under the condition that switch finds tokens in sequence: "auto identifier" or "identifier"
* continues checking for assigment, because simple declaration of auto modificator is not permitted according to the instructions
*/
void parseDefinition()
{
	Token *token = get_token(stack_token);

	if (token->type != ASSIGNMENT)
	{
#ifdef _DEBUG
		printf("parseDefinition(): declaration of auto modificator's not permitted\n\n");
#endif
		destroy_token(token);
		error(TYPE_DERIVATIVE_ERROR);
	}
	else //dattyp Id = <expr>;
	{
		precedence();
		token = get_token(stack_token);

		if (token->type == SEMICOLON) //typ ID = expr; 
		{
#ifdef _DEBUG
			printf("parseDefinition(): variable definition successfully parsed\n\n");
#endif
		}
		else
		{
#ifdef _DEBUG
			printf("parseDefinition(): semicolon missing after assignment\n\n");
#endif
			destroy_token(token);
			error(SYNTAX_ERROR);
		}
	}
}

/**
* parseCommand() gets the first token and based on its type calls other functions.
*/
void parseCommand()
{
	Token *token;
	Token *token2;
	token = get_token(stack_token);

	switch (token->type)
	{
	case KEYWORD_INT:
	case KEYWORD_DOUBLE:
	case KEYWORD_STRING:
#ifdef _DEBUG
		printf("parseCommand(): CASE VARIABLE:\n\n");
#endif
		parseDataType(token);
		break;

	case KEYWORD_AUTO:
#ifdef _DEBUG
		printf("parseCommand(): CASE AUTO:\n\n");
#endif
		token2 = get_token(stack_token);

		if (token2->type != IDENTIFIER)
		{
#ifdef _DEBUG
			printf("parseCommand(): CASE AUTO: identifier missing\n\n");
#endif
			destroy_token(token);
			destroy_token(token2);
			error(SYNTAX_ERROR);
		}
		else
		{
			parseDefinition();
		}
		break;

	case IDENTIFIER:
		parseDefinition();
		break;

	case LEFT_BRACE:
#ifdef _DEBUG
		printf("parseCommand(): CASE LEFT PARENTHESIS:\n\n");
#endif
		destroy_token(token);
		parseBody();
		break;

	case KEYWORD_CIN:
#ifdef _DEBUG
		printf("parseCommand(): CASE CIN:\n\n");
#endif
		destroy_token(token);
		parseCin();
		break;

	case KEYWORD_COUT:
#ifdef _DEBUG
		printf("parseCommand(): CASE COUT:\n\n");
#endif
		destroy_token(token);
		parseCout();
		break;

	case KEYWORD_IF:
#ifdef _DEBUG
		printf("parseCommand(): CASE IF-ELSE:\n\n");
#endif
		destroy_token(token);
		parseCondition();
		break;

	case KEYWORD_FOR:
#ifdef _DEBUG
		printf("parseCommand(): CASE FOR:\n\n");
#endif
		destroy_token(token);
		parseFor();
		break;

	case KEYWORD_DO:
#ifdef _DEBUG
		printf("parseCommand(): CASE DO-WHILE:\n\n");
#endif
		destroy_token(token);
		parseDoWhile();
		break;

	case KEYWORD_WHILE:
#ifdef _DEBUG
		printf("parseCommand(): CASE WHILE:\n\n");
#endif
		destroy_token(token);
		parseWhile();
		break;

	case KEYWORD_RETURN:
#ifdef _DEBUG
		printf("parseCommand(): CASE RETURN:\n\n");
#endif
		precedence();
		parseReturn();
		break;

	case FUNCTION_FIND:
#ifdef _DEBUG
		printf("parseCommand(): CASE BUILT-IN FUNCTION FIND:\n\n");
#endif
		break;

	case FUNCTION_LENGTH:
#ifdef _DEBUG
		printf("parseCommand(): CASE BUILT-IN FUNCTION LENGTH:\n\n");
#endif
		break;

	case FUNCTION_CONCAT:
#ifdef _DEBUG
		printf("parseCommand(): CASE BUILT-IN FUNCTION CONCAT:\n\n");
#endif
		break;

	case FUNCTION_SUBSTR:
#ifdef _DEBUG
		printf("parseCommand(): CASE BUILT-IN FUNCTION SUBSTR:\n\n");
#endif
		break;

	case FUNCTION_SORT:
#ifdef _DEBUG
		printf("parseCommand(): CASE BUILT-IN FUNCTION SORT:\n\n");
#endif
		break;

	default:
#ifdef _DEBUG
		printf("parseCommand(): CASE DEFAULT: SYNTAX ERROR:\n\n");
#endif
		destroy_token(token);
		error(SYNTAX_ERROR);
		break;
	}
}

/**
* Main function of parser.c
* parseAll() calls parseCommand() until there's no token left.
*/
void parseAll()
{
	Token *token;
	
	while (((Token *)stack_token->top->data)->type != END_OF_FILE)
	{
		parseCommand();
	}
}