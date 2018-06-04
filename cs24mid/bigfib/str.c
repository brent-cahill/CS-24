#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "str.h"

void init_str(str_t *s) {
    s->capacity = 0;
    s->length = 0;
    s->psz = NULL;
}

void clear_str(str_t *s) {
    if (s->psz != NULL) {
        free(s->psz);
        s->psz = NULL;
    }

    s->capacity = 0;
    s->length = 0;
}

void append_char(str_t *s, char ch) {
    if (s->psz == NULL) {
        s->capacity = 16;
        s->length = 0;
        s->psz = malloc(16);
        if (s->psz != NULL)
            s->psz[s->length] = '\0';
    }
    else {
        assert(s->length < s->capacity);
        if (s->length + 1 == s->capacity) {
            s->capacity *= 2;
            s->psz = realloc(s->psz, s->capacity);
        }
    }

    if (s->psz == NULL) {
        printf("ERROR:  Couldn't allocate string buffer!\n");
        abort();
    }

    s->psz[s->length] = ch;
    s->length++;
    s->psz[s->length] = '\0';
}

void reverse_str(str_t *s) {
    int a, b;

    a = 0;
    b = s->length - 1;

    while (a < b) {
        char tmp = s->psz[a];
        s->psz[a] = s->psz[b];
        s->psz[b] = tmp;

        a++;
        b--;
    }
}

