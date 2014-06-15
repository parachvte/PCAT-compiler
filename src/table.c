/**
 * Look-up Table
 *
 * File     : table.c
 * Author   : Ryannnnnnn
 */

#include "table.h"
#include <string.h>

/* Structure Pair, Pair list, Table */
typedef struct pair {
    char* key;
    ast* value;
} pair;

typedef struct pair_list {
    pair* elem;
    struct pair_list* next;
} pair_list;

typedef struct table {
    pair_list* list;
} table;


table* new_table() {
    table* t = malloc(sizeof(table)); // init a table header
    return t;
}

pair_list* new_pair_list(pair* elem, pair_list* next) {
    pair_list* pl = malloc(sizeof(pair_list));
    pl->elem = elem;
    pl->next = next;
    return pl;
}

pair* new_pair( const char * key, ast* value ){    
    pair* p = malloc( sizeof(pair) );
    p->key = malloc( 1 + strlen(key) );
    strcpy(p->key,key);
    p->value = value;
    return p;
}

int same_name(const char * a, const char * b ){
    return strcmp(a, b) == 0;
}

struct ast* table_lookup( table* t, const char * key ){
    pair_list* pl;
    for( pl = t->list; pl ; pl = pl->next ){
        pair* p = pl->elem;
        if ( same_name(p->key, key) )
            return p->value;
    }
    return NULL;
}

void table_insert ( table* t, const char* key, ast* binding ){
    // don't check if key is already in table...
    pair* p = new_pair(key,binding);
    t->list = new_pair_list( p, t->list );
}

////////////////////////
// multi-scope 
////////////////////////

typedef struct scope{
    table*              t;
    struct scope*       next;
    int                 level;
}scope;

scope* new_scope( table* t, scope* next, int level ){
    scope* s = malloc(sizeof(scope));
    s->t = t;
    s->next = next;
    s->level = level;
    return s;
}

scope* scope_head;

void scope_init(){
    scope_head = new_scope(NULL, NULL, 0);
}

void begin_scope(){
    scope_head = new_scope(new_table(), scope_head, scope_head->level+1);
}

void end_scope(){
    scope_head = scope_head->next;
}

void insert( const char * key, ast* binding ){
    table_insert(scope_head->t,key,binding);
}

ast* lookup( const char* key, int* p_level ){
    // return "ast*" the value
    // store level in "*p_level" iff p_level != NULL
    scope* s = scope_head;
    ast* a;
    for(; s->level > 0; s = s->next ){
        a = table_lookup(s->t,key);
        if ( a != NULL ){
            if ( p_level ) *p_level = s->level;
            return a;
        }
    }
    if ( p_level ) p_level = 0;
    return NULL;
}

int curr_level(){
    return scope_head->level;
}
