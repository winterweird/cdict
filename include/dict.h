#ifndef CDICT_H
#define CDICT_H

#include "dict_common.h"
#include "dict_item.h"
#include "dict_hash.h"

/**
 * Create a new dict with the given inital capacity and load factor.
 */
dict dict_new(int initialCapacity, double loadFactor);

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

#endif /* CDICT_H */
