/*
 * util.h - Utility functions.
 *
 * Author:   Debajyoti Nandi
 * Created:  2016-07-08
 * Modified: 2016-07-13
 * License:  MIT License (see LICENSE.txt)
 *
 */

#pragma once

#include <stdlib.h>
#include <stdint.h>

/* Print various array types */
void print_array(size_t len, const int a[len]);
void print_array_int64(size_t len, const int64_t a[len]);
void print_array_uint64(size_t len, const uint64_t a[len]);
void println_array(size_t len, const int a[len]);
void println_array_int64(size_t len, const int64_t a[len]);
void println_array_uint64(size_t len, const uint64_t a[len]);

/* Initialize various array types */
void init_array(size_t len, int a[len], int x);
void init_array_int64(size_t len, int64_t a[len], int64_t x);
void init_array_uint64(size_t len, uint64_t a[len], uint64_t x);
