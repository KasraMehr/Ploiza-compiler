#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ****************** تعریف نوع‌های داده متغیر ******************
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_BOOLEAN,
    TYPE_STRING
} VarType;

// ساختار نگهدارنده مقدار (Value) متغیر
typedef struct {
    VarType type;
    union {
        int i;
        float f;
        double d;
        int boolean;      // استفاده از int برای boolean (0 یا 1)
        char s[256];
    } data;
} Value;

// توابع کمکی ساخت Value
Value makeInt(int x) {
    Value v; v.type = TYPE_INT; v.data.i = x; return v;
}
Value makeFloat(float x) {
    Value v; v.type = TYPE_FLOAT; v.data.f = x; return v;
}
Value makeDouble(double x) {
    Value v; v.type = TYPE_DOUBLE; v.data.d = x; return v;
}
Value makeBoolean(int b) {
    Value v; v.type = TYPE_BOOLEAN; v.data.boolean = b ? 1 : 0; return v;
}
Value makeString(const char* s) {
    Value v; v.type = TYPE_STRING; strncpy(v.data.s, s, sizeof(v.data.s)-1); v.data.s[sizeof(v.data.s)-1] = '\0'; return v;
}

// تبدیل مقدار به double (برای محاسبات ریاضی)
double toDouble(Value v) {
    switch(v.type) {
        case TYPE_INT: return v.data.i;
        case TYPE_FLOAT: return v.data.f;
        case TYPE_DOUBLE: return v.data.d;
        case TYPE_BOOLEAN: return v.data.boolean;
        default: return 0;
    }
}

// تبدیل مقدار به بولی
int toBoolean(Value v) {
    switch(v.type) {
        case TYPE_INT: return v.data.i != 0;
        case TYPE_FLOAT: return v.data.f != 0.0f;
        case TYPE_DOUBLE: return v.data.d != 0.0;
        case TYPE_BOOLEAN: return v.data.boolean;
        default: return 0;
    }
}

// تبدیل مقدار به رشته (فقط برای اعداد و بولی)
void toString(Value v, char* buf, size_t bufSize) {
    switch(v.type) {
        case TYPE_INT:
            snprintf(buf, bufSize, "%d", v.data.i);
            break;
        case TYPE_FLOAT:
            snprintf(buf, bufSize, "%f", v.data.f);
            break;
        case TYPE_DOUBLE:
            snprintf(buf, bufSize, "%lf", v.data.d);
            break;
        case TYPE_BOOLEAN:
            snprintf(buf, bufSize, "%s", v.data.boolean ? "true" : "false");
            break;
        case TYPE_STRING:
            strncpy(buf, v.data.s, bufSize-1);
            buf[bufSize-1] = '\0';
            break;
    }
}

// تبدیل مقدار به هدف مورد نظر (در تعریف متغیر)
Value convertValue(Value v, VarType target) {
    Value result;
    result.type = target;
    switch(target) {
        case TYPE_INT:
            result.data.i = (int)toDouble(v);
            break;
        case TYPE_FLOAT:
            result.data.f = (float)toDouble(v);
            break;
        case TYPE_DOUBLE:
            result.data.d = toDouble(v);
            break;
        case TYPE_BOOLEAN:
            result.data.boolean = toBoolean(v);
            break;
        case TYPE_STRING: {
            char buf[256];
            toString(v, buf, sizeof(buf));
            strncpy(result.data.s, buf, sizeof(result.data.s)-1);
            result.data.s[sizeof(result.data.s)-1] = '\0';
            break;
        }
    }
    return result;
}

// ****************** تعریف نوع گره‌های AST ******************
typedef enum {
    NODE_INT,
    NODE_FLOAT,
    NODE_DOUBLE,
    NODE_BOOLEAN,
    NODE_STRING,
    NODE_OPERATOR,
    NODE_VARIABLE,
    NODE_DECLARATION
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        // برای اعداد صحیح
        int intValue;
        // برای اعداد اعشاری (float)
        float floatValue;
        // برای اعداد اعشاری (double)
        double doubleValue;
        // برای مقادیر بولی
        int booleanValue; // 0 یا 1
        // برای رشته‌ها
        char stringValue[256];

        // گره عملگرها
        struct {
            int operator; // استفاده از int (برای پشتیبانی از عملگرهای چند کاراکتری)
            struct ASTNode* left;
            struct ASTNode* right; // برای عملگر una­ry، right=NULL است.
        } op;

        // گره تعریف متغیر
        struct {
            char datatype[16];   // مانند "int"، "float" و ...
            char varname[32];
            struct ASTNode* value; // مقداردهی اولیه (اختیاری)
        } decl;
    };
} ASTNode;

// ****************** تعریف عملگرها ******************
// عملگرهای ریاضی تک کاراکتری از کد ASCII استفاده می‌شوند.
#define OP_ADD    '+'
#define OP_SUB    '-'
#define OP_MUL    '*'
#define OP_DIV    '/'
#define OP_MOD    '%'
#define OP_AND    256   // &&
#define OP_OR     257   // ||
#define OP_EQ     258   // ==
#define OP_NEQ    259   // !=
#define OP_NOT    260   // !

// ****************** جدول نمادها ******************
typedef struct Symbol {
    char name[32];
    Value value;
    struct Symbol* next;
} Symbol;

Symbol* symbolTable = NULL;

void insertSymbol(const char* name, Value value) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    strcpy(sym->name, name);
    sym->value = value;
    sym->next = symbolTable;
    symbolTable = sym;
}

Value lookupSymbol(const char* name) {
    Symbol* curr = symbolTable;
    while (curr != NULL) {
        if (strcmp(curr->name, name) == 0)
            return curr->value;
        curr = curr->next;
    }
    printf("error: '%s' has not been declared.\n", name);
    exit(1);
}

// ****************** توابع ایجاد گره‌های AST ******************
ASTNode* createIntNode(int value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_INT;
    node->intValue = value;
    return node;
}

ASTNode* createFloatNode(float value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_FLOAT;
    node->floatValue = value;
    return node;
}

ASTNode* createDoubleNode(double value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_DOUBLE;
    node->doubleValue = value;
    return node;
}

ASTNode* createBooleanNode(int value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_BOOLEAN;
    node->booleanValue = value ? 1 : 0;
    return node;
}

ASTNode* createStringNode(const char* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_STRING;
    strncpy(node->stringValue, value, sizeof(node->stringValue)-1);
    node->stringValue[sizeof(node->stringValue)-1] = '\0';
    return node;
}

ASTNode* createOperatorNode(int operator, ASTNode* left, ASTNode* right) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_OPERATOR;
    node->op.operator = operator;
    node->op.left = left;
    node->op.right = right;
    return node;
}

ASTNode* createDeclarationNode(const char* datatype, const char* varname, ASTNode* value) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = NODE_DECLARATION;
    strcpy(node->decl.datatype, datatype);
    strcpy(node->decl.varname, varname);
    node->decl.value = value;
    return node;
}

// ****************** توابع نمایش و آزادسازی AST ******************
void printAST(ASTNode* node, int depth) {
    if (node == NULL) return;
    for (int i = 0; i < depth; i++) printf("  ");
    switch (node->type) {
        case NODE_INT:
            printf("Int: %d\n", node->intValue);
            break;
        case NODE_FLOAT:
            printf("Float: %f\n", node->floatValue);
            break;
        case NODE_DOUBLE:
            printf("Double: %lf\n", node->doubleValue);
            break;
        case NODE_BOOLEAN:
            printf("Boolean: %s\n", node->booleanValue ? "true" : "false");
            break;
        case NODE_STRING:
            printf("String: \"%s\"\n", node->stringValue);
            break;
        case NODE_VARIABLE:
            printf("Variable: %s\n", node->stringValue); // در اینجا از فیلد stringValue برای نام متغیر استفاده می‌کنیم
            break;
        case NODE_OPERATOR: {
            printf("Operator: ");
            switch (node->op.operator) {
                case OP_ADD:  printf("+\n"); break;
                case OP_SUB:  printf("-\n"); break;
                case OP_MUL:  printf("*\n"); break;
                case OP_DIV:  printf("/\n"); break;
                case OP_MOD:  printf("%%\n"); break;
                case OP_AND:  printf("&&\n"); break;
                case OP_OR:   printf("||\n"); break;
                case OP_EQ:   printf("==\n"); break;
                case OP_NEQ:  printf("!=\n"); break;
                case OP_NOT:  printf("!\n"); break;
                default:      printf("Unknown(%d)\n", node->op.operator); break;
            }
            printAST(node->op.left, depth + 1);
            if (node->op.right)
                printAST(node->op.right, depth + 1);
            break;
        }
        case NODE_DECLARATION:
            printf("Declaration: %s %s\n", node->decl.datatype, node->decl.varname);
            if (node->decl.value) {
                printAST(node->decl.value, depth + 1);
            }
            break;
    }
}

void freeAST(ASTNode* node) {
    if (node == NULL) return;
    if (node->type == NODE_OPERATOR) {
        freeAST(node->op.left);
        freeAST(node->op.right);
    } else if (node->type == NODE_DECLARATION) {
        freeAST(node->decl.value);
    }
    free(node);
}

// ****************** مدیریت اولویت عملگرها ******************
int getPrecedence(int op) {
    switch (op) {
        case OP_MUL:
        case OP_DIV:
        case OP_MOD:
            return 3;
        case OP_ADD:
        case OP_SUB:
            return 2;
        case OP_EQ:
        case OP_NEQ:
            return 1;
        case OP_AND:
            return 0;
        case OP_OR:
            return -1;
        default:
            return -2;
    }
}

// ****************** توابع تجزیه (Parser) ******************
const char* expr;

// مشاهده عملگر (بدون پیشرفت اشاره‌گر)
int peekOperator(const char* p, int* opLen) {
    while (*p == ' ') p++;
    if (strncmp(p, "&&", 2) == 0) { *opLen = 2; return OP_AND; }
    if (strncmp(p, "||", 2) == 0) { *opLen = 2; return OP_OR; }
    if (strncmp(p, "==", 2) == 0) { *opLen = 2; return OP_EQ; }
    if (strncmp(p, "!=", 2) == 0) { *opLen = 2; return OP_NEQ; }
    if (strchr("+-*/%", *p)) { *opLen = 1; return *p; }
    return 0;
}

// تابع parsePrimary: تشخیص اعداد (صحیح/اعشاری)، رشته، مقادیر بولی و متغیرها
ASTNode* parsePrimary() {
    while (*expr == ' ') expr++;
    // اگر عدد شروع شود:
    if (isdigit(*expr)) {
        // تشخیص وجود نقطه برای عدد اعشاری:
        const char* start = expr;
        while (isdigit(*expr)) expr++;
        if (*expr == '.') {
            expr++; // مصرف نقطه
            while (isdigit(*expr)) expr++;
            char buf[64];
            int len = expr - start;
            strncpy(buf, start, len);
            buf[len] = '\0';
            double d = atof(buf);
            return createDoubleNode(d);
        } else {
            char buf[64];
            int len = expr - start;
            strncpy(buf, start, len);
            buf[len] = '\0';
            int val = atoi(buf);
            return createIntNode(val);
        }
    }
    // رشته‌ها (داخل " ")
    if (*expr == '\"') {
        expr++; // حذف "
        char buf[256] = {0};
        int i = 0;
        while (*expr && *expr != '\"' && i < 255) {
            buf[i++] = *expr;
            expr++;
        }
        if (*expr == '\"') expr++;
        return createStringNode(buf);
    }
    // مقادیر بولی
    if (strncmp(expr, "true", 4) == 0 && !isalnum(expr[4])) {
        expr += 4;
        return createBooleanNode(1);
    }
    if (strncmp(expr, "false", 5) == 0 && !isalnum(expr[5])) {
        expr += 5;
        return createBooleanNode(0);
    }
    // متغیرها (یا شناسه‌های دیگر)
    if (isalpha(*expr)) {
        char name[32] = {0};
        int i = 0;
        while (isalnum(*expr) && i < 31) {
            name[i++] = *expr;
            expr++;
        }
        // برای متغیر، ما از NODE_VARIABLE استفاده می‌کنیم.
        // در اینجا نام متغیر در فیلد stringValue ذخیره می‌شود.
        ASTNode* node = createStringNode(name);
        node->type = NODE_VARIABLE;
        return node;
    }
    return NULL;
}

// مدیریت عملگرهای una­ry (مانند !)
ASTNode* parseUnary() {
    while (*expr == ' ') expr++;
    if (*expr == '!') {
        expr++; // مصرف !
        ASTNode* operand = parseUnary();
        return createOperatorNode(OP_NOT, operand, NULL);
    }
    // می‌توان عملگر una­ry - را نیز اضافه کرد.
    return parsePrimary();
}

// تجزیه عبارات با اولویت‌بندی
ASTNode* parseExpression(int minPrecedence) {
    ASTNode* left = parseUnary();
    while (1) {
        while (*expr == ' ') expr++;
        int opLen = 0;
        int op = peekOperator(expr, &opLen);
        if (!op) break;
        int prec = getPrecedence(op);
        if (prec < minPrecedence) break;
        expr += opLen; // مصرف عملگر
        ASTNode* right = parseExpression(prec + 1);
        left = createOperatorNode(op, left, right);
    }
    return left;
}

// تجزیه دستورات (شامل تعریف متغیر)
ASTNode* parseStatement() {
    while (*expr == ' ') expr++;
    // بررسی انواع متغیر: int, float, double, boolean, string
    const char* types[] = {"int", "float", "double", "boolean", "string"};
    int typeFound = -1;
    for (int i = 0; i < 5; i++) {
        size_t len = strlen(types[i]);
        if (strncmp(expr, types[i], len) == 0 && isspace(expr[len])) {
            typeFound = i;
            expr += len;
            break;
        }
    }
    if (typeFound != -1) {
        // حذف فاصله‌ها
        while (*expr == ' ') expr++;
        // خواندن نام متغیر
        char varname[32] = {0};
        int i = 0;
        while (isalnum(*expr) && i < 31) {
            varname[i++] = *expr;
            expr++;
        }
        ASTNode* value = NULL;
        if (*expr == '=') {
            expr++; // حذف =
            value = parseExpression(0);
        }
        // برای تعریف متغیر از گره DECLARATION استفاده می‌شود
        // در فیلد decl.datatype، نوع به صورت رشته (مثلاً "int") ذخیره می‌شود.
        return createDeclarationNode(types[typeFound], varname, value);
    } else {
        return parseExpression(0);
    }
}

// ****************** ارزیابی AST ******************
// در اینجا تابع evaluate یک ساختار Value برمی‌گرداند.
Value evaluate(ASTNode* node);

Value evalBinaryOp(int op, Value left, Value right) {
    Value result;
    switch(op) {
        case OP_ADD:
            // اگر یکی از عملوندها رشته باشد، عمل + به معنای الحاق (concatenation) می‌شود.
            if (left.type == TYPE_STRING || right.type == TYPE_STRING) {
                char buf[512];
                char lstr[256], rstr[256];
                toString(left, lstr, sizeof(lstr));
                toString(right, rstr, sizeof(rstr));
                snprintf(buf, sizeof(buf), "%s%s", lstr, rstr);
                result = makeString(buf);
            } else if (left.type == TYPE_INT && right.type == TYPE_INT) {
                result = makeInt(left.data.i + right.data.i);
            } else {
                result = makeDouble(toDouble(left) + toDouble(right));
            }
            break;
        case OP_SUB:
            if (left.type == TYPE_INT && right.type == TYPE_INT) {
                result = makeInt(left.data.i - right.data.i);
            } else {
                result = makeDouble(toDouble(left) - toDouble(right));
            }
            break;
        case OP_MUL:
            if (left.type == TYPE_INT && right.type == TYPE_INT) {
                result = makeInt(left.data.i * right.data.i);
            } else {
                result = makeDouble(toDouble(left) * toDouble(right));
            }
            break;
        case OP_DIV:
            if (toDouble(right) == 0) {
                printf("error: division on 0\n");
                exit(1);
            }
            if (left.type == TYPE_INT && right.type == TYPE_INT) {
                result = makeInt(left.data.i / right.data.i);
            } else {
                result = makeDouble(toDouble(left) / toDouble(right));
            }
            break;
        case OP_MOD:
            if (right.type == TYPE_INT && right.data.i == 0) {
                printf("error: division on 0 in operation %%\n");
                exit(1);
            }
            if (left.type == TYPE_INT && right.type == TYPE_INT) {
                result = makeInt(left.data.i % right.data.i);
            } else {
                // برای مقادیر اعشاری، عملگر مد تعریف نشده است.
                printf("error: can't use mod on double\n");
                exit(1);
            }
            break;
        case OP_AND:
            result = makeBoolean(toBoolean(left) && toBoolean(right));
            break;
        case OP_OR:
            result = makeBoolean(toBoolean(left) || toBoolean(right));
            break;
        case OP_EQ:
            // برای مقایسه، در صورت عددی بودن از toDouble استفاده می‌کنیم؛ در غیر این صورت، برای رشته‌ها از strcmp
            if ((left.type == TYPE_STRING) || (right.type == TYPE_STRING)) {
                result = makeBoolean(strcmp(left.data.s, right.data.s) == 0);
            } else {
                result = makeBoolean(toDouble(left) == toDouble(right));
            }
            break;
        case OP_NEQ:
            if ((left.type == TYPE_STRING) || (right.type == TYPE_STRING)) {
                result = makeBoolean(strcmp(left.data.s, right.data.s) != 0);
            } else {
                result = makeBoolean(toDouble(left) != toDouble(right));
            }
            break;
        default:
            printf("invalid operator: %d\n", op);
            exit(1);
    }
    return result;
}

Value evaluate(ASTNode* node) {
    if (node == NULL) return makeInt(0);
    Value result;
    switch(node->type) {
        case NODE_INT:
            return makeInt(node->intValue);
        case NODE_FLOAT:
            return makeFloat(node->floatValue);
        case NODE_DOUBLE:
            return makeDouble(node->doubleValue);
        case NODE_BOOLEAN:
            return makeBoolean(node->booleanValue);
        case NODE_STRING:
            return makeString(node->stringValue);
        case NODE_VARIABLE: {
            // نام متغیر در فیلد stringValue (که در parsePrimary تنظیم شده بود)
            return lookupSymbol(node->stringValue);
        }
        case NODE_OPERATOR: {
            // عملگر una­ry !
            if (node->op.operator == OP_NOT) {
                Value val = evaluate(node->op.left);
                return makeBoolean(!toBoolean(val));
            }
            Value left = evaluate(node->op.left);
            Value right = evaluate(node->op.right);
            return evalBinaryOp(node->op.operator, left, right);
        }
        case NODE_DECLARATION: {
            // ارزیابی مقداردهی اولیه (در صورت وجود)
            Value initVal = makeInt(0);
            if (node->decl.value)
                initVal = evaluate(node->decl.value);
            // تعیین نوع متغیر از روی رشته decl.datatype
            VarType target;
            if (strcmp(node->decl.datatype, "int") == 0)
                target = TYPE_INT;
            else if (strcmp(node->decl.datatype, "float") == 0)
                target = TYPE_FLOAT;
            else if (strcmp(node->decl.datatype, "double") == 0)
                target = TYPE_DOUBLE;
            else if (strcmp(node->decl.datatype, "boolean") == 0)
                target = TYPE_BOOLEAN;
            else if (strcmp(node->decl.datatype, "string") == 0)
                target = TYPE_STRING;
            else {
                printf("invalid datatype: %s\n", node->decl.datatype);
                exit(1);
            }
            Value convVal = convertValue(initVal, target);
            insertSymbol(node->decl.varname, convVal);
            return convVal;
        }
    }
    return makeInt(0);
}

// ****************** تابع main ******************
int main() {
    // اگر آخرین دستور هم به نقطه‌ویرگول ختم شود یا نشود؛ مهم این است که اشاره‌گر به انتها برسد.
    expr = "string x = 4;";

    printf("Abstract Syntax Tree:\n");
    while (*expr) {
        // ذخیره موقعیت فعلی اشاره‌گر
        const char *prevExpr = expr;

        ASTNode* root = parseStatement();
        if (!root) {
            // اگر هیچ نودی دریافت نشد، می‌توانیم حلقه را بشکنیم
            break;
        }
        printAST(root, 0);
        Value res = evaluate(root);

        char buf[256];
        toString(res, buf, sizeof(buf));
        printf("Result: %s\n\n", buf);

        freeAST(root);

        // مصرف تمام کاراکترهای فاصله و جداکننده‌ها
        while (*expr == ';' || isspace(*expr)) {
            expr++;
        }

        // اگر اشاره‌گر تغییر نکرد، یعنی چیزی برای پردازش وجود ندارد
        if (expr == prevExpr) break;
    }
    return 0;
}
