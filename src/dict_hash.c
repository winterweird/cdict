#include "dict_hash.h"
#include "dict.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h> // strlen

/**
 * Code below was taken from http://www.azillionmonkeys.com/qed/hash.html
 * and is licensed under the LGPL 2.1 License:
 * https://www.gnu.org/licenses/lgpl-2.1.txt
 */
#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
      || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                               +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

uint32_t SuperFastHash (const char * data, int len) {
    uint32_t hash = len, tmp;
    int rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--) {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem) {
        case 3: hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= ((signed char)data[sizeof (uint16_t)]) << 18;
            hash += hash >> 11;
            break;
        case 2: hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        case 1: hash += (signed char)*data;
            hash ^= hash << 10;
            hash += hash >> 1;
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}

/** End of code from http://www.azillionmonkeys.com/qed/hash.html **/

int dict_item_hash(DictItem i) {
    int h;
    if (i.type == DICT_ITEM_TYPE_INT) {
        h = SuperFastHash((const char*)&i.item.i, sizeof i.item.i);
    }
    else if (i.type == DICT_ITEM_TYPE_LONG) {
        h = SuperFastHash((const char*)&i.item.l, sizeof i.item.l);
    }
    else if (i.type == DICT_ITEM_TYPE_CHAR) {
        h = SuperFastHash((const char*)&i.item.c, sizeof i.item.c);
    }
    else if (i.type == DICT_ITEM_TYPE_STRING) {
        h = SuperFastHash((const char*)i.item.s, strlen(i.item.s));
    }
    else if (i.type == DICT_ITEM_TYPE_DOUBLE) {
        h = SuperFastHash((const char*)&i.item.d, sizeof i.item.d);
    }
    else {
        h = SuperFastHash((const char*)i.item.o.data, i.item.o.size);
    }

    return h;
}
