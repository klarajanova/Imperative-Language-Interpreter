#ifndef IFJ15_HELPER_H
#define IFJ15_HELPER_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "token.h"

void error(int error_id);
void warning(const char* message);
int get_int(Token token);
double get_double(Token token);
unsigned char get_ascii_bin(const char *bin_code);
unsigned char get_ascii_oct(const char *oct_code);
unsigned char get_ascii_hex(const char *hex_code);
void get_value_bin(const char *bin_code, char *string);
void get_value_oct(const char *oct_code, char *string);
void get_value_hex(const char *hex_code, char *string);

#endif //IFJ15_HELPER_H
