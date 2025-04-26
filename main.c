#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CALC_DEBUG // Comment Line if not debug

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#ifndef M_E
#define M_E		2.7182818284590452354
#endif
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2		1.57079632679489661923
#endif

// Точность и лимиты
#define GAMMA_EPSILON 1e-10
#define MAX_ITERATIONS 100

void critic_error(const char *message) {
    fprintf(stderr, "Error!: %s\n", message);
    exit(EXIT_FAILURE);
}

typedef struct {
    const char *source;
    char current_char;
    size_t current_position;
    size_t length;
} Lexer;

#define LEXER_TOKEN_MAP(X) \
    X(LEXER_TOKEN_EOF, "LEXER_TOKEN_EOF") \
    X(LEXER_TOKEN_UNKNOWN, "LEXER_TOKEN_UNKNOWN") \
    X(LEXER_TOKEN_NUMBER, "number") \
    X(LEXER_TOKEN_LPAREN, "(") \
    X(LEXER_TOKEN_RPAREN, ")") \
    X(LEXER_TOKEN_PLUS, "+") \
    X(LEXER_TOKEN_MINUS, "-") \
    X(LEXER_TOKEN_MUL, "*") \
    X(LEXER_TOKEN_DIV, "/") \
    X(LEXER_TOKEN_INT_DIV, "//") \
    X(LEXER_TOKEN_MOD, "%") \
    X(LEXER_TOKEN_SQRT, "sqrt") \
    X(LEXER_TOKEN_POWER, "**") \
    X(LEXER_TOKEN_LG, "lg") \
    X(LEXER_TOKEN_LN, "ln") \
    X(LEXER_TOKEN_LOG, "log") \
    X(LEXER_TOKEN_ARC_SIN, "arcsin") \
    X(LEXER_TOKEN_SIN, "sin") \
    X(LEXER_TOKEN_ARC_COS, "arccos") \
    X(LEXER_TOKEN_COS, "cos") \
    X(LEXER_TOKEN_ARC_TAN, "arctan") \
    X(LEXER_TOKEN_TAN, "tan") \
    X(LEXER_TOKEN_ARC_COT, "arccot") \
    X(LEXER_TOKEN_COT, "cot") \
    X(LEXER_TOKEN_ABS, "abs") \
    X(LEXER_TOKEN_EXP, "exp") \
    X(LEXER_TOKEN_ARC_GAMMA, "arcgamma") \
    X(LEXER_TOKEN_GAMMA, "gamma") \
    X(LEXER_TOKEN_ARC_FACTORIAL, "arcfactorial") \
    X(LEXER_TOKEN_FACTORIAL, "factorial")

#define AST_NODE_TYPE_MAP(X) \
    X(AST_NODE_UNKNOWN, "AST_NODE_UNKNOWN") \
    X(AST_NODE_NUMBER, "number") \
    X(AST_NODE_BINARY_OP, "binary op") \
    X(AST_NODE_UNARY_OP, "unary op")

#define AST_UNARY_NODE_TYPE_MAP(X) \
    X(AST_UNARY_UNKNOWN, "AST_UNARY_UNKNOWN") \
    X(AST_UNARY_NEGATIVE, "negative") \
    X(AST_UNARY_ABS, "abs") \
    X(AST_UNARY_SQRT, "sqrt") \
    X(AST_UNARY_LG, "lg") \
    X(AST_UNARY_ARC_SIN, "asin") \
    X(AST_UNARY_SIN, "sin") \
    X(AST_UNARY_ARC_COS, "acos") \
    X(AST_UNARY_COS, "cos") \
    X(AST_UNARY_ARC_TAN, "atan") \
    X(AST_UNARY_TAN, "tan") \
    X(AST_UNARY_ARC_COT, "cot") \
    X(AST_UNARY_COT, "cot") \
    X(AST_UNARY_LN, "ln") \
    X(AST_UNARY_EXP, "exp") \
    X(AST_UNARY_ARC_GAMMA, "arcgamma") \
    X(AST_UNARY_GAMMA, "gamma") \
    X(AST_UNARY_ARC_FACTORIAL, "arcfactorial") \
    X(AST_UNARY_FACTORIAL, "factorial")

#define AST_BINARY_NODE_TYPE_MAP(X) \
    X(AST_BINARY_UNKNOWN, "AST_BINARY_UNKNOWN") \
    X(AST_BINARY_ADD, "add") \
    X(AST_BINARY_SUB, "sub") \
    X(AST_BINARY_MUL, "mul") \
    X(AST_BINARY_DIV, "div") \
    X(AST_BINARY_INT_DIV, "int div") \
    X(AST_BINARY_MOD, "mod") \
    X(AST_BINARY_POWER, "power") \
    X(AST_BINARY_LOG, "log")


typedef enum {
#define X(name, value) name,
    LEXER_TOKEN_MAP(X)
#undef X
    LEXER_TOKEN_COUNT
} LexerTokenType;

typedef enum {
#define X(name, text) name,
    AST_NODE_TYPE_MAP(X)
#undef X
    AST_NODE_COUNT
} ASTNodeType;

typedef enum {
#define X(name, text) name,
    AST_BINARY_NODE_TYPE_MAP(X)
#undef X
    AST_BINARY_NODE_COUNT
} ASTBinaryNodeType;

typedef enum {
#define X(name, text) name,
    AST_UNARY_NODE_TYPE_MAP(X)
#undef X
    AST_UNARY_NODE_COUNT
} ASTUnaryNodeType;


const char *LexerTokenType_to_string(const LexerTokenType token_type) {
    static const char *token_type_string[] = {
#define X(name, value) [name] = value,
        LEXER_TOKEN_MAP(X)
#undef X
    };

    if (token_type >= LEXER_TOKEN_COUNT) {
        return token_type_string[LEXER_TOKEN_UNKNOWN];
    }

    return token_type_string[token_type];
}

const char *ASTNodeType_to_string(const ASTNodeType node_type) {
    static const char *node_type_string[] = {
#define X(name, value) [name] = value,
        AST_NODE_TYPE_MAP(X)
#undef X
    };
    if (node_type >= AST_NODE_COUNT) {
        return node_type_string[AST_NODE_UNKNOWN];
    }

    return node_type_string[node_type];
}

const char *ASTBinaryNodeType_to_string(const ASTBinaryNodeType node_type) {
    static const char *node_type_string[] = {
#define X(name, value) [name] = value,
        AST_BINARY_NODE_TYPE_MAP(X)
#undef X
    };

    if (node_type >= AST_BINARY_NODE_COUNT) {
        return node_type_string[AST_BINARY_UNKNOWN];
    }

    return node_type_string[node_type];
}

const char *ASTUnaryNodeType_to_string(const ASTUnaryNodeType node_type) {
    static const char *node_type_string[] = {
#define X(name, value) [name] = value,
        AST_UNARY_NODE_TYPE_MAP(X)
#undef X
    };

    if (node_type >= AST_UNARY_NODE_COUNT) {
        return node_type_string[AST_UNARY_UNKNOWN];
    }

    return node_type_string[node_type];
}


typedef struct {
    LexerTokenType type;

    union {
        double number;
    };
} LexerToken;

Lexer Lexer_init(const char *source) {
    if (source == NULL) {
        return (Lexer){
            .source = NULL,
            .current_char = 0,
            .current_position = 0,
            .length = 0,
        };
    }
    const size_t length = strlen(source);

    const char current_char = (length > 0) ? source[0] : '\0';

    return (Lexer){
        .source = source,
        .current_char = current_char,
        .current_position = 0,
        .length = length,
    };
}

void Lexer_reset(Lexer *lexer) {
    lexer->current_position = 0;
    lexer->current_char = (lexer->length > 0) ? lexer->source[0] : '\0';
}

void Lexer_advance(Lexer *lexer) {
    if (lexer->current_position + 1 <= lexer->length) {
        ++lexer->current_position;
        lexer->current_char = lexer->source[lexer->current_position];
    } else {
        lexer->current_char = '\0';
    }
}

int is_digit(const int ch) {
    return ch >= '0' && ch <= '9';
}

LexerToken Lexer_parse_number(Lexer *lexer) {
    static char buffer[128];

    buffer[0] = lexer->current_char;
    size_t buffer_length = 1;
    Lexer_advance(lexer);
    bool is_int = true;

    while (buffer_length < 127) {
        if (is_digit(lexer->current_char)) {
        } else if (lexer->current_char == '.' && is_int) {
            is_int = false;
        } else {
            break;
        }

        buffer[buffer_length++] = lexer->current_char;
        Lexer_advance(lexer);
    }

    buffer[buffer_length] = '\0';

    return (LexerToken){
        .type = LEXER_TOKEN_NUMBER,
        .number = atof(buffer)
    };
}

LexerToken Lexer_parse_identifier(Lexer *lexer) {
    static char buffer[8];

    buffer[0] = lexer->current_char;
    size_t buffer_length = 1;
    Lexer_advance(lexer);

    while (isalnum(lexer->current_char) && buffer_length < 127) {
        buffer[buffer_length++] = lexer->current_char;
        Lexer_advance(lexer);
    }

    buffer[buffer_length] = '\0';

#define IDENTIFIER_COMPARE(name, token_type) if (strcasecmp(buffer, name) == 0) return (LexerToken){ .type = token_type };

    IDENTIFIER_COMPARE("sqrt", LEXER_TOKEN_SQRT)
    IDENTIFIER_COMPARE("lg", LEXER_TOKEN_LG)
    IDENTIFIER_COMPARE("ln", LEXER_TOKEN_LN)
    IDENTIFIER_COMPARE("log", LEXER_TOKEN_LOG)
    IDENTIFIER_COMPARE("sin", LEXER_TOKEN_SIN)
    IDENTIFIER_COMPARE("cos", LEXER_TOKEN_COS)
    IDENTIFIER_COMPARE("tan", LEXER_TOKEN_TAN)
    IDENTIFIER_COMPARE("tg", LEXER_TOKEN_TAN)
    IDENTIFIER_COMPARE("cot", LEXER_TOKEN_COT)
    IDENTIFIER_COMPARE("ctg", LEXER_TOKEN_COT)
    IDENTIFIER_COMPARE("abs", LEXER_TOKEN_ABS)
    IDENTIFIER_COMPARE("exp", LEXER_TOKEN_EXP)
    IDENTIFIER_COMPARE("gamma", LEXER_TOKEN_GAMMA)
    IDENTIFIER_COMPARE("factorial", LEXER_TOKEN_FACTORIAL)
    IDENTIFIER_COMPARE("fac", LEXER_TOKEN_FACTORIAL)
    IDENTIFIER_COMPARE("arcsin", LEXER_TOKEN_ARC_SIN)
    IDENTIFIER_COMPARE("asin", LEXER_TOKEN_ARC_SIN)
    IDENTIFIER_COMPARE("arccos", LEXER_TOKEN_ARC_COS)
    IDENTIFIER_COMPARE("acos", LEXER_TOKEN_ARC_COS)
    IDENTIFIER_COMPARE("arctan", LEXER_TOKEN_ARC_TAN)
    IDENTIFIER_COMPARE("arctg", LEXER_TOKEN_ARC_TAN)
    IDENTIFIER_COMPARE("arccot", LEXER_TOKEN_ARC_COT)
    IDENTIFIER_COMPARE("arcctg", LEXER_TOKEN_ARC_COT)
    IDENTIFIER_COMPARE("arcgamma", LEXER_TOKEN_ARC_GAMMA)
    IDENTIFIER_COMPARE("arcfactorial", LEXER_TOKEN_ARC_FACTORIAL)
    IDENTIFIER_COMPARE("arcfac", LEXER_TOKEN_ARC_FACTORIAL)

#undef IDENTIFIER_COMPARE

    if (strcasecmp(buffer, "pi") == 0) return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_PI};
    if (strcasecmp(buffer, "p") == 0) return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_PI};
    if (strcasecmp(buffer, "e") == 0) return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_E};

    return (LexerToken){
        .type = LEXER_TOKEN_UNKNOWN,
    };
}

char Lexer_next_char(const Lexer *lexer) {
    if (lexer->current_position + 1 < lexer->length) {
        return lexer->source[lexer->current_position + 1];
    }
    return '\0';
}

int is_space(const int ch) {
    return ch == ' ' || // пробел
           ch == '\f' || // форм-фид
           ch == '\n' || // перевод строки
           ch == '\r' || // возврат каретки
           ch == '\t' || // табуляция
           ch == '\v'; // вертикальная табуляция
}

LexerToken Lexer_next_token(Lexer *lexer) {
    if (lexer == NULL) return (LexerToken){.type = LEXER_TOKEN_EOF};
    if (lexer->current_position + 1 > lexer->length) {
        return (LexerToken){.type = LEXER_TOKEN_EOF};
    }
    while (is_space(lexer->current_char)) {
        Lexer_advance(lexer);
    }

    if (isdigit(lexer->current_char)) {
        return Lexer_parse_number(lexer);
    }

    if (isalpha(lexer->current_char)) {
        return Lexer_parse_identifier(lexer);
    }

    const char current_char = lexer->current_char;
    const char next_char = Lexer_next_char(lexer);
    Lexer_advance(lexer);
    LexerTokenType type;

    switch (current_char) {
        case '+':
            type = LEXER_TOKEN_PLUS;
            break;
        case '-':
            type = LEXER_TOKEN_MINUS;
            break;
        case '%':
            type = LEXER_TOKEN_MOD;
            break;
        case '(':
        case '[':
            type = LEXER_TOKEN_LPAREN;
            break;
        case ')':
        case ']':
            type = LEXER_TOKEN_RPAREN;
            break;
        case '*':
            if (next_char == '*') {
                type = LEXER_TOKEN_POWER;
                Lexer_advance(lexer);
                break;
            }
            type = LEXER_TOKEN_MUL;
            break;
        case '/':
            if (next_char == '/') {
                type = LEXER_TOKEN_INT_DIV;
                Lexer_advance(lexer);
                break;
            }
            type = LEXER_TOKEN_DIV;
            break;
        default:
            type = LEXER_TOKEN_UNKNOWN;
            break;
    }

    return (LexerToken){
        .type = type,
    };
}

typedef enum {
    AST_NODE_USED_FROM_FREE,
    AST_NODE_USED_FROM_PULL,
    AST_NODE_USED_FROM_MALLOC,
} ASTNodeUsed;

typedef struct tag_ASTNode {
    ASTNodeType type;
    ASTNodeUsed used;

    union {
        double number;

        struct {
            ASTBinaryNodeType op;
            struct tag_ASTNode *left;
            struct tag_ASTNode *right;
        } binary;

        struct {
            ASTUnaryNodeType op;
            struct tag_ASTNode *operand;
        } unary;
    };
} ASTNode;

typedef struct {
    ASTNode *nodes;
    size_t capacity;
    size_t reserved;
} ASTNodePull;

void ASTNodePull_init(ASTNodePull *node_pull, const size_t capacity) {
    node_pull->nodes = calloc(capacity, sizeof(ASTNode));
    if (node_pull->nodes == NULL) {
        critic_error("Failed to allocate memory for ASTNodePull");
    }
    node_pull->capacity = capacity;
    node_pull->reserved = 0;
}

void ASTNodePull_free(const ASTNodePull *node_pull) {
    if (node_pull == NULL) return;
    free(node_pull->nodes);
}

void ASTNode_allocate(ASTNode **node, const ASTNodeType type) {
    *node = (ASTNode *) malloc(sizeof(ASTNode));
    if (*node == NULL) {
        critic_error("ASTNode_allocate: malloc failed");
    }
    (*node)->type = type;
}

void ASTNode_free(ASTNode *node) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
        case AST_NODE_BINARY_OP:
            ASTNode_free(node->binary.left);
            ASTNode_free(node->binary.right);
            break;
        case AST_NODE_UNARY_OP:
            ASTNode_free(node->unary.operand);
            break;
        default:
            break;
    }

    free(node);
}

ASTNode *ASTNodePull_find_free(const ASTNodePull *node_pull) {
    if (node_pull == NULL || node_pull->reserved >= node_pull->capacity) return NULL;
    for (size_t i = 0; i < node_pull->capacity; i++) {
        if (node_pull->nodes[i].used == AST_NODE_USED_FROM_FREE) {
            ASTNode *node = node_pull->nodes + i;
            return node;
        }
    }
    return NULL;
}

ASTNode *ASTNodePull_pop(ASTNodePull *node_pull, const ASTNodeType node_type) {
    if (node_pull == NULL) return NULL;

    if (node_pull->reserved >= node_pull->capacity) {
        ASTNode *node;
        ASTNode_allocate(&node, node_type);
        node->used = AST_NODE_USED_FROM_MALLOC;
        return node;
    }

    ASTNode *node = ASTNodePull_find_free(node_pull);
    if (node == NULL) return NULL;

    node->used = AST_NODE_USED_FROM_PULL;
    node->type = node_type;
    return node;
}

void ASTNodePull_push(ASTNodePull *node_pull, ASTNode *node) {
    if (node_pull == NULL) return;

    if (node->used == AST_NODE_USED_FROM_MALLOC) {
        ASTNode_free(node);
        return;
    }

    node_pull->reserved -= 1;
    node->used = AST_NODE_USED_FROM_FREE;
}

typedef struct {
    Lexer *lexer;
    ASTNodePull node_pull;
    LexerToken current_token;
} Parser;

Parser Parser_init(Lexer *lexer, const size_t node_pull_capacity) {
    Parser parser = (Parser){
        .lexer = lexer,
        .current_token = Lexer_next_token(lexer),
    };
    ASTNodePull_init(&parser.node_pull, node_pull_capacity);

    return parser;
}

void Parser_reset(Parser *parser, Lexer *lexer) {
    if (parser == NULL) return;

    parser->lexer = lexer;
    Lexer_reset(parser->lexer);
    parser->current_token = Lexer_next_token(lexer);
}

void Parser_next_token(Parser *parser) {
    parser->current_token = Lexer_next_token(parser->lexer);
}

ASTNode *Parser_parse_expression(Parser *parser, int min_precedence);

ASTNode *Parser_parse_primary(Parser *parser) {
    ASTNode *node = NULL;
    switch (parser->current_token.type) {
        case LEXER_TOKEN_NUMBER:
            node = ASTNodePull_pop(&parser->node_pull, AST_NODE_NUMBER);
            node->number = parser->current_token.number;
            Parser_next_token(parser);
            break;
        case LEXER_TOKEN_LPAREN:
            Parser_next_token(parser);
            node = Parser_parse_expression(parser, 0);
            if (parser->current_token.type != LEXER_TOKEN_RPAREN) {
                critic_error("Parser_parse_primary: expected ')'");
            }
            Parser_next_token(parser); // Пропускаем ')'
            break;
        default:
            critic_error("Parser_parse_primary: unexpected token");
    }

    return node;
}

ASTNode *Parser_parse_unary(Parser *parser) {
    ASTUnaryNodeType uop;

    switch (parser->current_token.type) {
        case LEXER_TOKEN_MINUS:
            uop = AST_UNARY_NEGATIVE;
            Parser_next_token(parser); // съели '-'
            {
                ASTNode *operand = Parser_parse_unary(parser);
                ASTNode *node = ASTNodePull_pop(&parser->node_pull, AST_NODE_UNARY_OP);
                node->unary.op = uop;
                node->unary.operand = operand;
                return node;
            }
        case LEXER_TOKEN_SQRT: uop = AST_UNARY_SQRT;
            break;
        case LEXER_TOKEN_SIN: uop = AST_UNARY_SIN;
            break;
        case LEXER_TOKEN_COS: uop = AST_UNARY_COS;
            break;
        case LEXER_TOKEN_LG: uop = AST_UNARY_LG;
            break;
        case LEXER_TOKEN_LN: uop = AST_UNARY_LN;
            break;
        case LEXER_TOKEN_TAN: uop = AST_UNARY_TAN;
            break;
        case LEXER_TOKEN_COT: uop = AST_UNARY_COT;
            break;
        case LEXER_TOKEN_EXP: uop = AST_UNARY_EXP;
            break;
        case LEXER_TOKEN_GAMMA: uop = AST_UNARY_GAMMA;
            break;
        case LEXER_TOKEN_FACTORIAL: uop = AST_UNARY_FACTORIAL;
            break;
        case LEXER_TOKEN_ABS: uop = AST_UNARY_ABS;
            break;
        case LEXER_TOKEN_ARC_SIN: uop = AST_UNARY_ARC_SIN;
            break;
        case LEXER_TOKEN_ARC_COS: uop = AST_UNARY_ARC_COS;
            break;
        case LEXER_TOKEN_ARC_TAN: uop = AST_UNARY_ARC_TAN;
            break;
        case LEXER_TOKEN_ARC_COT: uop = AST_UNARY_ARC_COT;
            break;
        case LEXER_TOKEN_ARC_GAMMA: uop = AST_UNARY_ARC_GAMMA;
            break;
        case LEXER_TOKEN_ARC_FACTORIAL: uop = AST_UNARY_ARC_FACTORIAL;
            break;

        default:
            return Parser_parse_primary(parser);
    }

    // Если сюда попали — это функция, и текущий токен нам известен
    Parser_next_token(parser); // съели имя функции (sqrt, sin и т.д.)

    // Обязательно должны быть скобки!
    if (parser->current_token.type != LEXER_TOKEN_LPAREN) {
        critic_error("Parser_parse_unary: expected '(' after function (before argument)");
    }
    Parser_next_token(parser); // съели '('

    // Разбираем любое выражение внутри
    ASTNode *operand = Parser_parse_expression(parser, 0);

    if (parser->current_token.type != LEXER_TOKEN_RPAREN) {
        critic_error("Parser_parse_unary: expected ')' after function argument\n");
    }
    Parser_next_token(parser); // съели ')'

    // Строим узел
    ASTNode *node = ASTNodePull_pop(&parser->node_pull, AST_NODE_UNARY_OP);
    node->unary.op = uop;
    node->unary.operand = operand;
    return node;
}


int Parser_get_operator_precedence(const ASTBinaryNodeType type) {
    switch (type) {
        case AST_BINARY_ADD:
        case AST_BINARY_SUB: return 9;

        case AST_BINARY_MUL:
        case AST_BINARY_DIV:
        case AST_BINARY_MOD:
        case AST_BINARY_INT_DIV: return 10;

        case AST_BINARY_POWER: return 12;
        default:
            return 0;
    }
}

ASTNode *Parser_parse_expression(Parser *parser, const int min_precedence) {
    ASTNode *left = Parser_parse_unary(parser);

    while (true) {
        ASTBinaryNodeType op;
        const LexerTokenType token_type = parser->current_token.type;
        int precedence = 0;
        int is_right_assoc = 0;

        switch (token_type) {
            case LEXER_TOKEN_POWER:
                op = AST_BINARY_POWER;
                is_right_assoc = 1;
                break;
            case LEXER_TOKEN_PLUS:
                op = AST_BINARY_ADD;
                break;
            case LEXER_TOKEN_MINUS:
                op = AST_BINARY_SUB;
                break;
            case LEXER_TOKEN_MUL:
                op = AST_BINARY_MUL;
                break;
            case LEXER_TOKEN_DIV:
                op = AST_BINARY_DIV;
                break;
            case LEXER_TOKEN_INT_DIV:
                op = AST_BINARY_INT_DIV;
                break;
            case LEXER_TOKEN_MOD:
                op = AST_BINARY_MOD;
                break;
            default:
                return left;
        }

        precedence = Parser_get_operator_precedence(op);
        if (precedence < min_precedence) break;

        Parser_next_token(parser);

        const int next_min_precedence = is_right_assoc ? precedence : precedence + 1;
        ASTNode *right = Parser_parse_expression(parser, next_min_precedence);

        ASTNode *binary_node = ASTNodePull_pop(&parser->node_pull, AST_NODE_BINARY_OP);

        binary_node->binary.left = left;
        binary_node->binary.right = right;
        binary_node->binary.op = op;

        left = binary_node;
    }

    return left;
}

void indent_print(const int level) {
    for (int i = 0; i < level; ++i) printf("|    ");
}

void ASTNode_print(const ASTNode *node, const int indent) {
    if (node == NULL) return;

    indent_print(indent);
    const char *node_type_text = ASTNodeType_to_string(node->type);

    switch (node->type) {
        case AST_NODE_NUMBER:
            printf("%s: %f\n", node_type_text, node->number);
            break;
        case AST_NODE_UNARY_OP:
            const char *unary_operator_type_text = ASTUnaryNodeType_to_string(node->unary.op);
            printf("%s: (%s):\n", node_type_text, unary_operator_type_text);
            indent_print(indent + 1);
            printf("Operand:\n");
            ASTNode_print(node->unary.operand, indent + 2);
            break;
        case AST_NODE_BINARY_OP:
            const char *binary_operator_type_text = ASTBinaryNodeType_to_string(node->binary.op);
            printf("%s (%s):\n", node_type_text, binary_operator_type_text);
            indent_print(indent + 1);
            printf("Left:\n");
            ASTNode_print(node->binary.left, indent + 2);
            indent_print(indent + 1);
            printf("Right:\n");
            ASTNode_print(node->binary.right, indent + 2);
            break;
        default:
            printf("%s", node_type_text);
            break;
    }
}

double arccot(double x) {
    if (x == 0.0) {
        return M_PI_2; // π/2
    }
    double atan_val = atan(1.0 / x);
    return (x > 0) ? atan_val : (M_PI + atan_val);
}

// Производная гамма-функции через дигамма-функцию
double dgamma(double x) {
    return tgamma(x) * (x - 1.0) * (1.0 - x); // Упрощенная аппроксимация
}

double arcgamma(double x, bool *success) {
    if (x < 0.0 || isnan(x)) {
        *success = false;
        return NAN;
    }

    // Начальное приближение для больших x
    double y = (x > 10.0) ? log(x) * 2.0 : 2.0;
    int iter = 0;

    do {
        double gamma_val = tgamma(y);
        double delta = gamma_val - x;

        if (fabs(delta) < GAMMA_EPSILON) {
            *success = true;
            return y;
        }

        // Производная через дигамма-функцию (аппроксимация)
        double derivative = gamma_val * (log(y) - 0.5 / y);
        if (fabs(derivative) < DBL_EPSILON) break;

        y -= delta / derivative;

        // Ограничения области поиска
        y = fmax(y, 1e-3);
    } while (++iter < MAX_ITERATIONS);

    *success = false;
    return NAN;
}

double arcfactorial(double x, bool *success) {
    bool gamma_success = true;
    const double gamma_x = arcgamma(x, &gamma_success);
    if (!gamma_success) {
        *success = false;
        return NAN;
    }

    return gamma_x - 1.0f;
}

double ASTNode_evaluate(const ASTNode *node) {
    switch (node->type) {
        case AST_NODE_NUMBER:
            return node->number;
        case AST_NODE_UNARY_OP:
            const double v = ASTNode_evaluate(node->unary.operand);
            switch (node->unary.op) {
                case AST_UNARY_NEGATIVE: return -v;
                case AST_UNARY_SQRT: return sqrt(v);
                case AST_UNARY_SIN: return sin(v);
                case AST_UNARY_COS: return cos(v);
                case AST_UNARY_LG: return log10(v);
                case AST_UNARY_LN: return log(v);
                case AST_UNARY_TAN: return tan(v);
                case AST_UNARY_COT: {
                    double sin_v = sin(v);
                    if (fabs(sin_v) < DBL_EPSILON) {
                        fprintf(stderr, "cot(%g) is undefined\n", v);
                        return NAN;
                    }
                    return cos(v) / sin_v;
                }
                case AST_UNARY_ABS: return fabs(v);
                case AST_UNARY_GAMMA: return tgamma(v);
                case AST_UNARY_FACTORIAL: return tgamma(v + 1.0f);
                case AST_UNARY_EXP: return exp(v);
                case AST_UNARY_ARC_SIN: return asin(v);
                case AST_UNARY_ARC_COS: return acos(v);
                case AST_UNARY_ARC_TAN: return atan(v);
                case AST_UNARY_ARC_COT: return arccot(v);
                case AST_UNARY_ARC_GAMMA: {
                    bool success;
                    const double result = arcgamma(v, &success);
                    if (!success) {
                        critic_error("ASTNode_evaluate: arcgamma error");
                    }
                    return result;
                }
                case AST_UNARY_ARC_FACTORIAL: {
                    bool success = true;
                    const double result = arcfactorial(v, &success);
                    if (!success) {
                        critic_error("ASTNode_evaluate: arcfactorial error");
                    }
                    return result;
                }
                default:
                    critic_error("ASTNode_evaluate: invalid unary node type");
            }
        case AST_NODE_BINARY_OP:
            const double L = ASTNode_evaluate(node->binary.left);
            const double R = ASTNode_evaluate(node->binary.right);
            switch (node->binary.op) {
                case AST_BINARY_ADD: return L + R;
                case AST_BINARY_SUB: return L - R;
                case AST_BINARY_MUL: return L * R;
                case AST_BINARY_DIV: return L / R;
                case AST_BINARY_INT_DIV: return floor(L / R);
                case AST_BINARY_MOD: return fmod(L, R);
                case AST_BINARY_POWER: return pow(L, R);
                case AST_BINARY_LOG:
                    return log(R) / log(L);
                default:
                    critic_error("ASTNode_evaluate: invalid binary node type");
            }
        default:
            critic_error("ASTNode_evaluate: invalid node type");
    }
}

void input(char *buffer, const int buffer_size, const char *prompt) {
    printf("%s", prompt);
    if (fgets(buffer, buffer_size, stdin) == NULL) {
        critic_error("ASTNode_evaluate: input failed");
    }
    buffer[strcspn(buffer, "\n")] = '\0'; // Удаление '\n'
    if (strlen(buffer) == buffer_size - 1)
        while (getchar() != '\n') {
        }
}

ASTNode *Parser_parse(Parser *parser) {
    return Parser_parse_expression(parser, 0);
}

void build_expression_from_argv(char *buffer, size_t buffer_size,
                                const int argc, const char **argv) {
    if (buffer == NULL || buffer_size == 0 || argc == 0 || argv == NULL) {
        if (buffer_size > 0 && buffer != NULL) {
            buffer[0] = '\0';
        }
        return;
    }

    size_t pos = 0; // Текущая позиция записи в буфер

    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];
        size_t arg_len = strlen(arg);

        // Проверяем, нужно ли добавить пробел после аргумента
        int add_space = (i != argc - 1);
        size_t required_space = arg_len + (add_space ? 1 : 0);

        // Проверяем, не превысит ли это размер буфера
        if (pos + required_space >= buffer_size) {
            // Если аргумент не влезает даже частично
            if (pos == 0 && arg_len >= buffer_size) {
                strncpy(buffer, arg, buffer_size - 1);
                pos = buffer_size - 1;
            }
            break;
        }

        // Копируем аргумент в буфер
        strncpy(buffer + pos, arg, arg_len);
        pos += arg_len;

        // Добавляем пробел (если требуется)
        if (add_space) {
            buffer[pos] = ' ';
            pos++;
        }
    }

    // Гарантированно завершаем строку
    buffer[(pos < buffer_size) ? pos : buffer_size - 1] = '\0';
}

int main(const int argc, const char **argv) {
    static char input_expression[1024] = "";
    if (argc > 1) {
        build_expression_from_argv(input_expression, sizeof(input_expression), argc - 1, argv + 1);
    }

    Lexer lexer = Lexer_init(input_expression);
    Parser parser = Parser_init(&lexer, 64);
    ASTNode *expression = NULL;

    if (argc > 1) {
        expression = Parser_parse(&parser);
        const double value = ASTNode_evaluate(expression);
        printf("%f\n", value);
        ASTNodePull_push(&parser.node_pull, expression);
        return EXIT_SUCCESS;
    }

    while (true) {
        input(input_expression, sizeof(input_expression), ">>> ");
        lexer = Lexer_init(input_expression);
        Parser_reset(&parser, &lexer);

        expression = Parser_parse(&parser);
        const double value = ASTNode_evaluate(expression);

        printf("%f\n", value);
        ASTNodePull_push(&parser.node_pull, expression);
    }
}
