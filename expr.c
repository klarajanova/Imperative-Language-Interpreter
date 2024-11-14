#include "expr.h"

extern struct stack *stack_token;

#define TABLE_SIZE 16
#define RULE_SIZE 3
#define NO_RULE -1

enum pt_operators {
    PT_NONE,
    PT_LESS,
    PT_EQUAL,
    PT_GREATER
};

enum pt_items {
    ITEM_ADDITION,              // 0
    ITEM_SUBTRACTION,           // 1
    ITEM_MULTIPLICATION,        // 2
    ITEM_DIVISION,              // 3
    ITEM_LESS,                  // 4
    ITEM_GREATER,               // 5
    ITEM_LESS_OR_EQUAL,         // 6
    ITEM_GREATER_OR_EQUAL,      // 7
    ITEM_EQUAL,                 // 8
    ITEM_NOT_EQUAL,             // 9
    ITEM_LEFT_PARENTHESES,      // 10
    ITEM_RIGHT_PARENTHESES,     // 11
    ITEM_IDENTIFIER,            // 12
    ITEM_CONSTANT,              // 13
    ITEM_DOLLAR,                // 14
    ITEM_E,                     // 15
    ITEM_STOP                   // 16
};

const char precedence_table[TABLE_SIZE][TABLE_SIZE] = {
    //   +              -           *           /           <            >          <=           >=          ==         !=          (           )        const         id           $
    {PT_GREATER,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // +
    {PT_GREATER,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // -
    {PT_GREATER,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // *
    {PT_GREATER,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // /
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // <
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // >
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // <=
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // >=
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // ==
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_GREATER},    // !=
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_EQUAL,   PT_LESS,    PT_LESS,    PT_NONE},       // (
    {PT_GREATER,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_NONE,    PT_GREATER, PT_NONE,    PT_NONE,    PT_GREATER},    // )
    {PT_GREATER,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_NONE,    PT_NONE,    PT_GREATER},    // id
    {PT_GREATER,    PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_GREATER, PT_LESS,    PT_GREATER, PT_NONE,    PT_NONE,    PT_GREATER},    // const
    {PT_LESS,       PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_LESS,    PT_GREATER, PT_LESS,    PT_LESS,    PT_NONE}        // $
};

const char rule[][RULE_SIZE] = {
    {ITEM_E, ITEM_ADDITION, ITEM_E},                            // E -> E + E
    {ITEM_E, ITEM_SUBTRACTION, ITEM_E},                         // E -> E - E
    {ITEM_E, ITEM_MULTIPLICATION, ITEM_E},                      // E -> E * E
    {ITEM_E, ITEM_DIVISION, ITEM_E},                            // E -> E / E
    {ITEM_E, ITEM_LESS, ITEM_E},                                // E -> E < E
    {ITEM_E, ITEM_GREATER, ITEM_E},                             // E -> E > E
    {ITEM_E, ITEM_LESS_OR_EQUAL, ITEM_E},                       // E -> E <= E
    {ITEM_E, ITEM_GREATER_OR_EQUAL, ITEM_E},                    // E -> E >= E
    {ITEM_E, ITEM_EQUAL, ITEM_E},                               // E -> E == E
    {ITEM_E, ITEM_NOT_EQUAL, ITEM_E},                           // E -> E != E
    {ITEM_RIGHT_PARENTHESES, ITEM_E, ITEM_LEFT_PARENTHESES},    // E -> (E)
    {ITEM_IDENTIFIER, 0, 0},                                    // E -> i
    {ITEM_CONSTANT, 0, 0}                                       // E -> const
};

Token *set_term (Token *token) {
    switch (token->type) {
        case ADDITION:
            token->term = ITEM_ADDITION;
            break;
        case SUBTRACTION:
            token->term = ITEM_SUBTRACTION;
            break;
        case MULTIPLICATION:
            token->term = ITEM_MULTIPLICATION;
            break;
        case DIVISION:
            token->term = ITEM_DIVISION;
            break;
        case LESS:
            token->term = ITEM_LESS;
            break;
        case GREATER:
            token->term = ITEM_GREATER;
            break;
        case LESS_OR_EQUAL:
            token->term = ITEM_LESS_OR_EQUAL;
            break;
        case GREATER_OR_EQUAL:
            token->term = ITEM_GREATER_OR_EQUAL;
            break;
        case EQUAL:
            token->term = ITEM_EQUAL;
            break;
        case NOT_EQUAL:
            token->term = ITEM_NOT_EQUAL;
            break;
        case LEFT_PARENTHESES:
            token->term = ITEM_LEFT_PARENTHESES;
            break;
        case RIGHT_PARENTHESES:
            token->term = ITEM_RIGHT_PARENTHESES;
            break;
        case IDENTIFIER:
            token->term = ITEM_IDENTIFIER;
            break;
        case INT:
            token->term = ITEM_CONSTANT;
            break;
        default:
            token->term = ITEM_DOLLAR;
            break;
    }

    return token;
}

int check_rule (struct stack *stack) {
    int i, j, rule_size, found;
    Token *token;
    struct stack_item *item;

    rule_size = sizeof(rule)/sizeof(rule[0]);

    // checks 3 sized rules
    for (i = 0; i < rule_size; i++) {
        found = TRUE;
        item = stack->top;

        for (j = 0; j < RULE_SIZE; j++) {
            token = (Token *)item->data;

            if (token->term != rule[i][j]) {
                found = FALSE;
                break;
            }

            item = item->next;
        }

        if (found)
            return i;
    }

    // checks 1 sized rules
    for (i -= 2; i < rule_size; i++) {
        found = TRUE;
        item = stack->top;

        for (j = 0; j < RULE_SIZE - 2; j++) {
            token = (Token *)item->data;

            if (token->term != rule[i][j]) {
                found = FALSE;
                break;
            }

            item = item->next;
        }

        if (found)
            return i;
    }

    return NO_RULE;
}

void precedence () {
    int end, next;
    Token *token = NULL;
	Token *top_token = NULL;
	Token *end_token = NULL;
	Token *push_token = NULL;
    struct stack *stack;
    struct stack_item *item;

    end_token = (Token *)malloc (sizeof (Token));
    end_token->term = ITEM_DOLLAR;

    stack = stack_init ();
    stack_push (stack, end_token);

    next = TRUE;
    end = FALSE;
    while (!end) {
        item = stack->top;
        top_token = (Token *)stack->top->data;

        if (top_token->term == ITEM_E)
            top_token = (Token *)item->next->data;

        if (next)
            token = set_term (get_token (stack_token));

        switch (precedence_table[top_token->term][token->term]) {
            case PT_LESS:
                top_token = (Token *)stack->top->data;

                push_token = (Token *)malloc (sizeof (Token));
                push_token->term = ITEM_STOP;

                if (top_token->term == ITEM_E) {
                    stack_pop (stack);
                    stack_push (stack, push_token);

                    push_token = (Token *)malloc (sizeof (Token));
                    push_token->term = ITEM_E;
                    stack_push (stack, push_token);

                    stack_push (stack, token);
                } else {
                    stack_push (stack, push_token);
                    stack_push (stack, token);
                }

                next = TRUE;
                break;
            case PT_GREATER:
                switch (check_rule (stack)) {
                    case 0:
                    case 1:
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                    case 6:
                    case 7:
                    case 8:
                    case 9:
                    case 10:
                        push_token = (Token *)malloc (sizeof (Token));
                        push_token->term = ITEM_E;

                        stack_pop (stack);
                        stack_pop (stack);
                        stack_pop (stack);
                        stack_pop (stack);
                        stack_push (stack, push_token);
                        break;
                    case 11:
                    case 12:
                        push_token = (Token *)malloc (sizeof (Token));
                        push_token->term = ITEM_E;

                        stack_pop (stack);
                        stack_pop (stack);
                        stack_push (stack, push_token);
                        break;
                    default:
                        if (top_token->term != ITEM_DOLLAR) {
                            error (SYNTAX_ERROR);
                        } else {
                            end = true;
                            stack_push (stack_token, token);
                        }
                        break;
                }

                next = FALSE;
                break;
            case PT_EQUAL:
                stack_push (stack, token);

                next = TRUE;
                break;
            default:
                if (((Token *)stack->top->data)->term != ITEM_E)
                    error (SYNTAX_ERROR);
                break;
        }

        /*_item = stack->top;

        while (_item != NULL) {
            printf ("%d -> ", ((Token *)_item->data)->term);
            _item = _item->next;
        }
        printf ("END\n");*/

        if (token->term == ITEM_DOLLAR && top_token->term == ITEM_DOLLAR) {
            end = true;
            stack_push (stack_token, token);
        }
    }
}