#include "ial.h"

void
swap(char *a, char *b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void
sift_down(char *string,
          int start,
          int end)
{
    int child = 0;
    int root = start;
    
    while ((child = root * 2 + 1) <= end)
    {
        int tmp = root;
        if (string[tmp] < string[child])
            tmp = child;
        if (child + 1 <= end && string[tmp] < string[child + 1])
            tmp = child + 1;
        if (tmp == root)
        {
            return;
        }
        else
        {
            swap(&string[root], &string[tmp]);
            root = tmp;
        }
    }
}

void
heapify(char *string,
        int count)
{
    int start = (int)floor((count - 2) / 2);

    while (start >= 0)
    {
        sift_down(string, start, count);
        start -= 1;
    }
}

char *
sort(char *string)        /* string to sort */
{
    int count = (int)strlen(string);
    int end = count - 1;
    
    heapify(string, count);
    while (end > 0)
    {
        swap(&string[end], &string[0]);
        end -= 1;
        sift_down(string, 0, end);
    }

    return (string);
}

int
length(char *string)
{
    return ((int)strlen(string));
}

char *
substr(char *string,    /* searched string */
       int i,           /* start of wanted string */
       int n)           /* length of wanted string */
{
    char *new_string = malloc((size_t)(n + 1));
    if (new_string == NULL)
        error (INTERNAL_ERROR);
    strncpy(new_string, string + i, (size_t)n);

    return (new_string);
}

char *
concat(char *s1,  /* first part of new string */
       char *s2)  /* second part of new string */
{
    int count_s1 = (int)strlen(s1);
    int count_s2 = (int)strlen(s2);
    char *string = malloc((size_t)(count_s1 + count_s2 + 1));
    if (string == NULL)
        error (INTERNAL_ERROR);
    strncpy(string, s1, (size_t)count_s1);
    strncpy(string + count_s1, s2, (size_t)count_s2);
    string[count_s1 + count_s2 + 1] = '\0';
    
    return string;
}

int
bad_rule(char *search,
         int count,
         int miss)
{
    int i;
    do
    {
        count--;
        i = search[count];
    }
    while(i != miss);

    return count;
}

int
find(char *string,
     char *search)
{
    int i, j, tmp;
    int count_search = (int)strlen(search);
    int x = count_search;
    int y = count_search;
    do
    {
        do
        {
            x--;
            y--;
            i = string[x];
            j = search[y];
        }
        while (i == j);
        if (y < 0)
            return (x + 1);
        tmp = bad_rule(search, y, i);
        x = count_search + x - tmp;
        y = count_search;
    }
    while (true);
}

// TODO: remove this main
/*int
main(int argc,
     char **argv)
{
    char test[] = "LOOXOOL\n";

    // simple test for function SORT
    printf("=> %s\n", sort(test));                      // \nLLOOOOX

    // simple test for funcion LENGTH
    printf("=> %d\n", length(test));                    // 5

    // simple test for function SUBSTR
    printf("=> %s\n", substr(test, 3, 4));              // OOOO

    // simple test for function CONCAT
    printf("=> %s\n", concat("HELLO", "WORLD"));        // HELLOWORLD

    // simple test for function FIND
    strcpy(test, "maistuko kaima maisemaomaloma");
    printf("=> %d\n", find(test, "maisemaomaloma"));    // 15
    
    return (EXIT_SUCCESS);
}*/
