#include "dict_item.h"
#include "dict.h"
#include <string.h> // strcmp
#include <stdlib.h> // free

DictItem dict_item_new_null   () {
    DictItem i;
    i.type = DICT_ITEM_TYPE_NULL;
    return i;
}
DictItem dict_item_new_int    (int    v) {
    DictItem i;
    i.type = DICT_ITEM_TYPE_INT;
    i.item.i = v;
    return i;
}
DictItem dict_item_new_long   (long   v) {
    DictItem i;
    i.type = DICT_ITEM_TYPE_LONG;
    i.item.l = v;
    return i;
}
DictItem dict_item_new_char   (char   v) {
    DictItem i;
    i.type = DICT_ITEM_TYPE_CHAR;
    i.item.c = v;
    return i;
}
DictItem dict_item_new_string (char*  v) {
    DictItem i;
    i.type = DICT_ITEM_TYPE_STRING;
    i.item.s = malloc(1 + strlen(v) * sizeof *i.item.s);
    if(i.item.s == NULL) {
        dict_error_memerror(1 + strlen(v) * sizeof *i.item.s);
    }
    strcpy(i.item.s, v);
    return i;
}
DictItem dict_item_new_double (double v) {
    DictItem i;
    i.type = DICT_ITEM_TYPE_DOUBLE;
    i.item.d = v;
    return i;
}
DictItem dict_item_new_object (int type, void* ptr, unsigned int sz, dict_eq_t eq) {
    DictItem i;
    i.type = type;
    i.item.o.data = ptr;
    i.item.o.size = sz;
    i.item.o.eq = eq;
    i.item.o.str = NULL; // can be overridden
    return i;
}

int dict_item_null(DictItem i) {
    return i.type == DICT_ITEM_TYPE_NULL;
}

int dict_item_equals(DictItem a, DictItem b) {
    if (a.type != b.type) return 0;
    if (a.type == DICT_ITEM_TYPE_NULL)     return 1; // null types are always equal
    if (a.type == DICT_ITEM_TYPE_INT)      return a.item.i == b.item.i;
    if (a.type == DICT_ITEM_TYPE_LONG)     return a.item.l == b.item.l;
    if (a.type == DICT_ITEM_TYPE_CHAR)     return a.item.c == b.item.c;
    if (a.type == DICT_ITEM_TYPE_STRING)   return strcmp(a.item.s, b.item.s) == 0;
    if (a.type == DICT_ITEM_TYPE_DOUBLE)   return a.item.d == b.item.d;
    
    // custom type: use custom eq function
    return a.item.o.eq(a, b);
}

void dict_item_free(DictItem i) {
    switch (i.type) {
        case DICT_ITEM_TYPE_INT:
        case DICT_ITEM_TYPE_LONG:
        case DICT_ITEM_TYPE_CHAR:
        case DICT_ITEM_TYPE_DOUBLE:
            break; // nothing allocated
        case DICT_ITEM_TYPE_STRING:
            free(i.item.s);
            break;
        default:
            free(i.item.o.data);
            free(i.item.o.str);
    }
}
