#include <stdio.h>
#include "dict.h"
#include <string.h> //strlen

#define mks dict_item_new_string
#define mki dict_item_new_int

int main(int argc, char** argv) {

    dict d = dict_new(100, 0.75);
    char str[80];
    int hasInput = 1;
    while (hasInput) {
        char* ln = fgets(str, sizeof str, stdin);
        if (!ln) hasInput = 0;
        else {
            ln[strlen(ln)-1] = 0; // remove newline
            DictItem i = dict_get(d, mks(str));
            if (dict_item_null(i)) {
                dict_put(&d, mks(str), mki(1));
            }
            else {
                i.item.i += 1;
                dict_put(&d, mks(str), i);
            }
        }
    }

    for (size_t i = 0; i < d.capacity; i++) {
        DictKVPair* p = d.kvPairs[i];
        while (p != NULL) {
            DictItem k = d.kvPairs[i]->k;
            DictItem v = d.kvPairs[i]->v;
            printf("%s: %d\n", k.item.s, v.item.i);
            p = p->next;
        }
    }

    return 0;
}
