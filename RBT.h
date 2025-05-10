//
// Created by kir on 09.05.2025.
//

#ifndef RBT_H
#define RBT_H
#include <stdint.h>

#include "Enums.h"
#include "Error.h"

#define HASH_SEED 0

typedef union {
  int64_t i64;
  uint64_t u64;
  double f64;
  void *void_ptr;
} RBTNodeValue;

typedef enum {
  RBT_BLACK_COLOR,
  RBT_RED_COLOR,
} RBTNodeColor;

typedef struct tag_RBNode {
  struct tag_RBNode *parent;
  struct tag_RBNode *left;
  struct tag_RBNode *right;
  uint64_t hash;
  RBTNodeValue value;
  RBTNodeColor color;
} RBTNode;

typedef struct tag_RBTree {
  RBTNode *root;
  RBTNode *nil;
} RBTree;

#define MURMUR_SEED 0x12345678

ErrorType RBT_allocate(RBTree **rbt, Error *);
void RBT_deallocate(RBTree *rbt);

RBTNode *RBT_search(const RBTree *rbt, const char *key);
ErrorType RBT_insert(RBTree *rbt, const char *key, RBTNodeValue value);
void RBT_delete(RBTree *tree, const char *key);

#endif // RBT_H
