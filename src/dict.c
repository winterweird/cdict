#include "dict.h"
#include "dict_hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // strlen

/**
 * Helper function: return a pointer to the first pair matching k, or NULL if:
 * 1) p is initially NULL
 * 2) no such match is found
 */
static DictKVPair* getFirstPairMatch(DictItem k, DictKVPair* p) {
    while (p != NULL) {
        if (dict_item_equals(k, p->k)) break;
        p = p->next;
    }
    return p;
}

/**
 * Helper function: set item value in a way which doesn't leak memory.
 */
static void setItemValue(DictItem* i, DictItem nVal) {
    if (i->type != nVal.type) {
        dict_error_apierror("setItemValue called with values of different type");
    }
    switch (i->type) {
        case DICT_ITEM_TYPE_NULL:
        case DICT_ITEM_TYPE_INT:
        case DICT_ITEM_TYPE_LONG:
        case DICT_ITEM_TYPE_CHAR:
        case DICT_ITEM_TYPE_DOUBLE:
            break; // no issue
        case DICT_ITEM_TYPE_STRING:
            if (i->item.s != nVal.item.s) {
                dict_item_free(*i); // free old
            }
            break;
        default:
            if (i->item.o.data != nVal.item.o.data) {
                dict_item_free(*i); // free old
            }
    }
    *i = nVal; // override old with new
}

/**
 * Helper function: Copies the data from any data pointer item (prevents
 * destroying memory pointers we need later).
 */
static DictItem copy(DictItem i) {
    DictItem nVal = i;
    switch (i.type) {
        case DICT_ITEM_TYPE_NULL:
        case DICT_ITEM_TYPE_INT:
        case DICT_ITEM_TYPE_LONG:
        case DICT_ITEM_TYPE_CHAR:
        case DICT_ITEM_TYPE_DOUBLE:
            break; // no issue
        case DICT_ITEM_TYPE_STRING:
            nVal.item.s = malloc(1 + strlen(i.item.s) * sizeof *nVal.item.s);
            strcpy(nVal.item.s, i.item.s);
            break;
        default:
            nVal.item.o.data = malloc(nVal.item.o.size);
            memcpy(nVal.item.o.data, i.item.o.data, nVal.item.o.size); // probably works
    }

    return nVal;
}

/**
 * Helper function: replaces m with a rehashed version of m which is twice as
 * big.
 */
static void rehashMap(dict* m) {
    dict m2 = dict_new_custom(m->capacity*2, m->loadFactor);
    for (size_t i = 0; i < m->capacity; i++) {
        DictKVPair* p = m->kvPairs[i];
        while (p != NULL) {
            dict_put(&m2, copy(p->k), copy(p->v));
            p = p->next;
        }
    }
    
    dict_free(*m);
    *m = m2;
}

/**
 * Helper function: add key to list of keys. Rellocate memory if necessary.
 */
static void addKey(dict* m, DictItem k) {
    if (m->nKeys >= m->keyCapacity) {
        m->keyCapacity *= 2;
        m->keys = realloc(m->keys, m->keyCapacity * sizeof *m->keys);
        if (m->keys == NULL) {
            dict_error_memerror(m->keyCapacity * sizeof *m->keys);
        }
    }
    m->keys[m->nKeys++] = k;
}

dict dict_new() {
    return dict_new_custom(16, 0.75);
}

dict dict_new_custom(int initialCapacity, double loadFactor) {
    dict m;
    
    m.capacity = initialCapacity;
    if (m.capacity <= 0) {
        dict_error_message("Initial capacity must be greater than 0");
    }
    
    m.loadFactor = loadFactor;
    if (m.loadFactor <= 0 || m.loadFactor > 1.0) {
        dict_error_message("Load factor must be between 0 and 1");
    }
    
    m.nKeys = 0;
    m.keyCapacity = m.capacity; // probably an okay initial value
    m.keys = malloc(m.keyCapacity * sizeof *m.keys);
    if (m.keys == NULL) {
        dict_error_memerror(m.keyCapacity * sizeof *m.keys);
    }
    
    m.length = 0;
    m.kvPairs = malloc(m.capacity * sizeof *m.kvPairs);
    if (m.kvPairs == NULL) {
        dict_error_memerror(m.capacity * sizeof *m.kvPairs);
    }
    
    // NULL-initialize all pairs
    for (size_t i = 0; i < m.capacity; i++) {
        m.kvPairs[i] = NULL;
    }
    
    return m;
}

void dict_put(dict* m, DictItem k, DictItem v) {
    if (m->length >= m->capacity * m->loadFactor) {
        rehashMap(m);
    }

    size_t index = dict_item_hash(k) % m->capacity;

    if (m->kvPairs[index] == NULL) {
        m->kvPairs[index] = malloc(sizeof *m->kvPairs[index]);
        if (m->kvPairs[index] == NULL){
            dict_error_memerror(sizeof *m->kvPairs[index]);
        }
        // newly allocated, thus: no memory leak from direct assignment
        m->kvPairs[index]->k = k;
        m->kvPairs[index]->v = v;
        m->kvPairs[index]->next = NULL;
        m->length++; // one more bucket is used
        
        addKey(m, k); // add key
    }
    else {
        DictKVPair* p = getFirstPairMatch(k, m->kvPairs[index]);
        if (p == NULL) {
            // allocate new item at head of list
            p = malloc(sizeof *p);
            if (p == NULL) {
                dict_error_memerror(sizeof *p);
            }
            // newly allocated, thus: no memory leak from direct assignment
            p->k = k;
            p->v = v;
            p->next = m->kvPairs[index];
            m->kvPairs[index] = p;

            addKey(m, k); // add key
        }
        else {
            // can result in memory leak if not using setItemValue
            setItemValue(&p->v, v);
        }
    }
}

DictItem dict_get(dict m, DictItem k) {
    size_t index = dict_item_hash(k) % m.capacity;
    DictKVPair* p = getFirstPairMatch(k, m.kvPairs[index]);
    if (p == NULL) return dict_item_new_null();
    return p->v;
}

DictItem dict_get_strict(dict m, DictItem k) {
    DictItem r = dict_get(m, k);
    if (dict_item_null(r)) {
        dict_error_itemmessage("Key not found:", k);
    }
    return r;
}

void dict_free(dict m) {
    for (size_t i = 0; i < m.capacity; i++) {
        DictKVPair* p = m.kvPairs[i];
        while (p != NULL) {
            dict_item_free(p->k);
            dict_item_free(p->v);
            DictKVPair* tmp = p->next;
            free(p);
            p = tmp;
        }
    }
}

void dict_error_memerror(unsigned long sz) {
    fprintf(stderr, "Error: bad malloc (%lu bytes), out of memory\n", sz);
    exit(1);
}

void dict_error_message(const char* msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

void dict_error_itemmessage(const char* msg, DictItem i) {
    switch (i.type) {
        case DICT_ITEM_TYPE_NULL:
            fprintf(stderr, "Error: %s %s\n", msg, "null");
            break;
        case DICT_ITEM_TYPE_INT:
            fprintf(stderr, "Error: %s %d\n", msg, i.item.i);
            break;
        case DICT_ITEM_TYPE_LONG:
            fprintf(stderr, "Error: %s %ld\n", msg, i.item.l);
            break;
        case DICT_ITEM_TYPE_CHAR:
            fprintf(stderr, "Error: %s %c\n", msg, i.item.c);
            break;
        case DICT_ITEM_TYPE_DOUBLE:
            fprintf(stderr, "Error: %s %f\n", msg, i.item.d);
            break;
        case DICT_ITEM_TYPE_STRING:
            fprintf(stderr, "Error: %s %s\n", msg, i.item.s);
            break;
        default:
            fprintf(stderr, "Error: %s %s\n", msg, i.item.o.str);
    }
    exit(1);
}

void dict_error_apierror(const char* msg) {
    fprintf(stderr, "API error: %s\n", msg);
    exit(1);
}
