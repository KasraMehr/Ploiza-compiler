#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_STACK_SIZE 100

// Function prototypes
void parse_input(char* input);
int is_operator(char* c);
int precedence(char* c);

// Stack implementation
char stack[MAX_STACK_SIZE];
int stack_top = -1;

void push(char c) {
    if (stack_top < MAX_STACK_SIZE - 1) {
        stack[++stack_top] = c;
    } else {
        printf("Stack overflow\n");
    }
}

char pop() {
    if (stack_top >= 0) {
        return stack[stack_top--];
    } else {
        return -1;
    }
}

int peek() {
    if (stack_top >= 0) {
        return stack[stack_top];
    } else {
        return -1;
    }
}

// Function to check if a character is an operator (comparison operator)
int is_operator(char* c) {
    return (strcmp(c, "==") == 0 || strcmp(c, "<") == 0 || strcmp(c, "<=") == 0 ||
            strcmp(c, ">") == 0 || strcmp(c, ">=") == 0 || strcmp(c, "!=") == 0);
}

// Function to parse the input using LL(1) parsing strategy
void parse_input(char* input) {
    push('E');  // Start parsing with the start symbol E

    int i = 0;
    char current_char = input[i];

    while (peek() != -1) {
        char stack_top_symbol = peek();

        if (isdigit(current_char)) {
            // If the current char is a number, match the number
            printf("Number: %c\n", current_char);
            i++;
            current_char = input[i];
            pop(); // Pop the number from the stack
        }
        else if (stack_top_symbol == 'E') {
            if (current_char == '(' || isdigit(current_char)) {
                // E -> T E'
                push('E');
                push('T');
            } else {
                printf("Error: Unexpected character '%c'\n", current_char);
                return;
            }
        }
        else if (stack_top_symbol == 'E') {
            if (is_operator(&current_char)) {
                // E' -> comparison operator T E'
                char op[3] = {current_char, input[i+1], '\0'};
                pop();
                push('T');
                push('E');
                printf("Comparison Operator: %s\n", op);
                i+=2;  // Move past the operator
            }
        }
        else if (stack_top_symbol == 'T') {
            // T -> F T'
            push('T');
            push('F');
        }
        else if (stack_top_symbol == 'T') {
            // T' -> comparison operator
            char op[3] = {current_char, input[i+1], '\0'};
            if (op[1] == '\0') {
                printf("Term Operator: %s\n", op);
                i+=1;
            } else {
                printf("Binary Operation: %s\n", op);
                i+=2;
            }
        }
    }

    printf("Expression parsing complete!\n");
}

int main() {
    // Example logical expression to parse
    char input[] = "( 3 == 4 )";
    parse_input(input);
    return 0;
}
