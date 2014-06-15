#ifndef _TABLE_H_
#define _TABLE_H_

#include "ast.h"

/** Definitions */
typedef struct Pair {
    char* key;
    ast* value;
} Pair;

typedef struct Table {
    Pair* elem;
    struct Table* next;
} Table;

typedef struct Scope {
    Table* table;
    struct Scope* father;
    int level;
} Scope;

Scope* scope_top;

#define CUR_LEVEL (scope_top->level)

/** Functions */
void scope_init();
void begin_scope();
void end_scope();
void insert(const char *key, ast* value);
ast* lookup(const char *key, int* target_level);

#endif
