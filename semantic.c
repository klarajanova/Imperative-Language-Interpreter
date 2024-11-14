#include "semantic.h"

struct fx_tab fx_table;                 //Globalni tabulka symbolu - pro funkce
struct fx_tab_itm* last_declared_fx;    //Poslední deklarovaná funkce - kvuli generovani 3AC, pristupu ke stacku teto funkce atd...
struct fx_tab_itm* last_fx_args_add;    //Funkce, do ktere se prave pridavaji argumenty. 
int args_adding;                        //1 pokud argumenty pridavame, 2 pokud testujeme oproti jiz drive definovanemu zaznamu
int args_argc;                          //momentalne ocekavany argument pri testovani argumentu...

//0 kdyz nenalezeno, -1 kdyz nalezeno, jinak chyba dle cisla chyb.
//Zaroven vraci do druheho parametru pointer na nalezeny objekt
int seek_fx(char* identificator, struct fx_tab_itm** ret_itm)
{
    int hash = generate_hash(identificator);
    struct fx_tab_itm* item = fx_table.tab[hash];

// #ifdef _DEBUG
//     fprintf(stderr, "SEMANTIC: seek FX: %s\n", identificator);
// #endif

    while(item != NULL)
    {
        if(strcmp(item->identificator, identificator) == 0)
        {
            *ret_itm = item;
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: seek FX - found\n");
#endif
            return -1;
        }
        else
        {
            item = item->next;
        }
    }

#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: seek FX - not found\n");
#endif
    return 0;
}

//Vola se, kdyz je nalezea pouze deklarace funkce. 
//Vraci nulu, kdyz OK, cislo chyby, kdyz FAIL.
int declare_fx(char* identificator, int ret_type)
{
    struct fx_tab_itm* found_item;
    int seek_result = seek_fx(identificator, &found_item);
    int hash;
    struct fx_tab_itm* item, *new;

// #ifdef _DEBUG
//     fprintf(stderr, "SEMANTIC: declare_fx() init\n");
// #endif
    
    if(seek_result >0)
        return seek_result;
    
    if(seek_result == 0)        //nenalezeno - pridavame FX do tab.symb.
    {
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: declaring FX: %s\n", identificator);
#endif
        args_adding = 1;
        //Muzeme pridavat
        hash = generate_hash(identificator);
        item = fx_table.tab[hash];
        new = (struct fx_tab_itm*)malloc(sizeof(struct fx_tab_itm)); 
        if (new == NULL)
            error (INTERNAL_ERROR);
        
        new->rettype = ret_type;
        new->identificator = identificator;
        new->var_stack = NULL;
        new->arguments = NULL;          //TODO udelat argumenty funkce
        new->argc = 0;
        new->defined = FX_DECLARED;
        new->next = NULL;
        
        last_fx_args_add = new;
        last_declared_fx = new;
        
        if(item == NULL)
            fx_table.tab[hash] = new;
        else
        {
            while(item->next != NULL)
                item = item->next;
            item->next = new;
        }
        return 0;
    }
    else
    {   //nepridavame, testujeme
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: already declared FX, testing: %s\n", identificator);
#endif
        
        if(found_item->rettype != ret_type)
        {
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: already declared FX, diferent ret_type!\n");
#endif
            error (SEMANTIC_UNDEFINED);
        }
        else
        {    
            last_declared_fx = found_item;
            last_fx_args_add = found_item;
            args_adding = 2;
            args_argc = 1;
        }
    }
    return 0;
}

//Ukonci moznost pridavat argumenty do promenne/testovat
void end_declare_fx()
{
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: end_declare_fx(): %s (%s)\n", last_fx_args_add->identificator, last_declared_fx->identificator);
#endif
    last_fx_args_add = NULL;
    args_adding = 0;
}

//Vraci 0 kdyz OK, cislo chyby, kdyz ERROR
int define_fx()
{

#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: define_fx(): %s\n", last_declared_fx->identificator);
#endif
    
    if(last_declared_fx->defined == FX_DECLARED)
        last_declared_fx->defined = FX_DEFINED;
    else
        error(SEMANTIC_UNDEFINED);
    
    last_declared_fx->var_stack = stack_init();

    
    return 0;
}

int add_fx_arg(int arg_datatype, char* arg_identificator)       //TODO osetrit tu vsechny necekane stavy a chyby + DODELAT!!!
{
    struct fx_arg *arg, *old;
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: add_fx_arg(): FX: %s; ARG: %i: %s\n", last_declared_fx->identificator, arg_datatype, arg_identificator);
#endif
    
    if(args_adding==1)
    {
        last_declared_fx->argc++;
        //Pokud pridavame argumenty
        if(last_declared_fx->argc == 1)
        {
    #ifdef _DEBUG
            fprintf(stderr, "\tSEMANTIC: argc: %i (ifVetev)\n", last_declared_fx->argc);
    #endif
            arg = (struct fx_arg*) calloc(1, sizeof(struct fx_arg));
            arg->type = arg_datatype;
            arg->identificator = arg_identificator;
            
            last_declared_fx->arguments = arg;
            
    #ifdef _DEBUG
            fprintf(stderr, "\tSEMANTIC: argument (from struct): %i: %s (ifVetev)\n", arg->type, arg->identificator);
    #endif

        }
        else
        {
            arg = (struct fx_arg*)calloc(last_declared_fx->argc, sizeof(struct fx_arg));
            old = last_declared_fx->arguments;

    #ifdef _DEBUG
            fprintf(stderr, "\tSEMANTIC: argc: %i (elseVetev)\n", last_declared_fx->argc);
    #endif
            
            *arg = *old;
            arg[last_declared_fx->argc-1].type = arg_datatype;
            arg[last_declared_fx->argc-1].identificator = arg_identificator;
            free(old);
            
            last_declared_fx->arguments = arg;

            
#ifdef _DEBUG
            fprintf(stderr, "\tSEMANTIC: argument (from struct): %i: %s (elseVetev)\n", arg[last_declared_fx->argc-1].type, arg[last_declared_fx->argc-1].identificator);
#endif
        }
    }
    else
    {
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: arg_test: %d of %d\n", args_argc, last_declared_fx->argc);
#endif
        
        //Testujeme argumenty oproti drive deklarovane/definovane funkci
        if(args_argc > last_declared_fx->argc)
        {
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: arg_test: too many arguments!\n");
#endif
            error(SEMANTIC_UNDEFINED);
        }
        
        arg = last_declared_fx->arguments;
        
        if(arg[args_argc-1].type != arg_datatype)
        {
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: arg_test: incorrect type!\n");
#endif
            error(SEMANTIC_UNDEFINED);
        }
        
        if(strcmp(arg_identificator, arg[args_argc-1].identificator) != 0)
        {
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: arg_test: diferent identificators!\n");
#endif
            error(SEMANTIC_UNDEFINED);
        }
        args_argc++;
    }
    
    
    
    return 0;
}



//0 kdyz nenalezeno, -1 kdyz nalezeno, jinak chyba dle cisla chyb. 
int seek_var(char* identificator, struct var_tab_itm** ret_itm)
{
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: seek VAR: %s\n", identificator);
#endif
    
    int hash = generate_hash(identificator);
    struct stack_item *stack_itm = last_declared_fx->var_stack->top;
    struct var_tab *tbl = (struct var_tab*)stack_itm->data;
    struct var_tab_itm* item = tbl->tab[hash];

    while(item != NULL)
    {
        if(strcmp(item->identificator, identificator) == 0)
        {
            *ret_itm = item;
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: seek VAR - found\n");
#endif
            return -1;
        }
        else
        {
            item = item->next;
        }
    }

#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: seek VAR - not found\n");
#endif
    return 0;
}

//returns 0 if not found, -1 if found, >0 if error (then returns error number)
int seek_var_all_stack(char* identificator, struct var_tab_itm** ret_itm)
{
    struct var_tab_itm* found_itm;      //here is pointer to found variable
    struct stack_item *stack_itm = last_declared_fx->var_stack->top;    //Here is pointer to stack item where seek starts
    struct var_tab *tbl = (struct var_tab*)stack_itm->data;             //Stack item dereferenced to var_tab (stack_itm->next poinst to next tab)
    struct var_tab_itm* item;
    
    int hash = generate_hash(identificator);
    int seek_result = seek_var(identificator, &found_itm);
    
    if(seek_result > 0)
        return seek_result;
    
    if(seek_result <0)
    {
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: seek VAR all stack - found in TOP table\n");
#endif
        *ret_itm = found_itm;
        return -1;
    }
    
    while(stack_itm->next != NULL)
    {
        stack_itm = stack_itm->next;
        tbl = (struct var_tab*)stack_itm->data;
        item = tbl->tab[hash];
        
        while(item != NULL)
        {
            if(strcmp(item->identificator, identificator) == 0)
            {
                *ret_itm = item;
#ifdef _DEBUG
                fprintf(stderr, "SEMANTIC: seek VAR all stack - found\n");
#endif
                return -1;
            }
            else
            {
                item = item->next;
            }
        }
    }
    
    return 0;
    
}

int add_var(int datatype, char* identificator)
{
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: add_var(%d, %s) init\n", datatype, identificator);
#endif
    //odkladaci promenne pro seek funkce
//     struct fx_tab_itm* found_fx;
    struct var_tab_itm* found_itm;
    
    struct stack_item *stack_itm = last_declared_fx->var_stack->top;
    struct var_tab *tbl = (struct var_tab*)stack_itm->data;
    struct var_tab_itm* item;
    struct var_tab_itm* new;
    
    int hash;
    int seek_result = seek_var(identificator, &found_itm);
    
    if(seek_result >0)
        return seek_result;
    
    if(seek_result == 0)        //not found in this scope
    {
        seek_result = seek_fx(identificator, NULL);     //Pokud by zde neslo NULL, vlozit &found_fx a odkomentovat deklaraci teto promenne
        if(seek_result >0)
            return seek_result;
        
        if(seek_result != 0)        //same name fx found
        {
#ifdef _DEBUG
            fprintf(stderr, "SEMANTIC: add_var: found FX with same name (%s)\n", identificator);
#endif
            error (SEMANTIC_UNDEFINED);
        }
        
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: adding VAR: %s\n", identificator);
#endif
        //Muzeme pridavat
        hash = generate_hash(identificator);
        item = tbl->tab[hash];
        
        new = (struct var_tab_itm*)malloc(sizeof(struct var_tab_itm));
        if (new == NULL)
            error (INTERNAL_ERROR);
        
        new->type = datatype;
        new->identificator = identificator;
        new->next = NULL;
        
        if(item == NULL)
            tbl->tab[hash] = new;
        else
        {
            while(item->next != NULL)
                item = item->next;
            item->next = new;
        }
        return 0;
    }
    else
    {
#ifdef _DEBUG
        fprintf(stderr, "SEMANTIC: var already exists in this scope (%s)\n", identificator);
#endif

        // redefinition error - if var of the same name exists in this scope
        if(found_itm->type != datatype)
            error(SEMANTIC_UNDEFINED);
        
    }
    return 0;
}

//vlozi na stack tabulku symbolu
int var_tab_body_push()
{
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: var_tab_body_push()\n");
#endif
    struct var_tab* v_table;

    v_table = var_tab_init();
    stack_push(last_declared_fx->var_stack, (void*)v_table);
    return 0;
}

int var_tab_body_pop()
{
#ifdef _DEBUG
    fprintf(stderr, "SEMANTIC: var_tab_body_pop()\n");
    print_var_tab();
#endif
    stack_pop(last_declared_fx->var_stack);
    return 0;
}

//returns type of variable or 0 if not found
int var_by_type(char *identificator)
{
    struct var_tab_itm* found_itm;

    int seek_result = seek_var_all_stack(identificator, &found_itm);
    
    if(seek_result > 0)
        return seek_result;
    
    if(seek_result==0)
        return 0;
    
    return found_itm->type;
}


//vygeneruje hodnotu, která slouží jako HASH do tabulky symbolů
int generate_hash(char* string)
{
    int len = strlen(string);
    int sum = 0;
    int idx = 0;
    int i;
    
    for(i=0; i<len;i++)
    {
        sum=sum+(int)string[i];
    }
    
    while(sum>0)
    {
        idx = sum%10;
        sum = sum/10;
    }
    
    while(idx>=10)
        idx=idx%10;
    
    return idx;
}

//INIT funkce vracející pointer na nově inicializovanou tabulku symbolů.
struct var_tab* var_tab_init () 
{
    int i;

    struct var_tab* table = (struct var_tab *)malloc (sizeof (struct var_tab));
    if (table == NULL)
        error (INTERNAL_ERROR);

    for(i=0; i<10; i++)
        table->tab[i] = NULL;

    return table;
}


void print_var_tab() //TODO tuto funkci ostranit
{
    int i;
    
    struct stack_item *stack_itm = last_declared_fx->var_stack->top;
    struct var_tab *tbl = (struct var_tab*)stack_itm->data;
    struct var_tab_itm* item;
#ifdef _DEBUG
    printf("###### VAR TAB START #####\n");
    for(i=0; i<10; i++)
    {
        printf("%d: ", i);
        item = tbl->tab[i];
        if(item == NULL)
            printf(" ");
        else
        {
            do{
                printf("%s (%d), ", item->identificator, item->type);
                item = item->next;
            }while(item != NULL);
        }
        printf("\n");
    }
    printf("####### VAR TAB END ######\n");
#endif
}

void print_last_fx_args()
{
    int i;
    struct fx_arg *arg;
    arg = last_declared_fx->arguments;
#ifdef _DEBUG
    printf("####### LAST FX ARGS ######\n");
    printf("argc: %d\n",last_declared_fx->argc);
    for(i=0; i<last_declared_fx->argc; i++)
    {   
        printf("%i: %s\n", arg[i].type, arg[i].identificator);
    }
    
    printf("###### END LAST ARGS ######\n\n");
#endif
}


void print_fx_tab()
{
    int i;
    struct fx_tab_itm* item;
#ifdef _DEBUG
    printf("###### FX TAB START #####\n");
    for(i=0; i<10; i++)
    {
        printf("%d: ", i);
        item = fx_table.tab[i];
        if(item == NULL)
            printf(" ");
        else
        {
            do{
                printf("%s, ", item->identificator);
                item = item->next;
            }while(item != NULL);
        }
        printf("\n");
    }
    printf("####### FX TAB END ######\n");
#endif
}