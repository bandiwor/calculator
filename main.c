#include <ctype.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #define CALC_REMOVE_DEBUG_INFO // Comment Line if not debug

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif
#ifndef M_GAMMA
/** Euler's constant in high precision */
#define M_GAMMA 0.5772156649015328606065120900824024
#endif
#define HASH_TABLE_SIZE 1024
#define MAX_VAR_NAME_LEN 256
#define HASH_SEED 0

double Kncoe[] = {.30459198558715155634315638246624251,
                  .72037977439182833573548891941219706,
                  -.12454959243861367729528855995001087,
                  .27769457331927827002810119567456810e-1,
                  -.67762371439822456447373550186163070e-2,
                  .17238755142247705209823876688592170e-2,
                  -.44817699064252933515310345718960928e-3,
                  .11793660000155572716272710617753373e-3,
                  -.31253894280980134452125172274246963e-4,
                  .83173997012173283398932708991137488e-5,
                  -.22191427643780045431149221890172210e-5,
                  .59302266729329346291029599913617915e-6,
                  -.15863051191470655433559920279603632e-6,
                  .42459203983193603241777510648681429e-7,
                  -.11369129616951114238848106591780146e-7,
                  .304502217295931698401459168423403510e-8,
                  -.81568455080753152802915013641723686e-9,
                  .21852324749975455125936715817306383e-9,
                  -.58546491441689515680751900276454407e-10,
                  .15686348450871204869813586459513648e-10,
                  -.42029496273143231373796179302482033e-11,
                  .11261435719264907097227520956710754e-11,
                  -.30174353636860279765375177200637590e-12,
                  .80850955256389526647406571868193768e-13,
                  -.21663779809421233144009565199997351e-13,
                  .58047634271339391495076374966835526e-14,
                  -.15553767189204733561108869588173845e-14,
                  .41676108598040807753707828039353330e-15,
                  -.11167065064221317094734023242188463e-15};

#define loop while (1)

// Точность и лимиты
#define GAMMA_EPSILON 1e-12
#define MAX_ITERATIONS 1000
#define STIRLING_COEFF (4.44288293816) // sqrt(2*M_PI)

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

#define LEXER_TOKEN_MAP(X)                                                     \
  X(LEXER_TOKEN_EOF, "LEXER_TOKEN_EOF")                                        \
  X(LEXER_TOKEN_UNKNOWN, "LEXER_TOKEN_UNKNOWN")                                \
  X(LEXER_TOKEN_NUMBER, "number")                                              \
  X(LEXER_TOKEN_IDENTIFIER, "identifier")                                      \
  X(LEXER_TOKEN_LPAREN, "(")                                                   \
  X(LEXER_TOKEN_RPAREN, ")")                                                   \
  X(LEXER_TOKEN_ASSIGN, "=")                                                   \
  X(LEXER_TOKEN_PLUS, "+")                                                     \
  X(LEXER_TOKEN_MINUS, "-")                                                    \
  X(LEXER_TOKEN_MUL, "*")                                                      \
  X(LEXER_TOKEN_DIV, "/")                                                      \
  X(LEXER_TOKEN_INT_DIV, "//")                                                 \
  X(LEXER_TOKEN_MOD, "%")                                                      \
  X(LEXER_TOKEN_SQRT, "sqrt")                                                  \
  X(LEXER_TOKEN_POWER, "**")                                                   \
  X(LEXER_TOKEN_LG, "lg")                                                      \
  X(LEXER_TOKEN_LN, "ln")                                                      \
  X(LEXER_TOKEN_LOG, "log")                                                    \
  X(LEXER_TOKEN_ARC_SIN, "arcsin")                                             \
  X(LEXER_TOKEN_SIN, "sin")                                                    \
  X(LEXER_TOKEN_ARC_COS, "arccos")                                             \
  X(LEXER_TOKEN_COS, "cos")                                                    \
  X(LEXER_TOKEN_ARC_TAN, "arctan")                                             \
  X(LEXER_TOKEN_TAN, "tan")                                                    \
  X(LEXER_TOKEN_ARC_COT, "arccot")                                             \
  X(LEXER_TOKEN_COT, "cot")                                                    \
  X(LEXER_TOKEN_ABS, "abs")                                                    \
  X(LEXER_TOKEN_EXP, "exp")                                                    \
  X(LEXER_TOKEN_ARC_GAMMA, "arcgamma")                                         \
  X(LEXER_TOKEN_GAMMA, "gamma")                                                \
  X(LEXER_TOKEN_ARC_FACTORIAL, "arcfactorial")                                 \
  X(LEXER_TOKEN_FACTORIAL, "factorial")

#define AST_NODE_TYPE_MAP(X)                                                   \
  X(AST_NODE_UNKNOWN, "AST_NODE_UNKNOWN")                                      \
  X(AST_NODE_NUMBER, "number")                                                 \
  X(AST_NODE_VAR_REF, "var reference")                                         \
  X(AST_NODE_BINARY_OP, "binary op")                                           \
  X(AST_NODE_UNARY_OP, "unary op")

#define AST_UNARY_NODE_TYPE_MAP(X)                                             \
  X(AST_UNARY_UNKNOWN, "AST_UNARY_UNKNOWN")                                    \
  X(AST_UNARY_NEGATIVE, "negative")                                            \
  X(AST_UNARY_ABS, "abs")                                                      \
  X(AST_UNARY_SQRT, "sqrt")                                                    \
  X(AST_UNARY_LG, "lg")                                                        \
  X(AST_UNARY_ARC_SIN, "asin")                                                 \
  X(AST_UNARY_SIN, "sin")                                                      \
  X(AST_UNARY_ARC_COS, "acos")                                                 \
  X(AST_UNARY_COS, "cos")                                                      \
  X(AST_UNARY_ARC_TAN, "atan")                                                 \
  X(AST_UNARY_TAN, "tan")                                                      \
  X(AST_UNARY_ARC_COT, "cot")                                                  \
  X(AST_UNARY_COT, "cot")                                                      \
  X(AST_UNARY_LN, "ln")                                                        \
  X(AST_UNARY_EXP, "exp")                                                      \
  X(AST_UNARY_ARC_GAMMA, "arcgamma")                                           \
  X(AST_UNARY_GAMMA, "gamma")                                                  \
  X(AST_UNARY_ARC_FACTORIAL, "arcfactorial")                                   \
  X(AST_UNARY_FACTORIAL, "factorial")

#define AST_BINARY_NODE_TYPE_MAP(X)                                            \
  X(AST_BINARY_UNKNOWN, "AST_BINARY_UNKNOWN")                                  \
  X(AST_BINARY_ASSIGN, "assign")                                               \
  X(AST_BINARY_ADD, "add")                                                     \
  X(AST_BINARY_SUB, "sub")                                                     \
  X(AST_BINARY_MUL, "mul")                                                     \
  X(AST_BINARY_DIV, "div")                                                     \
  X(AST_BINARY_INT_DIV, "int div")                                             \
  X(AST_BINARY_MOD, "mod")                                                     \
  X(AST_BINARY_POWER, "power")                                                 \
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
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *token_type_string[] = {
#define X(name, value) [name] = (value),
      LEXER_TOKEN_MAP(X)
#undef X
  };

  if (token_type >= LEXER_TOKEN_COUNT) {
    return token_type_string[LEXER_TOKEN_UNKNOWN];
  }

  return token_type_string[token_type];
#else
  return "";
#endif
}

const char *ASTNodeType_to_string(const ASTNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
      AST_NODE_TYPE_MAP(X)
#undef X
  };
  if (node_type >= AST_NODE_COUNT) {
    return node_type_string[AST_NODE_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}

const char *ASTBinaryNodeType_to_string(const ASTBinaryNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
      AST_BINARY_NODE_TYPE_MAP(X)
#undef X
  };

  if (node_type >= AST_BINARY_NODE_COUNT) {
    return node_type_string[AST_BINARY_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}

const char *ASTUnaryNodeType_to_string(const ASTUnaryNodeType node_type) {
#ifndef CALC_REMOVE_DEBUG_INFO
  static const char *node_type_string[] = {
#define X(name, value) [name] = (value),
      AST_UNARY_NODE_TYPE_MAP(X)
#undef X
  };

  if (node_type >= AST_UNARY_NODE_COUNT) {
    return node_type_string[AST_UNARY_UNKNOWN];
  }

  return node_type_string[node_type];
#else
  return "";
#endif
}

uint64_t murmur3_64(const void *key, const size_t len, const uint64_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;
  uint64_t h = seed ^ (len * m);

  const uint64_t *data = key;
  const uint64_t *end = data + (len / 8);

  while (data != end) {
    uint64_t k = *data++;
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
  }

  const unsigned char *data2 = (const unsigned char *)data;
  switch (len & 7) {
  case 7:
    h ^= (uint64_t)data2[6] << 48;
  case 6:
    h ^= (uint64_t)data2[5] << 40;
  case 5:
    h ^= (uint64_t)data2[4] << 32;
  case 4:
    h ^= (uint64_t)data2[3] << 24;
  case 3:
    h ^= (uint64_t)data2[2] << 16;
  case 2:
    h ^= (uint64_t)data2[1] << 8;
  case 1:
    h ^= (uint64_t)data2[0];
    h *= m;
  default:
    break;
  };

  h ^= h >> r;
  h *= m;
  h ^= h >> r;
  return h;
}

typedef struct tag_HashNode {
  char *key;
  uint64_t hash;
  double value;
  struct tag_HashNode *next;
} HashNode;

typedef struct {
  HashNode **buckets;
  size_t size;
} HashTable;

static HashTable *g_hash_table;

HashTable *HashTable_create() {
  HashTable *ht = malloc(sizeof(HashTable));
  if (ht == NULL) {
    critic_error("HashTable_create(): memory allocation failed");
    exit(EXIT_FAILURE);
  }
  ht->size = HASH_TABLE_SIZE;
  ht->buckets = calloc(ht->size, sizeof(HashNode *));
  if (ht->buckets == NULL) {
    critic_error("HashTable_create(): memory allocation failed");
    exit(EXIT_FAILURE);
  }
  return ht;
}

void HashTable_set(const HashTable *ht, const char *key, const double value) {
  const uint64_t hash = murmur3_64(key, strlen(key), HASH_SEED);
  const size_t bucket_idx = hash % ht->size;

  HashNode *node = ht->buckets[bucket_idx];
  while (node != NULL) {
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      node->value = value;
      return;
    }
    node = node->next;
  }

  HashNode *new_node = malloc(sizeof(HashNode));
  new_node->key = strdup(key);
  new_node->hash = hash;
  new_node->value = value;
  new_node->next = ht->buckets[bucket_idx];
  ht->buckets[bucket_idx] = new_node;
}

double *HashTable_get(const HashTable *ht, const char *key) {
  const uint64_t hash = murmur3_64(key, strlen(key), 0);
  const size_t bucket_idx = hash % ht->size;

  HashNode *node = ht->buckets[bucket_idx];
  while (node) {
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      return &node->value;
    }
    node = node->next;
  }
  return NULL;
}

void HashTable_remove(const HashTable *ht, const char *key) {
  const uint64_t hash = murmur3_64(key, strlen(key), 0);
  const size_t bucket_idx = hash % ht->size;

  HashNode **prev_ptr = &ht->buckets[bucket_idx];
  HashNode *current = ht->buckets[bucket_idx];

  while (current) {
    if (current->hash == hash && strcmp(current->key, key) == 0) {
      *prev_ptr = current->next;
      free(current->key);
      free(current);
      return;
    }
    prev_ptr = &current->next;
    current = current->next;
  }
}

void HashTable_destroy(HashTable *ht) {
  for (size_t i = 0; i < ht->size; i++) {
    HashNode *node = ht->buckets[i];
    while (node) {
      HashNode *next = node->next;
      free(node->key);
      free(node);
      node = next;
    }
  }
  free(ht->buckets);
  free(ht);
}

typedef struct {
  LexerTokenType type;

  union {
    double number;
    char *identifier;
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

  return (Lexer){
      .source = source,
      .current_char = length > 0 ? source[0] : '\0',
      .current_position = 0,
      .length = length,
  };
}

LexerToken LexerToken_create_identifier(const char *source) {
  LexerToken token = {
      .type = LEXER_TOKEN_IDENTIFIER,
  };
  token.identifier = strdup(source);
  if (token.identifier == NULL) {
    critic_error("LexerToken_create_identifier(): memory allocation failed");
  }
  return token;
}

void LexerToken_free(const LexerToken *token) {
  if (token == NULL) {
    return;
  }
  if (token->type == LEXER_TOKEN_IDENTIFIER) {
    free(token->identifier);
  }
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

int is_digit(const int ch) { return ch >= '0' && ch <= '9'; }

LexerToken Lexer_parse_number(Lexer *lexer) {
  static char buffer[128];
#define MAX_BUFFER_SIZE (sizeof(buffer) - 1)

  buffer[0] = lexer->current_char;
  size_t buffer_length = 1;
  Lexer_advance(lexer);
  bool is_int = true;

  while (buffer_length < MAX_BUFFER_SIZE) {
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

  return (LexerToken){.type = LEXER_TOKEN_NUMBER,
                      .number = strtod(buffer, NULL)};
#undef MAX_BUFFER_SIZE
}

LexerToken Lexer_parse_identifier(Lexer *lexer) {
  static char buffer[16];
#define MAX_BUFFER_SIZE (sizeof(buffer) - 1)

  buffer[0] = lexer->current_char;
  size_t buffer_length = 1;
  Lexer_advance(lexer);

  while (isalnum(lexer->current_char) && buffer_length < MAX_BUFFER_SIZE) {
    buffer[buffer_length++] = lexer->current_char;
    Lexer_advance(lexer);
  }

  buffer[buffer_length] = '\0';

#define IDENTIFIER_COMPARE(name, token_type)                                   \
  if (strcasecmp(buffer, name) == 0)                                           \
    return (LexerToken){.type = (token_type)};

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

  if (strcasecmp(buffer, "pi") == 0)
    return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_PI};
  if (strcasecmp(buffer, "p") == 0)
    return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_PI};
  if (strcasecmp(buffer, "e") == 0)
    return (LexerToken){.type = LEXER_TOKEN_NUMBER, .number = M_E};

  return (LexerToken){.type = LEXER_TOKEN_IDENTIFIER,
                      .identifier = strdup(buffer)};
#undef MAX_BUFFER_SIZE
}

char Lexer_next_char(const Lexer *lexer) {
  if (lexer->current_position + 1 < lexer->length) {
    return lexer->source[lexer->current_position + 1];
  }
  return '\0';
}

int is_space(const int ch) {
  return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t' || ch == '\f' ||
         ch == '\v';
}

LexerToken Lexer_next_token(Lexer *lexer) {
  if (lexer == NULL)
    return (LexerToken){.type = LEXER_TOKEN_EOF};
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
  case '=':
    type = LEXER_TOKEN_ASSIGN;
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

    struct {
      char *name;
    } var_ref;
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
  if (node_pull == NULL)
    return;
  free(node_pull->nodes);
}

void ASTNode_allocate(ASTNode **node, const ASTNodeType type) {
  *node = (ASTNode *)malloc(sizeof(ASTNode));
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
  if (node_pull == NULL || node_pull->reserved >= node_pull->capacity)
    return NULL;
  for (size_t i = 0; i < node_pull->capacity; i++) {
    if (node_pull->nodes[i].used == AST_NODE_USED_FROM_FREE) {
      ASTNode *node = node_pull->nodes + i;
      return node;
    }
  }
  return NULL;
}

ASTNode *ASTNodePull_pop(const ASTNodePull *node_pull,
                         const ASTNodeType node_type) {
  if (node_pull == NULL)
    return NULL;

  if (node_pull->reserved >= node_pull->capacity) {
    ASTNode *node;
    ASTNode_allocate(&node, node_type);
    node->used = AST_NODE_USED_FROM_MALLOC;
    return node;
  }

  ASTNode *node = ASTNodePull_find_free(node_pull);
  if (node == NULL)
    return NULL;

  node->used = AST_NODE_USED_FROM_PULL;
  node->type = node_type;
  return node;
}

void ASTNodePull_push(ASTNodePull *node_pull, ASTNode *node) {
  if (node_pull == NULL)
    return;

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
  if (parser == NULL)
    return;

  parser->lexer = lexer;
  Lexer_reset(parser->lexer);
  parser->current_token = Lexer_next_token(lexer);
}

void Parser_next_token(Parser *parser) {
  LexerToken_free(&parser->current_token);
  parser->current_token = Lexer_next_token(parser->lexer);
}

ASTNode *Parser_parse_expression(Parser *parser, int min_precedence);

ASTNode *Parser_parse_primary(Parser *parser) {
  ASTNode *node = NULL;
  switch (parser->current_token.type) {
  case LEXER_TOKEN_IDENTIFIER:
    node = ASTNodePull_pop(&parser->node_pull, AST_NODE_VAR_REF);
    node->var_ref.name = strdup(parser->current_token.identifier);
    Parser_next_token(parser);
    break;
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
    Parser_next_token(parser);
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
    Parser_next_token(parser);
    {
      ASTNode *operand = Parser_parse_unary(parser);
      ASTNode *node = ASTNodePull_pop(&parser->node_pull, AST_NODE_UNARY_OP);
      node->unary.op = uop;
      node->unary.operand = operand;
      return node;
    }
  case LEXER_TOKEN_SQRT:
    uop = AST_UNARY_SQRT;
    break;
  case LEXER_TOKEN_SIN:
    uop = AST_UNARY_SIN;
    break;
  case LEXER_TOKEN_COS:
    uop = AST_UNARY_COS;
    break;
  case LEXER_TOKEN_LG:
    uop = AST_UNARY_LG;
    break;
  case LEXER_TOKEN_LN:
    uop = AST_UNARY_LN;
    break;
  case LEXER_TOKEN_TAN:
    uop = AST_UNARY_TAN;
    break;
  case LEXER_TOKEN_COT:
    uop = AST_UNARY_COT;
    break;
  case LEXER_TOKEN_EXP:
    uop = AST_UNARY_EXP;
    break;
  case LEXER_TOKEN_GAMMA:
    uop = AST_UNARY_GAMMA;
    break;
  case LEXER_TOKEN_FACTORIAL:
    uop = AST_UNARY_FACTORIAL;
    break;
  case LEXER_TOKEN_ABS:
    uop = AST_UNARY_ABS;
    break;
  case LEXER_TOKEN_ARC_SIN:
    uop = AST_UNARY_ARC_SIN;
    break;
  case LEXER_TOKEN_ARC_COS:
    uop = AST_UNARY_ARC_COS;
    break;
  case LEXER_TOKEN_ARC_TAN:
    uop = AST_UNARY_ARC_TAN;
    break;
  case LEXER_TOKEN_ARC_COT:
    uop = AST_UNARY_ARC_COT;
    break;
  case LEXER_TOKEN_ARC_GAMMA:
    uop = AST_UNARY_ARC_GAMMA;
    break;
  case LEXER_TOKEN_ARC_FACTORIAL:
    uop = AST_UNARY_ARC_FACTORIAL;
    break;
  default:
    return Parser_parse_primary(parser);
  }

  Parser_next_token(parser);

  if (parser->current_token.type != LEXER_TOKEN_LPAREN) {
    critic_error(
        "Parser_parse_unary: expected '(' after function (before argument)");
  }
  Parser_next_token(parser);

  ASTNode *operand = Parser_parse_expression(parser, 0);

  if (parser->current_token.type != LEXER_TOKEN_RPAREN) {
    critic_error("Parser_parse_unary: expected ')' after function argument\n");
  }
  Parser_next_token(parser);

  ASTNode *node = ASTNodePull_pop(&parser->node_pull, AST_NODE_UNARY_OP);
  node->unary.op = uop;
  node->unary.operand = operand;
  return node;
}

int Parser_get_operator_precedence(const ASTBinaryNodeType type) {
  switch (type) {
  case AST_BINARY_ASSIGN:
    return 0;
  case AST_BINARY_ADD:
  case AST_BINARY_SUB:
    return 9;

  case AST_BINARY_MUL:
  case AST_BINARY_DIV:
  case AST_BINARY_MOD:
  case AST_BINARY_INT_DIV:
    return 10;

  case AST_BINARY_POWER:
    return 12;
  default:
    return 0;
  }
}

ASTNode *Parser_parse_expression(Parser *parser, const int min_precedence) {
  ASTNode *left = Parser_parse_unary(parser);

  loop {
    ASTBinaryNodeType op;
    const LexerTokenType token_type = parser->current_token.type;
    int precedence = 0;
    int is_right_assoc = 0;

    switch (token_type) {
    case LEXER_TOKEN_POWER:
      op = AST_BINARY_POWER;
      is_right_assoc = 1;
      break;
    case LEXER_TOKEN_ASSIGN:
      op = AST_BINARY_ASSIGN;
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
    if (precedence < min_precedence)
      break;

    Parser_next_token(parser);

    const int next_min_precedence =
        is_right_assoc ? precedence : precedence + 1;
    ASTNode *right = Parser_parse_expression(parser, next_min_precedence);

    ASTNode *binary_node =
        ASTNodePull_pop(&parser->node_pull, AST_NODE_BINARY_OP);

    binary_node->binary.left = left;
    binary_node->binary.right = right;
    binary_node->binary.op = op;

    left = binary_node;
  }

  return left;
}

void indent_print(const int level) {
#ifndef CALC_REMOVE_DEBUG_INFO
  for (int i = 0; i < level; ++i)
    printf("|    ");
#endif
}

void ASTNode_print(const ASTNode *node, const int indent) {
#ifndef CALC_REMOVE_DEBUG_INFO
  if (node == NULL)
    return;

  indent_print(indent);
  const char *node_type_text = ASTNodeType_to_string(node->type);

  switch (node->type) {
  case AST_NODE_NUMBER:
    printf("%s: %f\n", node_type_text, node->number);
    break;
  case AST_NODE_UNARY_OP:
    const char *unary_operator_type_text =
        ASTUnaryNodeType_to_string(node->unary.op);
    printf("%s: (%s):\n", node_type_text, unary_operator_type_text);
    indent_print(indent + 1);
    printf("Operand:\n");
    ASTNode_print(node->unary.operand, indent + 2);
    break;
  case AST_NODE_BINARY_OP:
    const char *binary_operator_type_text =
        ASTBinaryNodeType_to_string(node->binary.op);
    printf("%s (%s):\n", node_type_text, binary_operator_type_text);
    indent_print(indent + 1);
    printf("Left:\n");
    ASTNode_print(node->binary.left, indent + 2);
    indent_print(indent + 1);
    printf("Right:\n");
    ASTNode_print(node->binary.right, indent + 2);
    break;
  case AST_NODE_VAR_REF:
    printf("%s: (%s):\n", node_type_text, node->var_ref.name);
    indent_print(indent + 1);
    printf("Value:\n");
    indent_print(indent + 2);
    printf("<value>\n");
    break;
  default:
    printf("%s", node_type_text);
    break;
  }
#endif
}

double arccot(const double x) {
  if (x == 0.0) {
    return M_PI_2;
  }
  const double atan_val = atan(1.0 / x);
  return (x > 0) ? atan_val : (M_PI + atan_val);
}

double digamma(double x) {
  if (x < 0.0)
    return digamma(1.0 - x) +
           M_PI / tan(M_PI * (1.0 - x)); /* reflection formula */
  if (x < 1.0)
    return digamma(1.0 + x) - 1.0 / x;
  if (x == 1.0)
    return -M_GAMMA;
  if (x == 2.0)
    return 1.0 - M_GAMMA;
  if (x == 3.0)
    return 1.5 - M_GAMMA;
  if (x > 3.0)
    return 0.5 * (digamma(x / 2.0) + digamma((x + 1.0) / 2.0)) + M_LN2;
  register double Tn_1 = 1.0;   /* T_{n-1}(x), started at n=1 */
  register double Tn = x - 2.0; /* T_{n}(x) , started at n=1 */
  register double resul = Kncoe[0] + Kncoe[1] * Tn;
  x -= 2.0;

  for (int n = 2; n < sizeof(Kncoe) / sizeof(double); ++n) {
    const double Tn1 = 2.0 * x * Tn - Tn_1;
    resul += Kncoe[n] * Tn1;
    Tn_1 = Tn;
    Tn = Tn1;
  }

  return resul;
}

double gamma_initial_guess(const double x) {
  if (x > 15.0) {
    // Решение уравнения Стирлинга: x ≈ y^y / e^y * sqrt(y)
    const double y = log(x) / (log(log(x)) - 1.0);
    return fmax(y, 1.461632); // Минимум гамма-функции (~1.461632)
  }
  if (x >= 1.0)
    return 2.0;
  return (x < 0.8856) ? 0.5 : 1.5; // gamma(0.5)=1.77245, gamma(1.5)=0.886227
}

double dgamma(const double x) { return tgamma(x) * digamma(x); }

double arcgamma(double x, bool *success) {
  *success = true;
  if (x < 0.0 || isnan(x)) {
    *success = false;
    return NAN;
  }
  if (x == 0.0) { // gamma(y) → ∞ при y → 0+
    *success = false;
    return NAN;
  }

  double y = gamma_initial_guess(x);
  int iter = 0;
  double prev_y = y;

  do {
    const double gamma_val = tgamma(y);
    const double delta = gamma_val - x;

    if (fabs(delta) < GAMMA_EPSILON * fabs(x)) {
      return y;
    }

    if (y <= 0.0 || isinf(gamma_val)) {
      *success = false;
      return NAN;
    }

    const double derivative = dgamma(y);
    if (fabs(derivative) < DBL_EPSILON)
      break;

    const double step = delta / derivative;
    y = fmax(y - fmax(fmin(step, 0.5 * y), -0.5 * y), 1e-4);

    if (fabs(y - prev_y) < 1e-15 * fabs(y))
      break;
    prev_y = y;
  } while (++iter < MAX_ITERATIONS);

  *success = (fabs(tgamma(y) - x) < GAMMA_EPSILON);
  return *success ? y : NAN;
}

double arcfactorial(const double x, bool *success) {
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
  case AST_NODE_VAR_REF:
    const double *value_p = HashTable_get(g_hash_table, node->var_ref.name);
    if (value_p == NULL) {
      critic_error("Variable not found");
      exit(EXIT_FAILURE);
    }
    return *value_p;
  case AST_NODE_UNARY_OP:
    const double v = ASTNode_evaluate(node->unary.operand);
    switch (node->unary.op) {
    case AST_UNARY_NEGATIVE:
      return -v;
    case AST_UNARY_SQRT:
      return sqrt(v);
    case AST_UNARY_SIN:
      return sin(v);
    case AST_UNARY_COS:
      return cos(v);
    case AST_UNARY_LG:
      return log10(v);
    case AST_UNARY_LN:
      return log(v);
    case AST_UNARY_TAN:
      return tan(v);
    case AST_UNARY_COT: {
      const double sin_v = sin(v);
      if (fabs(sin_v) < DBL_EPSILON) {
        fprintf(stderr, "cot(%g) is undefined\n", v);
        return NAN;
      }
      return cos(v) / sin_v;
    }
    case AST_UNARY_ABS:
      return fabs(v);
    case AST_UNARY_GAMMA:
      return tgamma(v);
    case AST_UNARY_FACTORIAL:
      return tgamma(v + 1.0f);
    case AST_UNARY_EXP:
      return exp(v);
    case AST_UNARY_ARC_SIN:
      return asin(v);
    case AST_UNARY_ARC_COS:
      return acos(v);
    case AST_UNARY_ARC_TAN:
      return atan(v);
    case AST_UNARY_ARC_COT:
      return arccot(v);
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
    const double R = ASTNode_evaluate(node->binary.right);
    if (node->binary.op == AST_BINARY_ASSIGN) {
      if (node->binary.left->type != AST_NODE_VAR_REF) {
        critic_error("ASTNode_evaluate: can assign only to var");
        exit(EXIT_FAILURE);
      }
      HashTable_set(g_hash_table, node->binary.left->var_ref.name, R);
      return R;
    }

    const double L = ASTNode_evaluate(node->binary.left);
    switch (node->binary.op) {
    case AST_BINARY_ADD:
      return L + R;
    case AST_BINARY_SUB:
      return L - R;
    case AST_BINARY_MUL:
      return L * R;
    case AST_BINARY_DIV:
      return L / R;
    case AST_BINARY_INT_DIV:
      return floor(L / R);
    case AST_BINARY_MOD:
      return fmod(L, R);
    case AST_BINARY_POWER:
      return pow(L, R);
    case AST_BINARY_LOG:
      return log(R) / log(L);
    default:
      critic_error("ASTNode_evaluate: invalid binary node type");
    }
  default:
    critic_error("ASTNode_evaluate: invalid node type");
  }
  return 0;
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

void build_expression_from_argv(char *buffer, const size_t buffer_size,
                                const int argc, const char **argv) {
  if (buffer == NULL || buffer_size == 0 || argc == 0 || argv == NULL) {
    if (buffer_size > 0 && buffer != NULL) {
      buffer[0] = '\0';
    }
    return;
  }

  size_t pos = 0;

  for (int i = 0; i < argc; i++) {
    const char *arg = argv[i];
    const size_t arg_len = strlen(arg);
    const int add_space = (i != argc - 1);
    const size_t required_space = arg_len + (add_space ? 1 : 0);

    if (pos + required_space >= buffer_size) {
      if (pos == 0 && arg_len >= buffer_size) {
        strncpy(buffer, arg, buffer_size - 1);
        pos = buffer_size - 1;
      }
      break;
    }

    strncpy(buffer + pos, arg, arg_len);
    pos += arg_len;

    if (add_space) {
      buffer[pos] = ' ';
      pos++;
    }
  }

  buffer[(pos < buffer_size) ? pos : buffer_size - 1] = '\0';
}

int main(const int argc, const char **argv) {
  g_hash_table = HashTable_create();

  static char input_expression[1024] = "";
  if (argc > 1) {
    build_expression_from_argv(input_expression, sizeof(input_expression),
                               argc - 1, argv + 1);
  }

  Lexer lexer = Lexer_init(input_expression);
  Parser parser = Parser_init(&lexer, 64);
  ASTNode *expression = NULL;

  if (argc > 1) {
    expression = Parser_parse(&parser);
    const double value = ASTNode_evaluate(expression);
    printf("%.30f\n", value);
    ASTNodePull_push(&parser.node_pull, expression);
    return EXIT_SUCCESS;
  }

  loop {
    input(input_expression, sizeof(input_expression), ">>> ");
    if (strcmp(input_expression, "") == 0) {
      break;
    }

    lexer = Lexer_init(input_expression);
    Parser_reset(&parser, &lexer);

    expression = Parser_parse(&parser);
    const double value = ASTNode_evaluate(expression);

    printf("%.30f\n", value);
    ASTNodePull_push(&parser.node_pull, expression);
  }
}
