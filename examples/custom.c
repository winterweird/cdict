#include <stdio.h>
#include <stdlib.h>
#include "dict.h"
#include <stdarg.h>
#include <math.h> // sqrt

// shorthand for make int
#define mki dict_item_new_int

// shorthand for make long
#define mkl dict_item_new_long

// shorthand for make string
#define mks dict_item_new_string

// shorthand for make custom1
#define mkpair dict_item_new_custom1

typedef struct {
    int x;
    int y;
} coord;

int eq(DictItem a, DictItem b) {
    coord* aCoord = a.item.o.data;
    coord* bCoord = b.item.o.data;
    return aCoord->x == bCoord->x && aCoord->y == bCoord->y;
}

DictItem mkpair(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int x = va_arg(args, int);
    int y = va_arg(args, int);
    va_end(args);
    
    coord* c = malloc(sizeof *c);
    c->x = x; c->y = y;
    DictItem i = dict_item_new_object(6, c, sizeof *c, eq);
    
    i.item.o.str = malloc(20);
    snprintf(i.item.o.str, 20, format, x, y);
    
    return i;
}

double distance(coord* a, coord* b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return sqrt(dx*dx + dy*dy);
}

int main(int argc, char** argv) {
    dict m = dict_new(20, 0.75);

    dict_put(&m, mki(0), mkpair("(%d, %d)", 542, 349));
    dict_put(&m, mki(1), mkpair("(%d, %d)", 711, 142));

    DictItem a = dict_get(m, mki(0));
    DictItem b = dict_get(m, mki(1));
    printf("Distance between %s and %s is %f\n", a.item.o.str, b.item.o.str,
            distance(a.item.o.data,b.item.o.data));

    dict_put(&m, mkpair("(%d, %d)", 399, 400), mki(42));
    dict_put(&m, mkpair("(%d, %d)", 400, 400), mki(45));
    dict_put(&m, mkpair("(%d, %d)", 401, 401), mki(43));
    dict_put(&m, mkpair("(%d, %d)", 399, 400), mki(44));

    printf("%d %d\n", dict_get(m, mkpair("(%d, %d)", 399, 400)).item.i,
            dict_get(m, mkpair("%d, %d)", 400, 400)).item.i);
    
    dict_free(m);
    
    return 0;
}
