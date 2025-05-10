//
// Created by kir on 09.05.2025.
//
#include "RBT.h"

#include "Error.h"
#include "Murmur3.h"

#include <stdlib.h>
#include <string.h>

static uint64_t compute_hash(const char *key) {
  return murmur3_64(key, strlen(key), MURMUR_SEED);
}

ErrorType RBT_allocate(RBTree **rbt, Error *error) {
  *rbt = malloc(sizeof(RBTree));
  if (*rbt == NULL) {
    if (error != NULL) {
      error->type = ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
    }
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }
  (*rbt)->nil = malloc(sizeof(RBTNode));
  if ((*rbt)->nil == NULL) {
    free(*rbt);
    if (error != NULL) {
      error->type = ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
    }
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }

  (*rbt)->nil->color = RBT_BLACK_COLOR;
  (*rbt)->nil->left = (*rbt)->nil->right = (*rbt)->nil->parent = (*rbt)->nil;
  (*rbt)->root = (*rbt)->nil;

  return ERROR_TYPE_OK;
}

void RBT_deallocate(RBTree *rbt) {
  if (!rbt)
    return;

  RBTNode *stack[64];
  int top = -1;
  RBTNode *current = rbt->root;

  while (1) {
    while (current != rbt->nil) {
      stack[++top] = current;
      current = current->left;
    }

    if (top == -1)
      break;

    RBTNode *node = stack[top--];
    current = node->right;

    free(node);
  }

  free(rbt->nil);
  free(rbt);
}

static void RBT_left_rotate(RBTree *tree, RBTNode *x) {
  RBTNode *y = x->right;
  x->right = y->left;

  if (y->left != tree->nil)
    y->left->parent = x;

  y->parent = x->parent;

  if (x->parent == tree->nil)
    tree->root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x;
  x->parent = y;
}

static void RBT_right_rotate(RBTree *tree, RBTNode *y) {
  RBTNode *x = y->left;
  y->left = x->right;

  if (x->right != tree->nil)
    x->right->parent = y;

  x->parent = y->parent;

  if (y->parent == tree->nil)
    tree->root = x;
  else if (y == y->parent->right)
    y->parent->right = x;
  else
    y->parent->left = x;

  x->right = y;
  y->parent = x;
}

static void RBT_fixup(RBTree *rbt, RBTNode *z) {
  while (z->parent->color == RBT_RED_COLOR) {
    if (z->parent == z->parent->parent->left) {
      RBTNode *y = z->parent->parent->right;

      if (y->color == RBT_RED_COLOR) {
        z->parent->color = RBT_BLACK_COLOR;
        y->color = RBT_BLACK_COLOR;
        z->parent->parent->color = RBT_RED_COLOR;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          RBT_left_rotate(rbt, z);
        }
        z->parent->color = RBT_BLACK_COLOR;
        z->parent->parent->color = RBT_RED_COLOR;
        RBT_right_rotate(rbt, z->parent->parent);
      }
    } else {
      RBTNode *y = z->parent->parent->left;

      if (y->color == RBT_RED_COLOR) {
        z->parent->color = RBT_BLACK_COLOR;
        y->color = RBT_BLACK_COLOR;
        z->parent->parent->color = RBT_RED_COLOR;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          RBT_right_rotate(rbt, z);
        }
        z->parent->color = RBT_BLACK_COLOR;
        z->parent->parent->color = RBT_RED_COLOR;
        RBT_left_rotate(rbt, z->parent->parent);
      }
    }
  }
  rbt->root->color = RBT_BLACK_COLOR;
}

ErrorType RBT_insert(RBTree *rbt, const char *key,
                     const RBTNodeValue value) {
  RBTNode *z = malloc(sizeof(RBTNode));
  if (z == NULL) {
    return ERROR_TYPE_MEMORY_ALLOCATION_FAILED;
  }

  z->hash = compute_hash(key);
  z->value = value;
  z->color = RBT_RED_COLOR;
  z->left = z->right = z->parent = rbt->nil;

  RBTNode *y = rbt->nil;
  RBTNode *x = rbt->root;

  while (x != rbt->nil) {
    y = x;
    if (z->hash < x->hash)
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;
  if (y == rbt->nil)
    rbt->root = z;
  else if (z->hash < y->hash)
    y->left = z;
  else
    y->right = z;

  RBT_fixup(rbt, z);

  return ERROR_TYPE_OK;
}

RBTNode *RBT_search(const RBTree *rbt, const char *key) {
  const uint64_t hash = compute_hash(key);
  RBTNode *current = rbt->root;

  while (current != rbt->nil) {
    if (hash == current->hash)
      return current;

    current = hash < current->hash ? current->left : current->right;
  }

  return NULL;
}

static RBTNode *RBT_find_node(const RBTree *tree, const char *key) {
  const uint64_t hash = compute_hash(key);
  RBTNode *current = tree->root;

  while (current != tree->nil) {
    if (hash == current->hash)
      return current;

    current = hash < current->hash ? current->left : current->right;
  }

  return NULL;
}

static void RBT_transplant(RBTree *tree, const RBTNode *u, RBTNode *v) {
  if (u->parent == tree->nil)
    tree->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

static RBTNode *RBT_minimum(const RBTree *tree, RBTNode *node) {
  while (node->left != tree->nil)
    node = node->left;
  return node;
}

void RBT_delete_fixup(RBTree *tree, RBTNode *x) {
  while (x != tree->root && x->color == RBT_BLACK_COLOR) {
    if (x == x->parent->left) {
      RBTNode *w = x->parent->right;
      if (w->color == RBT_RED_COLOR) {
        w->color = RBT_BLACK_COLOR;
        x->parent->color = RBT_RED_COLOR;
        RBT_left_rotate(tree, x->parent);
        w = x->parent->right;
      }
      if (w->left->color == RBT_BLACK_COLOR &&
          w->right->color == RBT_BLACK_COLOR) {
        w->color = RBT_RED_COLOR;
        x = x->parent;
      } else {
        if (w->right->color == RBT_BLACK_COLOR) {
          w->left->color = RBT_BLACK_COLOR;
          w->color = RBT_RED_COLOR;
          RBT_right_rotate(tree, w);
          w = x->parent->right;
        }
        w->color = x->parent->color;
        x->parent->color = RBT_BLACK_COLOR;
        w->right->color = RBT_BLACK_COLOR;
        RBT_left_rotate(tree, x->parent);
        x = tree->root;
      }
    } else {
      RBTNode *w = x->parent->left;
      if (w->color == RBT_RED_COLOR) {
        w->color = RBT_BLACK_COLOR;
        x->parent->color = RBT_RED_COLOR;
        RBT_right_rotate(tree, x->parent);
        w = x->parent->left;
      }
      if (w->right->color == RBT_BLACK_COLOR &&
          w->left->color == RBT_BLACK_COLOR) {
        w->color = RBT_RED_COLOR;
        x = x->parent;
      } else {
        if (w->left->color == RBT_BLACK_COLOR) {
          w->right->color = RBT_BLACK_COLOR;
          w->color = RBT_RED_COLOR;
          RBT_left_rotate(tree, w);
          w = x->parent->left;
        }
        w->color = x->parent->color;
        x->parent->color = RBT_BLACK_COLOR;
        w->left->color = RBT_BLACK_COLOR;
        RBT_right_rotate(tree, x->parent);
        x = tree->root;
      }
    }
  }
  x->color = RBT_BLACK_COLOR;
}

void RBT_delete(RBTree *tree, const char *key) {
  RBTNode *z = RBT_find_node(tree, key);
  if (z == NULL)
    return;

  RBTNode *y = z;
  RBTNode *x;
  RBTNodeColor y_original_color = y->color;

  if (z->left == tree->nil) {
    x = z->right;
    RBT_transplant(tree, z, z->right);
  } else if (z->right == tree->nil) {
    x = z->left;
    RBT_transplant(tree, z, z->left);
  } else {
    y = RBT_minimum(tree, z->right);
    y_original_color = y->color;
    x = y->right;

    if (y->parent == z) {
      x->parent = y;
    } else {
      RBT_transplant(tree, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    RBT_transplant(tree, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  free(z);

  if (y_original_color == RBT_BLACK_COLOR)
    RBT_delete_fixup(tree, x);
}
