#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

char input[100];
int position = 0;
char lookahead;

// Function prototypes
void E();
void E_prime();
void T();
void T_prime();
void F();
void L();
void match(char expected);
void remove_spaces(char *str);

// Error handling
void syntax_error(const char *msg) {
    printf("Syntax Error: %s at position %d near '%c'\n", msg, position, lookahead);
    exit(1);
}

// Advance to the next character
void advance() {
    position++;
    lookahead = input[position];
}

// Match the current character
void match(char expected) {
    if (lookahead == expected) {
        advance();
    } else {
        syntax_error("Unexpected character");
    }
}

// Parse E (E -> T E')
void E() {
    T();
    E_prime();
}

// Parse E' (E' -> + T E' | - T E' | L | epsilon)
void E_prime() {
    if (lookahead == '+') {
        match('+');
        if (!isdigit(lookahead) && lookahead != '(') {
            syntax_error("Expected number or '(' after '+'");
        }
        T();
        E_prime();
    } else if (lookahead == '-') {
        match('-');
        if (!isdigit(lookahead) && lookahead != '(') {
            syntax_error("Expected number or '(' after '-'");
        }
        T();
        E_prime();
    } else if (lookahead == '=' || lookahead == '<' || lookahead == '>' || lookahead == '!') {
        L();
    }
    // epsilon production: do nothing
}

// Parse L (L -> == T | < T | <= T | > T | >= T | != T)
void L() {
    if (lookahead == '=' || lookahead == '<' || lookahead == '>' || lookahead == '!') {
        if (lookahead == '=') {
            match('=');
            match('=');
        } else if (lookahead == '<') {
            match('<');
            if (lookahead == '=') {
                match('=');
            }
        } else if (lookahead == '>') {
            match('>');
            if (lookahead == '=') {
                match('=');
            }
        } else if (lookahead == '!') {
            match('!');
            match('=');
        } else {
            syntax_error("Unexpected logical operator");
        }
        if (!isdigit(lookahead) && lookahead != '(') {
            syntax_error("Expected number or '(' after logical operator");
        }
        T();
    } else {
        syntax_error("Expected logical operator");
    }
}

// Parse T (T -> F T')
void T() {
    F();
    T_prime();
}

// Parse T' (T' -> * F T' | / F T' | epsilon)
void T_prime() {
    if (lookahead == '*') {
        match('*');
        if (!isdigit(lookahead) && lookahead != '(') {
            syntax_error("Expected number or '(' after '*'");
        }
        F();
        T_prime();
    } else if (lookahead == '/') {
        match('/');
        if (!isdigit(lookahead) && lookahead != '(') {
            syntax_error("Expected number or '(' after '/'");
        }
        F();
        T_prime();
    }
    // epsilon production: do nothing
}

// Parse F (F -> ( E ) | NUMBER)
void F() {
    if (lookahead == '(') {
        match('(');
        E();
        match(')');
    } else if (isdigit(lookahead)) {
        while (isdigit(lookahead)) {
            advance();
        }
    } else {
        syntax_error("Expected number or '('");
    }
}

void remove_spaces(char *str) {
    int i = 0, j = 0;

    // Loop through each character in the string
    while (str[i]) {
        if (str[i] != ' ') {
            str[j++] = str[i]; // Copy non-space character to the compacted position
        }
        i++;
    }
    str[j] = '\0'; // Null-terminate the result
}

int main() {
    printf("Enter an expression: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '$'; // Add end-of-input marker
    remove_spaces(input);
    position = 0;
    lookahead = input[position];

    E();

    if (lookahead == '$') {
        printf("Parsing successful!\n");
    } else {
        syntax_error("Unexpected input");
    }

    return 0;
}
