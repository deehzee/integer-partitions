/*
 * partnid.c - Check partition identity (multithreaded, simplified).
 *
 * Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created:  2016-07-15
 * Modified: 2016-07-15
 * License:  MIT License (see LICENSE.txt)
 *
 * Compilation Suggestions:
 *   CC = gcc #(or clang)
 *   CFLAGS = -std=gnu11 -Ofast #(or, -O2, -O3)
 *   OBJS = qseries.o util.o
 *   LIBS = -lpthread
 *   $(CC) $(CFLAGS) -o partnid partnid.c $(OBJS) $(LIBS)
 *
 * References:
 *   http://jvns.ca/blog/2014/12/14/fun-with-threads/
 *   https://github.com/jvns/fun-with-threads/
 *   http://www.alexonlinux.com/pthread-mutex-vs-pthread-spinlock
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <pthread.h>
#include "qseries.h"
#include "util.h"

/*******************************************************************\
 * MACRO DEFINITIONS                                               *
\*******************************************************************/

#define MAXN 199
#define MAXNUMP MAXN
#define NUMTHREADS 8

#define PSIDEF pside_new_06
#define FILTER_PARTN filter_new_06
#define GEN_PARTN merca3

/*******************************************************************\
 * PARTITION (HEADER)                                              *
\*******************************************************************/

typedef struct {
    int n;
    size_t len;
    int a[MAXNUMP+1];
} partition_t;

static inline void print_partition(const partition_t *p);
static inline void println_partition(const partition_t *p);
static inline uint64_t merca3(int n);

static inline bool filter_none(const partition_t *p);
static inline bool filter_new_01(const partition_t *p);
static inline bool filter_new_02(const partition_t *p);
static inline bool filter_new_03(const partition_t *p);
static inline bool filter_new_04(const partition_t *p);
static inline bool filter_new_05(const partition_t *p);
static inline bool filter_new_06(const partition_t *p);
static inline bool filter_new_6x(const partition_t *p);
static inline bool filter_new_6y(const partition_t *p);
static inline bool filter_new_07(const partition_t *p);
static inline bool filter_new_08(const partition_t *p);
static inline bool filter_new_09(const partition_t *p);
static inline bool filter_new_10(const partition_t *p);
static inline bool filter_new_11(const partition_t *p);
static inline bool filter_new_12(const partition_t *p);

static inline void pside_none(qseries_t s);
static inline void pside_new_01(qseries_t s);
static inline void pside_new_02(qseries_t s);
static inline void pside_new_03(qseries_t s);
static inline void pside_new_04(qseries_t s);
static inline void pside_new_05(qseries_t s);
static inline void pside_new_06(qseries_t s);
static inline void pside_new_6x(qseries_t s);
static inline void pside_new_6y(qseries_t s);
static inline void pside_new_07(qseries_t s);
static inline void pside_new_08(qseries_t s);
static inline void pside_new_09(qseries_t s);
static inline void pside_new_10(qseries_t s);
static inline void pside_new_11(qseries_t s);
static inline void pside_new_12(qseries_t s);

/*******************************************************************\
 * OTHER (HEADER)                                                  *
\*******************************************************************/

typedef enum {
    E_SUCCESS = 0,
    E_WRONG_NUM_ARGS = 32,
    E_UNKNOWN_COMMAND,
    E_SCAN_FAILURE,
    E_OUT_OF_RANGE,
    E_THREAD_FAILURE,
} error_t;

typedef enum {
    COMMAND_HELP,
    COMMAND_SHOW,
    COMMAND_VERIFY,
} command_t;

typedef enum {
    ACTION_NONE,
    ACTION_PRINT,
} action_t;


static inline void usage(const char *com);
static inline error_t parse_args(
        int argc,
        char *argv[],
        command_t *com_p,
        int *n_p);

static inline void show(int n);
static inline void verify(int N);
static inline void report(int N);

static inline void *run_thread(void *arg);

/*******************************************************************\
 * GLOBAL VARIABLES                                                *
\*******************************************************************/

static int64_t sum_side[MAXN+1];
static qseries_t prod_side;
static int64_t diff[MAXN+1];
static int current;
static action_t action;

/*******************************************************************\
 * FUNCTION DEFINITIONS                                            *
\*******************************************************************/

int main(int argc, char *argv[]) {
#ifdef DEBUG
    fprintf(stderr, "main: entering...\n");
#endif
    command_t command;
    error_t err;
    int n;
    if ((err = parse_args(argc, argv, &command, &n))) {
        usage(argv[0]);
        return err;
    }
#ifdef DEBUG
    fprintf(stderr, "main: command=%d\n", command);
    fprintf(stderr, "main: n=%d\n", n);
#endif
    switch (command) {
        case COMMAND_HELP:
            usage(argv[0]);
            break;
        case COMMAND_SHOW:
            show(n);
            break;
        case COMMAND_VERIFY:
            verify(n);
            break;
        default:
            usage(argv[0]);
            exit(E_UNKNOWN_COMMAND);
    }
#ifdef DEBUG
    fprintf(stderr, "main: exiting...\n");
#endif
    return E_SUCCESS;
}

static inline void usage(const char *com)
{
    fprintf(stderr, "Check partition identities (need to recompile");
    fprintf(stderr, " for different identities).\n\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "  %s [ show N | verify N | help ]\n\n", com);
    fprintf(stderr, "Commands:\n");
    fprintf(stderr, "  show");
    fprintf(stderr, "\t\tShow the sumside for N (0-199).\n");
    fprintf(stderr, "  verify");
    fprintf(stderr, "\tVerify partition identity upto N (0-199).\n");
    fprintf(stderr, "  help\t\tShow this help.\n");
}

static inline error_t parse_args(
        int argc,
        char *argv[],
        command_t *com_p,
        int *n_p)
{
    if (argc < 2) {
        return E_WRONG_NUM_ARGS;
    } else if (strcmp(argv[1], "help") == 0) {
        *com_p = COMMAND_HELP;
    } else if (strcmp(argv[1], "show") == 0) {
        *com_p = COMMAND_SHOW;
        if (argc < 3)
            return E_WRONG_NUM_ARGS;
        if (! sscanf(argv[2], "%d", n_p))
            return E_SCAN_FAILURE;
        if (*n_p < 0 || *n_p > MAXN)
            return E_OUT_OF_RANGE;
    } else if (strcmp(argv[1], "verify") == 0) {
        *com_p = COMMAND_VERIFY;
        if (argc < 3)
            return E_WRONG_NUM_ARGS;
        if (! sscanf(argv[2], "%d", n_p))
            return E_SCAN_FAILURE;
        if (*n_p < 0 || *n_p > MAXN)
            return E_OUT_OF_RANGE;
    } else {
        return E_UNKNOWN_COMMAND;
    }
    return E_SUCCESS;
}

static inline void show(int n)
{
#ifdef DEBUG
    fprintf(stderr, "show(n=%d): entering...\n", n);
#endif
    PSIDEF(prod_side);
    action = ACTION_PRINT;
    GEN_PARTN(n);
    printf("\n");
    diff[n] = sum_side[n] - prod_side[n];
    printf("n=%d  s(n)=%" PRId64 "  p(n)=%" PRId64 "  diff=%" PRId64 "\n",
            n, sum_side[n], prod_side[n], diff[n]);
#ifdef DEBUG
    fprintf(stderr, "show(n=%d): exiting...\n", n);
#endif
}

static inline void verify(int N)
{
#ifdef DEBUG
    fprintf(stderr, "verify(N=%d): entering...\n", N);
#endif
    PSIDEF(prod_side);
    action = ACTION_NONE;
    current = N;
    pthread_t threads[NUMTHREADS];
    int ids[NUMTHREADS];
    for (int t = 0; t < NUMTHREADS; t++) {
        ids[t] = t;
        if (pthread_create(&threads[t], NULL, run_thread, &ids[t])) {
            fprintf(stderr, "[ERR] Failed to create thread!\n");
            exit(E_THREAD_FAILURE);
        }
    }
    for (int t = 0; t < NUMTHREADS; t++)
        pthread_join(threads[t], NULL);
    report(N);
#ifdef DEBUG
    fprintf(stderr, "verify(N=%d): exiting...\n", N);
#endif
}

static inline void report(int N)
{
#ifdef DEBUG
    fprintf(stderr, "report(N=%d): entering...\n", N);
#endif
    printf("  %3s %13s %13s %13s\n", "n", "s(n)", "p(n)", "diff");
    for (int i = 0; i < 47; i++)
        printf("=");
    printf("\n");
    for (int n = 0; n <= N; n++) {
        if ((diff[n] = sum_side[n] - prod_side[n]))
            printf("**");
        else
            printf("  ");
        printf("%3d %13" PRId64 " %13" PRId64 " %13" PRId64 "\n",
               n, sum_side[n], prod_side[n], diff[n]);
    }
#ifdef DEBUG
    fprintf(stderr, "report(N=%d): exiting...\n", N);
#endif
}

static inline void *run_thread(void *arg)
{
    int id = *((int *) arg);
    int n;
#ifdef DEBUG
    fprintf(stderr, "run_thread [thr#%02d]: entering...\n", id);
#endif
    while ((n = __sync_fetch_and_sub(&current, 1)) >= 0) {
        GEN_PARTN(n);
    }
#ifdef DEBUG
    fprintf(stderr, "run_thread [thr#%02d]: exiting...\n", id);
#endif
    pthread_exit(NULL);
}

/*******************************************************************\
 * PARTITION (DEFINITIONS)                                         *
\*******************************************************************/

static inline void print_partition(const partition_t *p)
{
    printf("[");
    for (int i = 0; i < p->len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", p->a[i]);
    }
    printf("]");
}

static inline void println_partition(const partition_t *p)
{
    printf("[");
    for (int i = 0; i < p->len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", p->a[i]);
    }
    printf("]\n");
}

static inline uint64_t merca3(int n)
{
    uint64_t count = 0;
    int k, r, s, t, u, x, y;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (FILTER_PARTN(&p)) {
            if (action == ACTION_PRINT)
                println_partition(&p);
            sum_side[n]++;
        }
        return count;
    }
    memset(p.a, 0, (MAXNUMP + 1) * sizeof(int));
    k = 0;
    x = 1;
    y = n - 1;
    while (k >= 0) {
        while (3*x <= y) {
            p.a[k] = x;
            y -= x;
            k++;
        }
        t = k + 1;
        u = k + 2;
        while (2*x <= y) {
            p.a[k] = x;
            p.a[t] = x;
            p.a[u] = y - x;
            p.len = u + 1;
            if (FILTER_PARTN(&p)) {
                if (action == ACTION_PRINT)
                    println_partition(&p);
                sum_side[n]++;
            }
            r = x + 1;
            s = y - r;
            while (r <= s) {
                p.a[t] = r;
                p.a[u] = s;
                p.len = u + 1;
                if (FILTER_PARTN(&p)) {
                    if (action == ACTION_PRINT)
                        println_partition(&p);
                    sum_side[n]++;
                }
                r++;
                s--;
            }
            p.a[t] = y;
            p.len = t + 1;
            if (FILTER_PARTN(&p)) {
                if (action == ACTION_PRINT)
                    println_partition(&p);
                sum_side[n]++;
            }
            x++;
            y--;
        }
        while (x <= y) {
            p.a[k] = x;
            p.a[t] = y;
            p.len = t + 1;
            if (FILTER_PARTN(&p)) {
                if (action == ACTION_PRINT)
                    println_partition(&p);
                sum_side[n]++;
            }
            x++;
            y--;
        }
        y += x - 1;
        p.a[k] = y + 1;
        p.len = k + 1;
        if (FILTER_PARTN(&p)) {
            if (action == ACTION_PRINT)
                println_partition(&p);
            sum_side[n]++;
        }
        k--;
        x = p.a[k] + 1;
    }
    return count;
}

/*******************************************************************\
 * None (verified, n <= 100)                                       *
\*******************************************************************/

static inline void pside_none(qseries_t s)
{
    int mod = 1;
    int cong[1] = {-1};
    product_side(mod, cong, s);
}

static inline bool filter_none(const partition_t *p)
{
    return true;
}

/*******************************************************************\
 * New-01 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_01(qseries_t s)
{
    /* Forbidden: parts cong to 3 (mod 4) */
    int mod = 4;
    int cong[4] = {-1, -1, -1, 0};
    product_side(mod, cong, s);
}

static inline bool filter_new_01(const partition_t *p)
{
    /* Forbidden: diff@1 = 1 for sum@2 cong to 3 (mod 4) */
    if (p->len < 2)
        return true;
    int cur = 0;    /* current */
    int prv = 0;    /* previous */
    int s2 = 0;     /* running sum over 2 elements */
    int d1 = -1;    /* difference at distance 1 */
    for (size_t i = 0; i < p->len; i++) {
        s2 -= prv;
        prv = cur;
        cur = p->a[i];
        s2 += cur;
        d1 = cur - prv;
        if (i >= 1 && d1 == 1 && s2 % 4 == 3)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-02 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_02(qseries_t s)
{
    /* Forbidden: parts cong to 3, 5 (mod 6) */
    int mod = 6;
    int cong[6] = {-1, -1, -1, 0, -1, 0};
    product_side(mod, cong, s);
}

static inline bool filter_new_02(const partition_t *p)
{
    /* Forbidden: diff@1 = 1 for sum@2 cong to 3 (mod 4) *
     *            diff@1 = 3 for sum@2 cong to 1 (mod 4) */
    if (p->len < 2)
        return true;
    int cur = 0;    /* current */
    int prv = 0;    /* previous */
    int s2 = 0;     /* running sum over 2 elements */
    int d1 = -1;    /* difference at distance 1 */
    for (size_t i = 0; i < p->len; i++) {
        s2 -= prv;
        prv = cur;
        cur = p->a[i];
        s2 += cur;
        d1 = cur - prv;
        if (i >= 1 && ((d1 == 1 && s2 % 4 == 3) ||
                        (d1 == 3 && s2 % 4 == 1)))
                return false;
    }
    return true;
}

/*******************************************************************\
 * New-03 (false-positive, first discrepancy @n=13)                *
\*******************************************************************/

static inline void pside_new_03(qseries_t s)
{
    /* Forbidden: parts cong to 3, 5, 10 (mod 10) */
    int mod = 10;
    int cong[10] = {0, -1, -1, 0, -1, 0, -1, -1, -1, -1};
    product_side(mod, cong, s);
}

static inline bool filter_new_03(const partition_t *p)
{
    /* Forbidden: diff@1 = 0, 1 for sum@2 cong to 0 (mod 5) *
     *            diff@3 = 0, 1 for sum@3 cong to 3 (mod 5) */
    if (p->len < 2)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s2 = 0;     /* running sum over 2 elements */
    int s3 = 0;     /* running sum over 3 elements */
    int d1 = -1;    /* difference at distance 1 */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s2 -= pv[0];
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s2 += cur;
        s3 += cur;
        d1 = cur - pv[0];
        d2 = cur - pv[1];
        if ((i >= 1 && d1 < 2 && s2 % 5 == 0) ||
            (i >= 2 && d2 < 2 && s3 % 5 == 3))
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-04 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_04(qseries_t s)
{
    /* Forbidden: parts cong to 1 (mod 5) */
    int mod = 5;
    int cong[5] = {-1, 0, -1, -1, -1};
    product_side(mod, cong, s);
}

static inline bool filter_new_04(const partition_t *p)
{
    /* Forbidden: diff@2 = 0, 1 for sum@3 cong to 1 (mod 5) *
     *            IC: (1)                                   */
    if (p->len > 0 && p->a[0] == 1)
        return false;
    if (p->len < 3)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s3 = 0;     /* running sum over 3 elements */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s3 += cur;
        d2 = cur - pv[1];
        if (i >= 2 && d2 < 2 && s3 % 5 == 1)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-05 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_05(qseries_t s)
{
    /* Forbidden: parts cong to 2 (mod 5) */
    int mod = 5;
    int cong[5] = {-1, -1, 0, -1, -1};
    product_side(mod, cong, s);
}

static inline bool filter_new_05(const partition_t *p)
{
    /* Forbidden: diff@2 = 0, 1 for sum@3 cong to 2 (mod 5) *
     *            IC: (1,1)                                 */
    if (p->len > 1 && p->a[0] == 1 && p->a[1] == 1)
        return false;
    if (p->len < 3)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s3 = 0;     /* running sum over 3 elements */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s3 += cur;
        d2 = cur - pv[1];
        if (i >= 2 && d2 < 2 && s3 % 5 == 2)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-06 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_06(qseries_t s)
{
    /* Forbidden: parts cong to 3 (mod 5) */
    int mod = 5;
    int cong[5] = {-1, -1, -1, 0, -1};
    product_side(mod, cong, s);
}

static inline bool filter_new_06(const partition_t *p)
{
    /* Forbidden: diff@2 = 0, 1 for sum@3 cong to 3 (mod 5) *
     *            IC: none                                  */
    if (p->len < 3)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s3 = 0;     /* running sum over 3 elements */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s3 += cur;
        d2 = cur - pv[1];
        if (i >= 2 && d2 < 2 && s3 % 5 == 3)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-6x (verified, n <= 100)                                     *
 *        (The missing on in the series - I4, I5, I6, I6x)         *
\*******************************************************************/

static inline void pside_new_6x(qseries_t s)
{
    /* Forbidden: parts cong to 4 (mod 5) */
    int mod = 5;
    int cong[5] = {-1, -1, -1, -1, 0};
    product_side(mod, cong, s);
}

static inline bool filter_new_6x(const partition_t *p)
{
    /* Forbidden: diff@2 = 0, 1 for sum@3 cong to 4 (mod 5) *
     *            IC: none                                  */
    if (p->len < 3)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s3 = 0;     /* running sum over 3 elements */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s3 += cur;
        d2 = cur - pv[1];
        if (i >= 2 && d2 < 2 && s3 % 5 == 4)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-6y (verified, n <= 100)                                     *
 *        (The missing on in the series - I4, I5, I6, I6x, I6y)    *
\*******************************************************************/

static inline void pside_new_6y(qseries_t s)
{
    /* Forbidden: parts cong to 0 (mod 5) */
    int mod = 5;
    int cong[5] = {0, -1, -1, -1, -1};
    product_side(mod, cong, s);
}

static inline bool filter_new_6y(const partition_t *p)
{
    /* Forbidden: diff@2 = 0, 1 for sum@3 cong to 0 (mod 5) *
     *            IC: none                                  */
    if (p->len < 3)
        return true;
    int cur = 0;    /* current */
    int pv[2]       /* previous 2 elements */
        = {0, 0};
    int s3 = 0;     /* running sum over 3 elements */
    int d2 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s3 -= pv[1];
        pv[1] = pv[0];
        pv[0] = cur;
        cur = p->a[i];
        s3 += cur;
        d2 = cur - pv[1];
        if (i >= 2 && d2 < 2 && s3 % 5 == 0)
            return false;
    }
    return true;
}

/*******************************************************************\
 * New-07 (verified, n <= 100)                                     *
\*******************************************************************/

static inline void pside_new_07(qseries_t s)
{
    /* Forbidden: parts cong to 1, 5, 6, 7, 11 (mod 12) */
    int mod = 12;
    int cong[12] = {-1, 0, -1, -1, -1, 0, 0, 0, -1, -1, -1, 0};
    product_side(mod, cong, s);
}

static inline bool filter_new_07(const partition_t *p)
{
    /* Forbidden: diff@1 = 2    for sum@2 cong to 0 (mod 4) *
     *            diff@1 = 1    for sum@2 cong to 1 (mod 4) *
     *            diff@1 = 0, 4 for sum@2 cong to 2 (mod 4) *
     *            diff@1 = 1    for sum@2 cong to 3 (mod 4) *
     *            IC: (1)                                   */
    if (p->len > 0 && p->a[0] == 1)
        return false;
    if (p->len < 2)
        return true;
    int cur = 0;    /* current */
    int prv = 0;    /* previous */
    int s2 = 0;     /* running sum over 3 elements */
    int d1 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s2 -= prv;
        prv = cur;
        cur = p->a[i];
        s2 += cur;
        d1 = cur - prv;
        if (i > 0)
            if ((d1 == 2 && s2 % 4 == 0) ||
                (d1 == 1 && s2 % 4 == 1) ||
                (d1 == 0 && s2 % 4 == 2) ||
                (d1 == 4 && s2 % 4 == 2) ||
                (d1 == 1 && s2 % 4 == 3))
                return false;
    }
    return true;
}

/*******************************************************************\
 * New-08                                                          *
\*******************************************************************/

static inline void pside_new_08(qseries_t s)
{
    /* Forbidden: parts cong to 1, 5, 6, 7, 11 (mod 12) */
    int mod = 12;
    int cong[12] = {-1, 0, -1, -1, -1, 0, 0, 0, -1, -1, -1, 0};
    product_side(mod, cong, s);
}

static inline bool filter_new_08(const partition_t *p)
{
    /* Forbidden: diff@1 = 2    for sum@2 cong to 0 (mod 4) *
     *            diff@1 = 1    for sum@2 cong to 1 (mod 4) *
     *            diff@1 = 0, 4 for sum@2 cong to 2 (mod 4) *
     *            diff@1 = 1    for sum@2 cong to 3 (mod 4) *
     *            IC: (1)                                   */
    if (p->len > 0 && p->a[0] == 1)
        return false;
    if (p->len < 2)
        return true;
    int cur = 0;    /* current */
    int prv = 0;    /* previous */
    int s2 = 0;     /* running sum over 3 elements */
    int d1 = -1;    /* difference at distance 2 */
    for (size_t i = 0; i < p->len; i++) {
        s2 -= prv;
        prv = cur;
        cur = p->a[i];
        s2 += cur;
        d1 = cur - prv;
        if (i > 0)
            if ((d1 == 2 && s2 % 4 == 0) ||
                (d1 == 1 && s2 % 4 == 1) ||
                (d1 == 0 && s2 % 4 == 2) ||
                (d1 == 4 && s2 % 4 == 2) ||
                (d1 == 1 && s2 % 4 == 3))
                return false;
    }
    return true;
}

