#include "helper.h"

#define BIN         2
#define OCT         8
#define HEX         16
#define MAX_VALUE   255

void error(int error_id) {
    char *error_message;

    switch (error_id) {
        case LEXICAL_ERROR:
            error_message = "Lexcial error";    //1
            break;
        case SYNTAX_ERROR:
            error_message = "Syntax error";     //2
            break;
        case SEMANTIC_UNDEFINED:
            error_message = "Semantic error - undefined var/fx or redef attempt";     //3
            break;
        case SEMANTIC_EXPRESSION:
            error_message = "Semantic error - expression";     //4
            break;
        case READ_ERROR:
            error_message = "Read error";       //7
            break;
        case UNINITIALIZED_VARIABLE:
            error_message = "Uninitialized variable";   //8
            break;
        case DIVISION_BY_ZERO:
            error_message = "Division by zero"; //9
            break;
        case INTERNAL_ERROR:
            error_message = "Internal error";   //99
            break;
        default:
#ifdef _DEBUG
//             sprintf(error_message,"%i", error_id);
#endif
            break;
    }

    fprintf (stderr, "ERROR: %s\n", error_message);

#ifdef _WIN32
    system ("pause");
#endif
    exit (error_id);
}

void warning(const char* message) {
    fprintf(stderr, "WARNING: %s\n", message);
}

int get_int(Token token) {
    if (token.type != INT) {
        warning("Trying to get int from non-int token.");
    }

    return atoi(token.data);
}

double get_double(Token token) {
    if (token.type != DOUBLE) {
        warning("Trying to get double from non-double token.");
    }

    return atof(token.data);
}

unsigned char get_ascii_bin(const char *bin_code) {
    return (unsigned char) strtol(bin_code, NULL, BIN);
}

unsigned char get_ascii_oct(const char *oct_code) {
    return (unsigned char) strtol(oct_code, NULL, OCT);
}

unsigned char get_ascii_hex(const char *hex_code) {
    return (unsigned char) strtol(hex_code, NULL, HEX);
}

void get_value_bin(const char *bin_code, char *string) {
    sprintf(string, "%d", (int) strtol(bin_code, NULL, BIN));
}

void get_value_oct(const char *oct_code, char *string) {
    long int value = strtol(oct_code, NULL, OCT);
    if (value > MAX_VALUE)
        error(INTERNAL_ERROR);
    
    sprintf(string, "%d", (int) value);
}

void get_value_hex(const char *hex_code, char *string) {
    sprintf(string, "%d", (int) strtol(hex_code, NULL, HEX));
}