/*
 * qseries.h - q-Series related programs (header file).
 *
 * Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created:  2016-07-05
 * Modified: 2016-07-13
 * License:  MIT License (see LICENSE.txt)
 *
 * Note: We are only dealing with q-series with integer coefficients
 *   (64-bit).
 */

#pragma once

#include <stdint.h>

/* The maximum order = highest degree + 1. */
#define MAXORD 256

/* Coefficient array to represent a q-series. */
typedef int64_t qseries_t[MAXORD];

/*******************************************************************\
 * Initializers                                                    *
 *******************************************************************/

/* Initialize the coefficients of a q-series to `initial_val`. */
void init_qseries(qseries_t s, int64_t initial_val);

/*
 * Make a q-series from `num` consecutive elements, starting at
 * index `start`, of an array `a[]`, with coeffs shifted by `offset`.
 */
void mk_qseries(const int64_t a[], size_t start, size_t num,
        size_t offset, qseries_t s);

/* Copy a q-series `s` into another `t`. */
void cp_qseries(const qseries_t s, qseries_t t);

/*******************************************************************\
 * Input/Output                                                    *
 *******************************************************************/

/* Print a q-series upto order `ord`. */
void print_qseries(const qseries_t s, size_t ord);

/* Print a q-series upto order `ord` (with a newline). */
void println_qseries(const qseries_t s, size_t ord);

/* Print the coefficient array of the q-series `s`. */
void print_coeffs(const qseries_t s, size_t ord);

/* Print the coefficient array of the q-series `s` (with newline). */
void println_coeffs(const qseries_t s, size_t ord);

/* Read a q-series into `s`. */
void read_qseries(const char *buf, qseries_t s);

/*******************************************************************\
 * Basic Operations                                                *
 *******************************************************************/

/* Shift the power series by `shift` to the right, result in `ans` */
void shift_qseries(const qseries_t s, int shift, qseries_t ans);

/* Scale a q-series `s` by a scalar `c`, put the result in `ans`. */
void scale_qseries(int64_t c, const qseries_t s, qseries_t ans);

/* Add two q-series, put the result in `ans`. */
void add_qseries(const qseries_t s, const qseries_t t, qseries_t ans);

/* Subtract q-sries `t` from the q-series `s`, result in `ans`. */
void subtract_qseries(const qseries_t s, const qseries_t t, qseries_t ans);

/* Multiply two q-series, put the resutl in `ans`. */
void multiply_qseries(const qseries_t s, const qseries_t t, qseries_t ans);

/* Invert a q-series, put the result in `ans`. */
void invert_qseries(const qseries_t s, qseries_t ans);

/* Divide a q-series `s` by another `t`, put the result in `ans`. */
void divide_qseries(const qseries_t s, const qseries_t t, qseries_t ans);

/* Compute q-series `s`, raised to the power `n`, result in `ans`. */
/* Right now, n is assumed to be an integer. */
void pow_qseries(const qseries_t s, int n, qseries_t ans);

/*******************************************************************\
 * Miscellaneous                                                   *
 *******************************************************************/

/*
 * Compute the product side, put the result in `ans`.
 *   Product over all natural number, n:
 *      (1 - q^n)^cong[n]
 *   cong[n] = -1, 0, or 1.
 */
void product_side(int mod, const int cong[mod], qseries_t ans);

