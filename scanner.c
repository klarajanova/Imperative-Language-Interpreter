#include "scanner.h"

#define ALLOC_SIZE 16
#define ERROR -1
#define BIN_LENGTH 8
#define OCT_LENGTH 3
#define HEX_LENGTH 2

enum STATE {
    STATE_ADDITION,
    STATE_ASSIGNMENT,
    STATE_COMMA,
    STATE_DECREMENT,
    STATE_DIVISION,
    STATE_DOUBLE,
    STATE_DOUBLE_EXPONENT,
    STATE_END_OF_FILE,
    STATE_EQUAL,
    STATE_ESCAPE,
    STATE_ESCAPE_BIN,
    STATE_ESCAPE_HEX,
    STATE_ESCAPE_OCT,
    STATE_GREATER,
    STATE_GREATER_OR_EQUAL,
    STATE_IDENTIFIER,
    STATE_INCREMENT,
    STATE_INT,
    STATE_ISTREAM,
    STATE_KEYWORD,
    STATE_LEFT_BRACE,
    STATE_LEFT_PARENTHESES,
    STATE_LESS,
    STATE_LESS_OR_EQUAL,
    STATE_LOGICAL_NOT,
    STATE_MULTIPLE_LINE_COMMENT,
    STATE_MULTIPLE_LINE_COMMENT_PREEND,
    STATE_MULTIPLICATION,
    STATE_ONE_LINE_COMMENT,
    STATE_OSTREAM,
    STATE_NOT_EQUAL,
    STATE_RIGHT_BRACE,
    STATE_RIGHT_PARENTHESES,
    STATE_SEMICOLON,
    STATE_START,
    STATE_STRING,
    STATE_STRING_ESCAPE,
    STATE_STRING_ESCAPE_BIN,
    STATE_STRING_ESCAPE_OCT,
    STATE_STRING_ESCAPE_HEX,
    STATE_SUBTRACTION
};

const char *KEYWORDS[] = {
        "auto",
        "bool",
        "cin",
        "cout",
        "do",
        "double",
        "else",
        "false",
        "for",
        "if",
        "int",
        "return",
        "string",
        "true",
        "while",

        // functions
        "concat",
        "find",
        "length",
        "sort",
        "substr"
};

void set_token(FILE *file);
bool is_keyword(char *string);
enum TOKEN get_keyword_type(char *string);

Token *token;
static enum STATE state = STATE_START;
static int row = 1;
static int col = 1;

Token *_get_token(FILE *file) {
    token = (Token*) malloc(sizeof(Token));
    if (token == NULL)
        error(INTERNAL_ERROR);

    set_token(file);

    return token;
}

void set_token(FILE *file) {
    int c;
    int i = 0;
    unsigned int length;
    char *string;

    int counter = 0;
    char *tmp = NULL;

    //STATE_DOUBLE, STATE_DOUBLE_EXPONENT
    bool exponent = false;
    bool decimal_point = false;
    bool sign = false;
    bool exponent_number = false;

    string = (char *) malloc(ALLOC_SIZE);
    if (string == NULL)
        error(INTERNAL_ERROR);
    memset(string, 0, ALLOC_SIZE);

    while ((c = fgetc(file))) {
        if (((length = (unsigned int) strlen(string)) != 0) && (length % ALLOC_SIZE == ALLOC_SIZE - 1)) {
            string = (char *) realloc(string, length + ALLOC_SIZE + 1);
            if (string == NULL)
                error(INTERNAL_ERROR);
            memset(string + length, 0, ALLOC_SIZE + 1);
        }

        switch (state) {
            case STATE_START:
                i = 0;
                col++;
                memset(string, 0, strlen(string));

                if (isspace(c)) {
                    if (c == '\n') {
                        row++;
                        col = 1;
                    }

                    state = STATE_START;
                } else if (c == EOF) {
                    ungetc(c, file);
                    state = STATE_END_OF_FILE;
                } else if (c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    string[i] = (char) c;
                    state = STATE_IDENTIFIER;
                } else if (c >= '0' && c <= '9') {
                    string[i] = (char) c;
                    state = STATE_INT;
                } else if (c == '(') {
                    string[i] = (char) c;
                    state = STATE_LEFT_PARENTHESES;
                } else if (c == ')') {
                    string[i] = (char) c;
                    state = STATE_RIGHT_PARENTHESES;
                } else if (c == '{') {
                    string[i] = (char) c;
                    state = STATE_LEFT_BRACE;
                } else if (c == '}') {
                    string[i] = (char) c;
                    state = STATE_RIGHT_BRACE;
                } else if (c == ';') {
                    string[i] = (char) c;
                    state = STATE_SEMICOLON;
                } else if (c == '=') {
                    string[i] = (char) c;
                    state = STATE_ASSIGNMENT;
                } else if (c == '"') {
                    state = STATE_STRING;
                } else if (c == '>') {
                    string[i] = (char) c;
                    state = STATE_GREATER;
                } else if (c == '<') {
                    string[i] = (char) c;
                    state = STATE_LESS;
                } else if (c == '*') {
                    string[i] = (char) c;
                    state = STATE_MULTIPLICATION;
                } else if (c == '/') {
                    string[i] = (char) c;
                    state = STATE_DIVISION;
                } else if (c == '+') {
                    string[i] = (char) c;
                    state = STATE_ADDITION;
                } else if (c == '-') {
                    string[i] = (char) c;
                    state = STATE_SUBTRACTION;
                } else if (c == '!') {
                    string[i] = (char) c;
                    state = STATE_LOGICAL_NOT;
                } else if (c == '\\') {
                    state = STATE_ESCAPE;
                } else if (c == ',') {
                    state = STATE_COMMA;
                    string[i] = (char) c;
                }
                break;
            case STATE_IDENTIFIER:
                if (c == '_' || (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                    i++;
                    col++;
                    string[i] = (char) c;
                    state = STATE_IDENTIFIER;
                } else if (is_keyword(string)) {
                    ungetc(c, file);
                    state = STATE_KEYWORD;
                } else {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = IDENTIFIER;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_INT:
                if (c >= '0' && c <= '9') {
                    i++;
                    col++;
                    string[i] = (char) c;
                    state = STATE_INT;
                } else if (c == '.') {
                    i++;
                    col++;
                    decimal_point = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE;
                } else if (c == 'e' || c == 'E') {
                    i++;
                    col++;
                    exponent = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE;
                } else {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = INT;
                    token->data = string;

                    ungetc(c ,file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_DOUBLE:
                if (c >= '0' && c <= '9') {
                    i++;
                    col++;
                    string[i] = (char) c;
                    state = STATE_DOUBLE;
                } else if (c == '.') {
                    if (decimal_point)
                        exit(LEXICAL_ERROR);

                    i++;
                    col++;
                    decimal_point = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE;
                } else if (c == 'e' || c == 'E') {
                    if (exponent)
                        exit(LEXICAL_ERROR);

                    i++;
                    col++;
                    exponent = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE_EXPONENT;
                } else {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = DOUBLE;
                    token->data = string;

                    ungetc(c ,file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_DOUBLE_EXPONENT:
                if (c == '+' || c == '-') {
                    if (sign)
                        exit(LEXICAL_ERROR);

                    i++;
                    col++;
                    sign = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE_EXPONENT;
                } else if (c >= '0' && c <= '9') {
                    i++;
                    col++;
                    exponent_number = true;
                    string[i] = (char) c;
                    state = STATE_DOUBLE_EXPONENT;
                } else {
                    if (!exponent_number)
                        exit(LEXICAL_ERROR);

                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = DOUBLE;
                    token->data = string;

                    ungetc(c ,file);
                    state = STATE_START;
                    return;
                }

                break;
            case STATE_KEYWORD:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = get_keyword_type (string);
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_LEFT_BRACE:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = LEFT_BRACE;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_RIGHT_BRACE:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = RIGHT_BRACE;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_LEFT_PARENTHESES:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = LEFT_PARENTHESES;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_RIGHT_PARENTHESES:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = RIGHT_PARENTHESES;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_SEMICOLON:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = SEMICOLON;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_ASSIGNMENT:
                if (c == '=') {
                    state = STATE_EQUAL;
                    ungetc(c, file);
                } else {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = ASSIGNMENT;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_EQUAL:
                i++;
                col++;
                string[i] = (char) c;

                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = EQUAL;
                token->data = string;

                state = STATE_START;
                return;
            case STATE_ONE_LINE_COMMENT:
                if (c == '\n') {
                    row++;
                    col = 1;
                    state = STATE_START;
                }
                break;
            case STATE_MULTIPLE_LINE_COMMENT:
                if (c == '*') {
                    state = STATE_MULTIPLE_LINE_COMMENT_PREEND;
                }
                break;
            case STATE_MULTIPLE_LINE_COMMENT_PREEND:
                if (c == '/') {
                    state = STATE_START;
                } else {
                    state = STATE_MULTIPLE_LINE_COMMENT;
                }
                break;
            case STATE_STRING:
                // Happens when there is unclosed string
                if (c == '\n')
                    exit(LEXICAL_ERROR);
                if (c == EOF)
                    exit(LEXICAL_ERROR);

                if (c == '"') {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = STRING;
                    token->data = string;

                    col++;
                    state = STATE_START;
                    return;
                } else if (c == '\\') {
                    state = STATE_STRING_ESCAPE;
                } else {
                    string[i] = (char) c;
                    i++;
                    col++;
                }
                break;
            case STATE_STRING_ESCAPE:
                if (c == '"') {
                    string[i] = '"';
                    i++;
                    col++;

                    state = STATE_STRING;
                }
                else if (c == 'n') {
                    string[i] = '\n';
                    i++;
                    col++;

                    state = STATE_STRING;
                } else if (c == 't') {
                    string[i] = '\t';
                    i++;
                    col++;

                    state = STATE_STRING;
                } else if (c == '\\') {
                    string[i] = '\\';
                    i++;
                    col++;

                    state = STATE_STRING;
                } else if (c == 'b') {
                    col++;
                    state = STATE_STRING_ESCAPE_BIN;
                } else if (c == '0') {
                    col++;
                    state = STATE_STRING_ESCAPE_OCT;
                } else if (c == 'x') {
                    col++;
                    state = STATE_STRING_ESCAPE_HEX;
                }
                break;
            case STATE_STRING_ESCAPE_BIN:
                if (c >= '0' && c <= '1') {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(BIN_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;

                    if (counter == BIN_LENGTH) {
                        string[i] = get_ascii_bin(tmp);
                        free(tmp);
                        tmp = NULL;
                        counter = 0;
                        i++;

                        state = STATE_STRING;
                    }
                } else {
                    exit(LEXICAL_ERROR);
                }
                break;
            case STATE_STRING_ESCAPE_OCT:
                if (c >= '0' && c <= '8') {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(OCT_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;

                    if (counter == OCT_LENGTH) {
                        string[i] = get_ascii_oct(tmp);
                        free(tmp);
                        tmp = NULL;
                        counter = 0;
                        i++;

                        state = STATE_STRING;
                    }
                } else {
                    exit(LEXICAL_ERROR);
                }
                break;
            case STATE_STRING_ESCAPE_HEX:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(HEX_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;

                    if (counter == HEX_LENGTH) {
                        string[i] = get_ascii_hex(tmp);
                        free(tmp);
                        tmp = NULL;
                        counter = 0;
                        i++;

                        state = STATE_STRING;
                    }
                } else {
                    exit(LEXICAL_ERROR);
                }
                break;
            case STATE_GREATER:
                if (c == '>') {
                    i++;
                    col++;
                    string[i] = (char) c;

                    state = STATE_ISTREAM;
                    break;
                } else if (c == '=') {
                    i++;
                    col++;
                    string[i] = (char) c;

                    state = STATE_GREATER_OR_EQUAL;
                }

                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = GREATER;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;

                return;
            case STATE_GREATER_OR_EQUAL:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = GREATER_OR_EQUAL;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;

                return;
            case STATE_ISTREAM:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = ISTREAM;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_LESS:
                if (c == '<') {
                    i++;
                    col++;
                    string[i] = (char) c;

                    state = STATE_OSTREAM;
                    break;
                } else if (c == '=') {
                    i++;
                    col++;
                    string[i] = (char) c;

                    state = STATE_LESS_OR_EQUAL;
                    break;
                }

                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = LESS;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;

                return;
            case STATE_LESS_OR_EQUAL:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = LESS_OR_EQUAL;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;

                return;
            case STATE_OSTREAM:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = OSTREAM;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_MULTIPLICATION:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = MULTIPLICATION;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_DIVISION:
                if (c == '/') {
                    state = STATE_ONE_LINE_COMMENT;
                } else if (c == '*') {
                    state = STATE_MULTIPLE_LINE_COMMENT;
                } else {
                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = DIVISION;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_ADDITION:
                if (c == '+') {
                    state = STATE_INCREMENT;
                    string[++i] = (char) c;
                    break;
                }
                
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = ADDITION;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_INCREMENT:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = INCREMENT;
                token->data = string;
                
                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_SUBTRACTION:
                if (c == '-') {
                    state = STATE_DECREMENT;
                    string[++i] = (char) c;
                    break;
                }
                
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = SUBTRACTION;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_DECREMENT:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = DECREMENT;
                token->data = string;
                
                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_END_OF_FILE:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = END_OF_FILE;
                token->data = NULL;

                return;
            case STATE_LOGICAL_NOT:
                if (c == '=') {
                    i++;
                    col++;
                    string[i] = (char) c;
                    state = STATE_NOT_EQUAL;
                    break;
                }

                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = LOGICAL_NOT;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_NOT_EQUAL:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = NOT_EQUAL;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            case STATE_ESCAPE:
                if (c == 'b') {
                    col++;
                    state = STATE_ESCAPE_BIN;
                } else if (c == '0') {
                    col++;
                    state = STATE_ESCAPE_OCT;
                } else if (c == 'x') {
                    col++;
                    state = STATE_ESCAPE_HEX;
                } else {
                    exit(LEXICAL_ERROR);
                }
                break;
            case STATE_ESCAPE_BIN:
                if (c >= '0' && c <= '1') {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(BIN_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;
                    
                    if (counter > BIN_LENGTH)
                        error (INTERNAL_ERROR);
                } else {
                    get_value_bin(tmp, &string[i]);
                    free(tmp);

                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = INT;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_ESCAPE_OCT:
                if (c >= '0' && c <= '8') {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(OCT_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;
                    
                    if (counter > OCT_LENGTH)
                        error (INTERNAL_ERROR);
                } else {
                    get_value_oct(tmp, &string[i]);
                    free(tmp);

                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = INT;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_ESCAPE_HEX:
                if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')) {
                    if (tmp == NULL) {
                        tmp = (char *)malloc(HEX_LENGTH);
                    }

                    tmp[counter] = (char) c;
                    col++;
                    counter++;
                    
                    if (counter > HEX_LENGTH)
                        error (INTERNAL_ERROR);
                } else {
                    get_value_hex(tmp, &string[i]);
                    free(tmp);

                    token->row = row;
                    token->col[COL_START] = col - (int) strlen(string);
                    token->col[COL_END] = col;
                    token->type = INT;
                    token->data = string;

                    ungetc(c, file);
                    state = STATE_START;
                    return;
                }
                break;
            case STATE_COMMA:
                token->row = row;
                token->col[COL_START] = col - (int) strlen(string);
                token->col[COL_END] = col;
                token->type = COMMA;
                token->data = string;

                ungetc(c, file);
                state = STATE_START;
                return;
            default:
                break;
        }
    }
}

void destroy_token(Token *token) {
    free(token->data);
    free(token);
}

bool is_keyword(char *string) {
    int i, size;

    size = sizeof(KEYWORDS) / sizeof(char *);
    for (i = 0; i < size; i++) {
        if (strcmp(string, KEYWORDS[i]) == 0) {
            return true;
        }
    }

    return false;
}

enum TOKEN get_keyword_type(char *string) {
    if (!strcmp(string, "auto"))
        return KEYWORD_AUTO;
    if (!strcmp(string, "bool"))
        return KEYWORD_BOOL;
    if (!strcmp(string, "cin"))
        return KEYWORD_CIN;
    if (!strcmp(string, "cout"))
        return KEYWORD_COUT;
    if (!strcmp(string, "do"))
        return KEYWORD_DO;
    if (!strcmp(string, "double"))
        return KEYWORD_DOUBLE;
    if (!strcmp(string, "else"))
        return KEYWORD_ELSE;
    if (!strcmp(string, "false"))
        return KEYWORD_FALSE;
    if (!strcmp(string, "for"))
        return KEYWORD_FOR;
    if (!strcmp(string, "if"))
        return KEYWORD_IF;
    if (!strcmp(string, "int"))
        return KEYWORD_INT;
    if (!strcmp(string, "return"))
        return KEYWORD_RETURN;
    if (!strcmp(string, "string"))
        return KEYWORD_STRING;
    if (!strcmp(string, "true"))
        return KEYWORD_TRUE;
    if (!strcmp(string, "while"))
        return KEYWORD_WHILE;

    if (!strcmp(string, "concat"))
        return FUNCTION_CONCAT;
    if (!strcmp(string, "find"))
        return FUNCTION_FIND;
    if (!strcmp(string, "length"))
        return FUNCTION_LENGTH;
    if (!strcmp(string, "sort"))
        return FUNCTION_SORT;
    if (!strcmp(string, "substr"))
        return FUNCTION_SUBSTR;

    return KEYWORD;
}

struct stack *scanner (char *filename) {
    int result;
    FILE *file;
    Token *token;
    struct stack *stack;

    file = fopen(filename, "r");
    if (file == NULL)
        error(INTERNAL_ERROR);

    stack = stack_init ();
    while ((token = _get_token(file))->type != END_OF_FILE) {
        stack_push (stack, token);
#ifdef _DEBUG
        printf ("TOKEN: %d - %s\n", token->type, token->data);
#endif
    }
    stack_push (stack, token);
    stack_reverse (&(stack->top));

    result = fclose(file);
    if (result == EOF)
        error(INTERNAL_ERROR);

    return stack;
}

Token *get_token (struct stack *stack) {
    Token *token = (Token *)stack->top->data;
    stack_pop (stack);

    return token;
}