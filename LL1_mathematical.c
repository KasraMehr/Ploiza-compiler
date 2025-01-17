#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_RULES 10
#define MAX_SYMBOLS 10

// Definitions for token types
#define NUM 0
#define PLUS 1
#define MINUS 2
#define MUL 3
#define DIV 4
#define LPAREN 5
#define RPAREN 6
#define END 7

// Function to identify token types
int get_token(char c) {
    if (c == '+') return PLUS;
    if (c == '-') return MINUS;
    if (c == '*') return MUL;
    if (c == '/') return DIV;
    if (c == '(') return LPAREN;
    if (c == ')') return RPAREN;
    if (c >= '0' && c <= '9') return NUM;
    if (c == '\0') return END;
    return -1;
}

// Grammar productions
// Expr -> Term Expr'
// Expr' -> + Term Expr' | - Term Expr' | ε
// Term -> Factor Term'
// Term' -> * Factor Term' | / Factor Term' | ε
// Factor -> ( Expr ) | num

// Parse table for LL(1) (Simplified version)
// Each production is represented by an index (0 to n) for rules
// M[non-terminal][token]
char* parse_table[5][8];

// Non-terminal rules indices
#define RULE_EXPR 0
#define RULE_EXPR_PRIME 1
#define RULE_TERM 2
#define RULE_TERM_PRIME 3
#define RULE_FACTOR 4

void construct_parse_table() {
    // Fill in the table based on LL(1) parsing logic

    // Rule: Expr -> Term Expr'
    parse_table[0][NUM] = "Term Expr'";
    parse_table[0][LPAREN] = "Term Expr'";

    // Rule: Expr' -> + Term Expr'
    parse_table[1][PLUS] = "+ Term Expr'";

    // Rule: Expr' -> - Term Expr'
    parse_table[1][MINUS] = "- Term Expr'";

    // Rule: Expr' -> ε (empty string)
    parse_table[1][RPAREN] = "";
    parse_table[1][END] = "";

    // Rule: Term -> Factor Term'
    parse_table[2][NUM] = "Factor Term'";
    parse_table[2][LPAREN] = "Factor Term'";

    // Rule: Term' -> * Factor Term'
    parse_table[3][MUL] = "* Factor Term'";

    // Rule: Term' -> / Factor Term'
    parse_table[3][DIV] = "/ Factor Term'";

    // Rule: Term' -> ε (empty string)
    parse_table[3][PLUS] = "";
    parse_table[3][MINUS] = "";
    parse_table[3][RPAREN] = "";
    parse_table[3][END] = "";

    // Rule: Factor -> ( Expr )
    parse_table[4][LPAREN] = "( Expr )";

    // Rule: Factor -> num
    parse_table[4][NUM] = "num";
}

// Parser function
void parse_input(char* input) {
    char stack[MAX_SYMBOLS];
    int stack_top = -1;

    // Start with the non-terminal 'Expr'
    stack[++stack_top] = 'E';

    int input_idx = 0;
    int current_token = get_token(input[input_idx]);

    while (stack_top != -1) {
        char current_stack = stack[stack_top];

        if (current_stack == current_token) {
            // If the token matches, pop from the stack and move to the next token
            stack_top--;
            input_idx++;
            current_token = get_token(input[input_idx]);
        } else if (current_stack == 'E') {
            // Parse Expr
            char* rule = parse_table[RULE_EXPR][current_token];
            if (rule != NULL) {
                for (int i = strlen(rule) - 1; i >= 0; i--) {
                    stack[++stack_top] = rule[i];
                }
            } else {
                printf("Syntax error: Unexpected token.\n");
                return;
            }
        } else if (current_stack == 'T') {
            // Parse Term
            char* rule = parse_table[RULE_TERM][current_token];
            if (rule != NULL) {
                for (int i = strlen(rule) - 1; i >= 0; i--) {
                    stack[++stack_top] = rule[i];
                }
            } else {
                printf("Syntax error: Unexpected token.\n");
                return;
            }
        } else {
            printf("Syntax error: Unexpected character in input.\n");
            return;
        }
    }
    printf("Expression parsed successfully.\n");
}

int main() {
    // Initialize parse table
    construct_parse_table();

    // Test the parser with a simple input
    char input[] = "3+5*2";
    parse_input(input);

    return 0;
}
