#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define INPUT_SIZE 400
#define STACK_SIZE 30
#define STR_SIZE 20

char input[INPUT_SIZE];
int input_position = 0;
char stack[STACK_SIZE][STR_SIZE];
int stacksize = 0;
int table[14][16] =
{
  //{int,float,var,fun,main,( ,) ,+ ,- ,* ,// ,/ ,% ,^ ,, ,$
    {1  ,1    ,1  ,1  ,1   ,1 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //Em
    {0  ,0    ,0  ,0  ,0   ,0 ,4 ,2 ,3 ,0 ,0  ,0 ,0 ,0 ,0 ,4}, //E'
    {5  ,5    ,5  ,5  ,5   ,5 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //T
    {0  ,0    ,0  ,0  ,0   ,0 ,10,10,10,6 ,8  ,9 ,7 ,0 ,0 ,10}, //T''
    {11 ,11   ,11 ,11 ,11  ,11,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //F
    {0  ,0    ,0  ,0  ,0   ,0 ,13,13,13,13,13 ,13,13,12,0 ,13}, //F''
    {14 ,14   ,14 ,14 ,14  ,15,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //R
    {16 ,16   ,18 ,17 ,17  ,0 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //N'
    {20 ,19   ,0  ,0  ,0   ,0 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //L'
    {0  ,0    ,0  ,21 ,22  ,0 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //Mc
    {0  ,0    ,0  ,0  ,0   ,23,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //E''
    {0  ,0    ,25 ,0  ,0   ,0 ,24,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0}, //E1
    {0  ,0    ,0  ,0  ,0   ,0 ,26,0 ,0 ,0 ,0  ,0 ,0 ,0 ,27,0}, //E2
    {0  ,0    ,0  ,0  ,0   ,0 ,28,0 ,0 ,0 ,0  ,0 ,0 ,0 ,29,0}  //E3
};

int getrow(char *non_terminal)
{
    if (strcmp(non_terminal, "Em") == 0)
    {
        return 0;
    }
    else if (strcmp(non_terminal, "E'") == 0)
    {
        return 1;
    }
    else if (strcmp(non_terminal, "T") == 0)
    {
        return 2;
    }
    else if (strcmp(non_terminal, "T''") == 0)
    {
        return 3;
    }
    else if (strcmp(non_terminal, "F") == 0)
    {
        return 4;
    }
    else if (strcmp(non_terminal, "F''") == 0)
    {
        return 5;
    }
    else if (strcmp(non_terminal, "R") == 0)
    {
        return 6;
    }
    else if (strcmp(non_terminal, "N'") == 0)
    {
        return 7;
    }
    else if (strcmp(non_terminal, "L'") == 0)
    {
        return 8;
    }
    else if (strcmp(non_terminal, "Mc") == 0)
    {
        return 9;
    }
    else if (strcmp(non_terminal, "E''") == 0)
    {
        return 10;
    }
    else if (strcmp(non_terminal, "E1") == 0)
    {
        return 11;
    }
    else if (strcmp(non_terminal, "E2") == 0)
    {
        return 12;
    }
    else if (strcmp(non_terminal, "E3") == 0)
    {
        return 13;
    }
    else
    {
        printf("ERROR: row of non_terminal %s did not found!", non_terminal);
        exit(0);
    }
}

int getcolumn(char *token)
{
    if (strcmp(token, "<integer_literal>") == 0)
    {
        return 0;
    }
    else if (strcmp(token, "<float_literal>") == 0)
    {
        return 1;
    }
    else if (strcmp(token, "<variable>") == 0)
    {
        return 2;
    }
    else if (strcmp(token, "<function>") == 0)
    {
        return 3;
    }
    else if (strcmp(token, "<main>") == 0)
    {
        return 4;
    }
    else if (strcmp(token, "<(>") == 0)
    {
        return 5;
    }
    else if (strcmp(token, "<)>") == 0)
    {
        return 6;
    }
    else if (strcmp(token, "<+>") == 0)
    {
        return 7;
    }
    else if (strcmp(token, "<->") == 0)
    {
        return 8;
    }
    else if (strcmp(token, "<*>") == 0)
    {
        return 9;
    }
    else if (strcmp(token, "<//>") == 0)
    {
        return 10;
    }
    else if (strcmp(token, "</>") == 0)
    {
        return 11;
    }
    else if (strcmp(token, "<%>") == 0)
    {
        return 12;
    }
    else if (strcmp(token, "<^>") == 0)
    {
        return 13;
    }
    else if (strcmp(token, "<,>") == 0)
    {
        return 14;
    }
    else if (strcmp(token, "<$>") == 0)
    {
        return 15;
    }
    else
    {
        printf("ERROR: row of token %s did not found!", token);
        exit(0);
    }
}

void push(char *str)
{
    if (stacksize < STACK_SIZE)
    {
        strcpy(stack[stacksize++], str);
    }
    else
    {
        printf("ERROR: stackoverflow");
        exit(0);
    }
}

char *pop()
{
    if (stacksize > 0)
    {
        return stack[--stacksize];
    }
    else
    {
        printf("ERROR: Pop on empty stack");
        exit(0);
    }
}

void topelement(char *str)
{
    if (stacksize > 0)
    {
        strcpy(str, stack[stacksize - 1]);
    }
    else
    {
        printf("ERROR: top element on empty stack");
        exit(0);
    }
}

void do_rule(int rule, char *x, char *c)
{
    pop();
    switch(rule)
    {
        case 0: printf("ERROR: no rule associated to %s and %s", x, c); exit(0);
        case 1: printf("apply rule 1\n"); push("E'"); push("T"); break;
        case 2: printf("apply rule 2\n"); push("E'"); push("T"); push("<+>"); break;
        case 3: printf("apply rule 3\n"); push("E'"); push("T"); push("<->"); break;
        case 4: printf("apply rule 4\n"); break;
        case 5: printf("apply rule 5\n"); push("T''"); push("F"); break;
        case 6: printf("apply rule 6\n"); push("T''"); push("F"); push("<*>"); break;
        case 7: printf("apply rule 7\n"); push("T''"); push("F"); push("<%>"); break;
        case 8: printf("apply rule 8\n"); push("T''"); push("F"); push("<//>"); break;
        case 9: printf("apply rule 9\n"); push("T''"); push("F"); push("</>"); break;
        case 10: printf("apply rule 10\n"); break;
        case 11: printf("apply rule 11\n"); push("F''"); push("R"); break;
        case 12: printf("apply rule 12\n"); push("F''"); push("R"); push("<^>"); break;
        case 13: printf("apply rule 13\n"); break;
        case 14: printf("apply rule 14\n"); push("N'"); break;
        case 15: printf("apply rule 15\n"); push("<)>"); push("Em"); push("<(>"); break;
        case 16: printf("apply rule 16\n"); push("L'"); break;
        case 17: printf("apply rule 17\n"); push("Mc"); break;
        case 18: printf("apply rule 18\n"); push("<variable>"); break;
        case 19: printf("apply rule 19\n"); push("<float_literal>"); break;
        case 20: printf("apply rule 20\n"); push("<integer_literal>"); break;
        case 21: printf("apply rule 21\n"); push("E''"); push("<function>"); break;
        case 22: printf("apply rule 22\n"); push("E''"); push("<main>"); break;
        case 23: printf("apply rule 23\n"); push("E1"); push("<(>"); break;
        case 24: printf("apply rule 24\n"); push("<)>"); break;
        case 25: printf("apply rule 25\n"); push("E2"); push("<variable>"); break;
        case 26: printf("apply rule 26\n"); push("<)>"); break;
        case 27: printf("apply rule 27\n"); push("E3"); push("<variable>"); push("<,>"); break;
        case 28: printf("apply rule 28\n"); push("<)>"); break;
        case 29: printf("apply rule 29\n"); push("<)>"); push("<variable>"); push("<,>"); break;
    }
}

void parse(char *input)
{
    push("<$>");
    push("Em");
    char x[STR_SIZE];
    char c[STR_SIZE];
    firsttokenoninput(c);
    while (stacksize > 0)
    {
        topelement(x);
        char temp[STACK_SIZE * STR_SIZE] = "";
        for (int i = stacksize - 1; i >= 0 ; i--)
        {
            strcat(temp, stack[i]);
        }
        printf("stack: %50s", temp);
        printf(", input-top: %50s, action:", c);
        if (strcmp(x, c) == 0)
        {
            printf("match %s with %s\n", x, c);
            pop();
            if (input_position == strlen(input))
            {
                break;
            }
            firsttokenoninput(c); // advance to next token.
        }
        else if (x[0] == '<')
        {
            printf("ERROR: expected %s but faced with %s in the stack", c, x);
            exit(0);
        }
        else
        {
            do_rule(table[getrow(x)][getcolumn(c)], x, c);
        }
    }

    if (stacksize == 0)
    {
        printf("Parse was successful!");
    }
    else
    {
        printf("ERROR: stack remains but there are no inputs");
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

void firsttokenoninput(char *str)
{
    int i = 0;
    while (1)
    {
        if ((input[input_position] == '>') && (str[i - 1] != '<'))
        {
            str[i++] = input[input_position++];
            str[i] = '\0';
            break;
        }
        else
        {
            str[i++] = input[input_position++];
        }
    }
}

void main()
{
    FILE *tokensf = fopen("tokens.txt", "r");
    if (tokensf == NULL)
    {
        printf("Token file could not be opened");
    }
    else
    {
        fgets(input, INPUT_SIZE, tokensf);
        fclose(tokensf);
        strcat(input, "<$>");
        remove_spaces(input);
        printf("input: %s\n", input);
        parse(input);
    }
}
