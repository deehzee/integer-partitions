/*
 * qseries.c - q-Series related programs.
 *
 * Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created:  2016-07-05
 * Modified: 2016-07-13
 * License:  MIT License (see LICENSE.txt)
 *
 * Compilation Suggestions:
 *   CC = gcc #( or clang)
 *   CFLAGS = -std=gnu11 -O3 #(or, -O2)
 *   $(CC) $(CFLAGS) -c qseries.c
 *
 * Note: We are only dealing with q-series with integer coefficients
 *   (64-bit).
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "qseries.h"
#include "util.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*******************************************************************\
 * Initializers                                                    *
 *******************************************************************/

void init_qseries(qseries_t s, int64_t initial_val)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        s[deg] = initial_val;
}

void mk_qseries(const int64_t a[], size_t start, size_t num,
        size_t offset, qseries_t s)
{
    size_t max = MIN(MAXORD, num + offset);
    for (size_t deg = offset; deg + offset < max; deg++)
        s[deg] = a[start + deg];
}

void cp_qseries(const qseries_t s, qseries_t t)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        t[deg] = s[deg];
}

/*******************************************************************\
 * Input/Output                                                    *
 *******************************************************************/

void print_qseries(const qseries_t s, size_t ord)
{
    size_t max = MIN(MAXORD, ord);
    size_t deg = 0;
    int nonzero_terms = 0;
    int64_t abs_coeff;
    int neg;
    char *sgn;

    for (deg = 0; deg < max; deg++) {
        if (s[deg] == 0)
            continue;
        abs_coeff = (s[deg] > 0) ? s[deg] : -s[deg];
        neg = abs_coeff - s[deg];
        sgn = (neg)? "-" : "+";
        if (nonzero_terms) {
            printf(" %s ", sgn);
            sgn = "";
        } else {
            sgn = (neg) ? "-" : "";
        }
        if (deg == 0) {
            printf("%s%" PRId64, sgn, abs_coeff);
        } else {
            if (abs_coeff == 1)
                printf("%s", sgn);
            else
                printf("%s%" PRId64 "*", sgn, abs_coeff);
            if (deg == 1)
                printf("q");
            else
                printf("q^%zu", deg);
        }
        nonzero_terms++;
    }
    if (! nonzero_terms)
        printf("0");
    printf(" + O(%zu)", max);
}

void println_qseries(const qseries_t s, size_t ord)
{
    print_qseries(s, ord);
    printf("\n");
}

void print_coeffs(const qseries_t s, size_t ord)
{
    print_array_int64(ord, s);
}

void println_coeffs(const qseries_t s, size_t ord)
{
    print_array_int64(ord, s);
    printf("\n");
}

/* TODO */
void read_qseries(const char *buf, qseries_t s)
{
    char *str1, *str2, *tok, *subtok;
    char *saveptr1, *saveptr2;
    int j;

    for (j = 1, str1 = strdup(buf); ; j++, str1 = NULL) {
        tok = strtok_r(str1, "+", &saveptr1);
        if (tok == NULL)
            break;
        printf ("Term[%d]: %s\n", j, tok);

        for (str2 = tok; ; str2 = NULL) {
            subtok = strtok_r(str2, "*", &saveptr2);
            if (subtok == NULL)
                break;
            printf(" --> %s\n", subtok);
        }
    }
}

/*******************************************************************\
 * Basic Operations                                                *
 *******************************************************************/

void shift_qseries(const qseries_t s, int shift, qseries_t ans)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        ans[deg] = ((int) deg < shift) ? 0: s[deg - shift];
}

void scale_qseries(int64_t c, const qseries_t s, qseries_t ans)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        ans[deg] = c * s[deg];
}

void add_qseries(const qseries_t s, const qseries_t t, qseries_t ans)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        ans[deg] = s[deg] + t[deg];
}

void subtract_qseries(const qseries_t s, const qseries_t t, qseries_t ans)
{
    for (size_t deg = 0; deg < MAXORD; deg++)
        ans[deg] = s[deg] - t[deg];
}

void multiply_qseries(const qseries_t s, const qseries_t t, qseries_t ans)
{
    int64_t sum;
    size_t i;
    for (size_t deg = 0; deg < MAXORD; deg++) {
        for(i = 0, sum = 0; i <= deg; i++)
            sum += s[i] * t[deg - i];
        ans[deg] = sum;
    }
}

void invert_qseries(const qseries_t s, qseries_t ans)
{
    int64_t sum;
    size_t i;
    ans[0] = 1/s[0];
    for (size_t deg = 1; deg < MAXORD; deg++) {
        for (i = 1, sum = 0; i <= deg; i++)
            sum += s[i] * ans[deg - i];
        ans[deg] = -sum / s[0];
    }
}

void divide_qseries(const qseries_t s, const qseries_t t, qseries_t ans)
{
    qseries_t tmp;
    invert_qseries(t, tmp);
    multiply_qseries(s, tmp, ans);
}

/* Compute q-series `s`, raised to the power `n`, result in `ans`. */
/* Right now, n is assumed to be an integer. */
void pow_qseries(const qseries_t s, int n, qseries_t ans)
{
    qseries_t s0, tmp;
    int abs_n = 0;

    if (n > 0) {
        cp_qseries(s, s0);
        abs_n = n;
    } else if (n < 0) {
        invert_qseries(s, s0);
        abs_n = -n;
    }
    init_qseries(ans, 0);
    ans[0] = 1;
    for (int i = 0; i < abs_n; i++) {
        cp_qseries(ans, tmp);
        multiply_qseries(tmp, s0, ans);
    }
}

/*******************************************************************\
 * Miscellaneous                                                   *
 *******************************************************************/

void product_side(int mod, const int cong[mod], qseries_t ans)
{
    qseries_t tmp, nxt;
    init_qseries(ans, 0);
    ans[0] = 1;
    for (int n = 1; n < MAXORD; n++) {
        init_qseries(tmp, 0);
        tmp[0] = 1;
        tmp[n] = -1;
        pow_qseries(tmp, cong[n % mod], nxt);
        cp_qseries(ans, tmp);
        multiply_qseries(tmp, nxt, ans);
    }
}

