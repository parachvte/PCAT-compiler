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
Table* new_table() {
    // Init a table header (header don't actually store any information)
    Table* t = malloc(sizeof(Table));
    t->elem = NULL;
    t->next = NULL;
    return t;
}

/**
 * Insert a new `struct pair` into table `*t`
 *
 * Don't check if the key to be inserted exists already or not.
 */
void table_insert(Table** t, const char* key, ast* value) {
    Pair* p = new_pair(key, value);
    Table* new_header = malloc(sizeof(Table));
    new_header->elem = p;
    new_header->next = *t;
    *t = new_header;
}

/**
 * look up `key` in Table `t`
 * @return ast|NULL
 */
struct ast* table_lookup(Table* t, const char *key) {
    for (Table* i = t; i->elem; i = i->next) {
        Pair* p = i->elem;
        if (!strcmp(p->key, key))
            return p->value;
    }
    return NULL;
}

/*****************************************************
              Operations on Scope
*****************************************************/

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
    table_insert(&scope_top->table, key, value);
}

/**
 * look up `key` in Scopes 
 * @return ast|NULL
 * `target_level` will be changed
 */
ast* lookup(const char* key, int* target_level) {
    for (Scope* i = scope_top; i->level; i = i->father) {
        ast* res = table_lookup(i->table, key);
        if (res) {
            *target_level = i->level;
            return res;
        }
    }
    target_level = NULL;
    return NULL;
}
