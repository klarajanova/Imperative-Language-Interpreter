#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "helper.h"

#ifdef _WIN32
	#define true 1
	#define false 0
	typedef char bool;
#else
	#include <stdbool.h>
#endif

char *sort(char *string);
int length(char *string);
char *substr(char *string, int i, int n); 
char *concat(char *s1, char *s2);
int find(char *string, char *search);