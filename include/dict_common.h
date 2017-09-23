#ifndef CDICT_COMMON_H
#define CDICT_COMMON_H

#define DICT_ITEM_TYPE_NULL     0
#define DICT_ITEM_TYPE_INT      1
#define DICT_ITEM_TYPE_LONG     2
#define DICT_ITEM_TYPE_CHAR     3
#define DICT_ITEM_TYPE_STRING   4
#define DICT_ITEM_TYPE_DOUBLE   5

struct _DictItem;

typedef int (*dict_eq_t)(struct _DictItem a, struct _DictItem b);

/**
 * Generic dict item.
 */
typedef struct _DictItem {
    unsigned int type;
    union {
        int    i;
        long   l;
        char   c;
        char*  s;
        double d;
        struct {
            void* data;
            char* str;
            dict_eq_t eq;
            unsigned int size;
        } o;
    } item;
} DictItem;

typedef struct _DictKVPair {
    DictItem k;
    DictItem v;
    struct _DictKVPair* next;
} DictKVPair;

/**
 * Map structure.
 */
typedef struct {
    unsigned int capacity;
    unsigned int length;
    double loadFactor;
    DictKVPair** kvPairs;
} dict;

#endif /* CDICT_COMMON_H */
