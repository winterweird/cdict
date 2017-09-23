#ifndef CDICT_ITEM_H
#define CDICT_ITEM_H

#include "dict_common.h"

/**
 * Functions for creating new Items.
 */
DictItem dict_item_new_null    ();
DictItem dict_item_new_int     (int    v);
DictItem dict_item_new_long    (long   v);
DictItem dict_item_new_char    (char   v);
DictItem dict_item_new_string  (char*  v);
DictItem dict_item_new_double  (double v);
DictItem dict_item_new_object  (int type, void* ptr, unsigned int sz, dict_eq_t eq);

/**
 * Returns 1 if the item is null, 0 otherwise.
 */
int dict_item_null(DictItem i);

/**
 * Returns 1 if the items are to be considered equal, 0 otherwise.
 */
int dict_item_equals(DictItem a, DictItem b);

/**
 * Free resources allocated to item, if any.
 */
void dict_item_free(DictItem i);

#endif /* CDICT_ITEM_H */
