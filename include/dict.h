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

/**
 * Slightly less horrible set of macros for lazy people.
 * 
 * If you're anything like me, you absolutely loathe having to type long
 * function names and whatnot. Particularly things you type often. And this
 * library comes with a bunch of long function names. You might even have to
 * type them often.
 *
 * Avoid this problem by defining the macro "DICT_USE_SHORT_ALIAS" before
 * including this header. This will define aliases "dmk<character>" where the
 * character corresponds to the kind of item you want to make: i for int, l for
 * long, c for char, d for double, s for string and o for object (read:
 * everything else). Now instead of long and descriptive, yet inherently similar
 * and thus confusing function names, you may instead use short and
 * nondescriptive, yet inherently similar and thus confusing function names.
 * You're welcome!
 *
 * (Note: does not attempt to redefine already defined macros which are
 * coincidentally named the same as a short function alias.)
 *
 * (Also note that there is no short version of dict_item_new_null - you may
 * shoot yourself in the foot if you'd like, but I'm not gonna make it easier
 * for you than it already is.)
 */

#ifdef DICT_USE_SHORT_ALIAS

#ifndef dmki
#define dmki dict_item_new_int
#endif /* dmki */

#ifndef dmkl
#define dmkl dict_item_new_long
#endif /* dmkl */

#ifndef dmkc
#define dmkc dict_item_new_char
#endif /* dmkc */

#ifndef dmks
#define dmks dict_item_new_string
#endif /* dmks */

#ifndef dmkd
#define dmkd dict_item_new_double
#endif /* dmkd */

#ifndef dmko
#define dmko dict_item_new_object
#endif /* dmko */

#endif /* DICT_USE_SHORT_ALIAS */

#endif /* CDICT_H */
