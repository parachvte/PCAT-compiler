/**
 * Look-up Table
 *
 * File     : table.c
 * Author   : Ryannnnnnn
 */

#include "table.h"
#include <string.h>

/*****************************************************
              Operations on pair 
*****************************************************/
typedef struct Pair {
    char* key;
    ast* value;
} Pair;

/** Construct function */
Pair* new_pair(const char * key, ast* value) {    
    Pair* p = malloc(sizeof(Pair));
    p->key = malloc(strlen(key) + 1);
    strcpy(p->key, key);
    p->value = value;
    return p;
}

/*****************************************************
              Operations on table
*****************************************************/
typedef struct PairList {
    Pair* elem;
    struct PairList* next;
} PairList;
typedef struct Table {
    PairList* list;
} Table;

Table* new_table() {
    Table* t = malloc(sizeof(Table)); // init a table header
    return t;
}

/**
 * Insert a new `struct pair` into table.
 *
 * Don't check if the key to be inserted is exists already
 */
void table_insert(Table* t, const char* key, ast* value) {
    Pair* p = new_pair(key, value);
    PairList* new_header = malloc(sizeof(PairList));
    new_header->elem = p;
    new_header->next = t->list;
    t->list = new_header;
}

/**
 * look up `key` in Table `t`
 * @return ast|NULL
 */
struct ast* table_lookup(Table* t, const char *key) {
    for (PairList* i = t->list; i; i = i->next) {
        Pair* p = i->elem;
        if (!strcmp(p->key, key))
            return p->value;
    }
    return NULL;
}

/*****************************************************
              Operations on scope
*****************************************************/
typedef struct Scope {  // A Stack
    Table* table;
    struct Scope* father;
    int level;
} Scope;

Scope* scope_top;

/** Construct function */
Scope* new_scope(Table* table, Scope* father, int level) {
    Scope* s = malloc(sizeof(Scope));
    s->table = table;
    s->father = father;
    s->level = level;
    return s;
}

void scope_init() {
    scope_top = new_scope(NULL, NULL, 0);
}

void begin_scope() { // Push to the top of stack
    scope_top = new_scope(new_table(), scope_top, scope_top->level + 1);
}

void end_scope() { // Pop out of stack
    scope_top = scope_top->father;
}

void insert(const char *key, ast* value) {
    table_insert(scope_top->table, key, value);
}

/**
 * look up `key` in Scopes 
 * @return ast|NULL
 * ``
 */
ast* lookup(const char* key, int* target_level) {
    for (Scope* i = scope_top; i->level; i = i->father) {
        ast* res = table_lookup(i->table, key);
        if (res) {
            if (target_level) *target_level = i->level;
            return res;
        }
    }
    if (target_level) target_level = 0;
    return NULL;
}

int curr_level() {
    return scope_top->level;
}

int same_name(const char * a, const char * b ){
    return strcmp(a, b) == 0;
}
