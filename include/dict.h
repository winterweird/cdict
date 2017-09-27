#ifndef CDICT_H
#define CDICT_H

#include "dict_common.h"
#include "dict_item.h"
#include "dict_hash.h"

/**
 * Create a new dict with initial capacity 16 and load factor 0.75 (mirrors
 * defaults in Java HashMap).
 */
dict dict_new();

/**
 * Create a new dict with the given inital capacity and load factor.
 */
dict dict_new_custom(int initialCapacity, double loadFactor);

/**
 * Add an item with key k and value v to the dict.
 */
void dict_put(dict* m, DictItem k, DictItem v);

/**
 * Retrieve item with key k from the dict.
 */
DictItem dict_get(dict m, DictItem k);

/**
 * Retrieve item with key k from the dict if such an item exists; otherwise,
 * print an error message and exit.
 */
DictItem dict_get_strict(dict m, DictItem k);

/**
 * Free the dict (destroying any allocated resources).
 */
void dict_free(dict m);

/**
 * Generic "out of memory" error function which prints an error message and
 * exits with exit code 1. Argument is attempted size of allocation, in bytes.
 */
void dict_error_memerror(unsigned long sz);

/**
 * Prints error message and exits with exit code 1.
 */
void dict_error_message(const char* msg);

/**
 * Prints error message followed by item and exits with exit code 1. Item is
 * formatted according to type.
 */
void dict_error_itemmessage(const char* msg, DictItem i);

/**
 * Used when there's an error in the API, not in the usage pattern. Should never
 * be seen by end users. Output error message and exit with code 1.
 */
void dict_error_apierror(const char* msg);

/**
 * Horrible, horrible macro for lazy people. May stop working without warning.
 *
 * This macro takes a dict object (not pointer), a type, and a DictItem key, and
 * returns the data of the result of the regular dict_get return value with the
 * given dictionary and key cast to the specified type. The macro casts to long
 * first because it stops the compiler from complaining with a
 * -Wpointer-to-int-cast warning. The reason it works is because the location in
 *  memory of the data pointer and everything else in theory should start at
 *  the same point because of the union. But I don't know if this is specified
 *  in the standard or implementation dependent, so don't use this in production
 *  code or whatever I guess.
 */
#define dict_get_t(dict, type, ditem) (type)((long)dict_get(dict, ditem).item.o.data)

#endif /* CDICT_H */
