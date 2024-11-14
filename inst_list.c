#include "inst_list.h"

int isEmpty(I_ListOfVariables *ListOfVariables)
{
	if(ListOfVariables->first == NULL)
		return 1;
	else
		return 0;
}

void addToEnd(I_ListOfVariables *ListOfVariables, I_item *variable)
{
	I_variable *tmp;
	if((tmp = (I_variable *)malloc(sizeof(I_variable))) == NULL)
		error(INTERNAL_ERROR);
		
	if(isEmpty(ListOfVariables))
	{
		ListOfVariables->first = tmp;
		ListOfVariables->last = tmp;
		ListOfVariables->last->value_type = variable->value_type;
		ListOfVariables->last->identificator = variable->value.s;
		ListOfVariables->last->inicialized = 0;
		ListOfVariables->last->next = NULL;
	}
	else
	{
		ListOfVariables->last->next = tmp;
		ListOfVariables->last->next->value_type = variable->value_type;
		ListOfVariables->last->next->identificator = variable->value.s;
		ListOfVariables->last->next->inicialized = 0;
		ListOfVariables->last->next->next = NULL;
		ListOfVariables->last = tmp;
	}
}

I_variable * findVariable(I_ListOfVariables *ListOfVariables, char *name)
{
	I_variable *found;
	I_variable *tmp;
	found = NULL;
	tmp = ListOfVariables->first;
	
	if(isEmpty(ListOfVariables))
	{
		error(UNINITIALIZED_VARIABLE);
	}
		
	while(tmp != NULL)
	{
		if(strcmp(tmp->identificator, name) == 0)
			found = tmp;
		tmp = tmp->next;
	}
	
	if(found == NULL)
		error(UNINITIALIZED_VARIABLE);

	return found;
}

void delVariable(I_ListOfVariables *ListOfVariables, char *name)
{
    int delFirst = 0;
	I_variable *found;
	I_variable *tmp;
	found = NULL;
	tmp = ListOfVariables->first;
	
	if(isEmpty(ListOfVariables))
	{
		error(UNINITIALIZED_VARIABLE);
	}
		
	if(strcmp(ListOfVariables->first->identificator, name) == 0)
		delFirst = 1;
		
	while(tmp->next != NULL)
	{
		if(strcmp(tmp->next->identificator, name) == 0)
		{
			found = tmp;
			delFirst = 0;
		}
		tmp = tmp->next;
	}
	
	if(delFirst)
	{
		tmp = ListOfVariables->first;
		ListOfVariables->first = ListOfVariables->first->next;
	}
	else
	{
		if(found == NULL)
			error(UNINITIALIZED_VARIABLE);
		else
		{
			if(found->next->next == NULL)
				ListOfVariables->last = found;
			tmp = found->next;
			found->next = found->next->next;
		}
	}
	free(tmp);
}

void add_instruction (inst_list inst, I_item *op1, I_item *op2, I_item *op3) {
    I_instruction *tmp_inst, *skip;

    tmp_inst = (I_instruction *)malloc (sizeof (I_instruction));
    tmp_inst->inst_type = inst;
    tmp_inst->next = NULL;
    
    if (op1 != NULL) {
        tmp_inst->op1.value_type = op1->value_type;
        tmp_inst->op1.value = op1->value;
    }
    if (op2 != NULL) {
        tmp_inst->op2.value_type = op2->value_type;
        tmp_inst->op2.value = op2->value;
    }
    if (op3 != NULL) {
        tmp_inst->op3.value_type = op3->value_type;
        tmp_inst->op3.value = op3->value;
    }

    if (instruction == NULL) {
        instruction = tmp_inst;
        instruction->next = NULL;
    } else if (instruction->next == NULL) {
        instruction->next = tmp_inst;
    } else {
        skip = instruction;
        while ((skip = skip->next)->next != NULL);
        skip->next = tmp_inst;
    }
}

I_item *set_item (type_list type, void *value) {
    I_item *item = (I_item *)malloc (sizeof (I_item));

    switch (type) {
        case TI_STRING:
            item->value_type = TI_STRING;
            item->value.s = (char *)malloc (strlen ((char *)value) + 1);
            if (item->value.s == NULL)
                error (INTERNAL_ERROR);
            strcpy (item->value.s, (char *)value);
            break;
        default:
            break;
    }

    return item;
}

int isEmptyPointers(I_ListOfPointers *ListOfPointers)
{
	if(ListOfPointers->first == NULL)
		return 1;
	else
		return 0;
}

void addToEndPointers(I_ListOfPointers *ListOfPointers, I_instruction *pointer)
{
	I_pointer *tmp;
	if((tmp = (I_pointer *)malloc(sizeof(I_pointer))) == NULL)
		error(INTERNAL_ERROR);
		
	if(isEmptyPointers(ListOfPointers))
	{
		ListOfPointers->first = tmp;
		ListOfPointers->last = tmp;
		ListOfPointers->first->pointer = pointer;
		ListOfPointers->first->next = NULL;
	}
	else
	{
		ListOfPointers->last->next = tmp;
		ListOfPointers->last->next->pointer = pointer;
		ListOfPointers->last->next->next = NULL;
		ListOfPointers->last = tmp;
	}
	
}

I_instruction * popLastPointer(I_ListOfPointers *ListOfPointers)
{
	I_pointer *tmp = ListOfPointers->first;
	I_instruction *result;
	
	if(isEmptyPointers(ListOfPointers))
		error(INTERNAL_ERROR);
		
	if(ListOfPointers->first == ListOfPointers->last)
	{
		result = ListOfPointers->first->pointer;
		free(ListOfPointers->first);
		ListOfPointers->first = ListOfPointers->last = NULL;
	}
	
	while(tmp->next->next != NULL)
	{
		tmp = tmp->next;
	}
	result = tmp->next->pointer;
	free(tmp->next);
	
	ListOfPointers->last = tmp;
	return result;
}

