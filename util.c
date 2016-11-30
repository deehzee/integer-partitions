/*
 * util.c - Utility functions.
 *
 * Author:   Debajyoti Nandi
 * Created:  2016-07-08
 * Modified: 2016-07-13
 * License:  MIT License (see LICENSE.txt)
 *
 * Compilation Suggestions:
 *   CC = gcc #( or clang)
 *   CFLAGS = -std=gnu11 -O3 #(or, -O2)
 *   $(CC) $(CFLAGS) -c util.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "util.h"

void print_array(size_t len, const int a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", a[i]);
    }
    printf("]");
}

void print_array_int64(size_t len, const int64_t a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%" PRId64, a[i]);
    }
    printf("]");
}

void print_array_uint64(size_t len, const uint64_t a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%" PRIu64, a[i]);
    }
    printf("]");
}

void println_array(size_t len, const int a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", a[i]);
    }
    printf("]\n");
}

void println_array_int64(size_t len, const int64_t a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%" PRId64, a[i]);
    }
    printf("]\n");
}

void println_array_uint64(size_t len, const uint64_t a[len])
{
    printf("[");
    for (size_t i = 0; i < len; i++) {
        if (i != 0)
            printf(", ");
        printf("%" PRIu64, a[i]);
    }
    printf("]\n");
}

void init_array(size_t len, int a[len], int x)
{
    for (size_t i = 0; i < len; i++)
        a[i] = x;
}

void init_array_int64(size_t len, int64_t a[len], int64_t x)
{
    for (size_t i = 0; i < len; i++)
        a[i] = x;
}

void init_array_uint64(size_t len, uint64_t a[len], uint64_t x)
{
    for (size_t i = 0; i < len; i++)
        a[i] = x;
}
