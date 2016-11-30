/*
 * genpartn.c - Generate partitions.
 *
 * Author: Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created: 2016-07-05
 * Modified: 2016-07-13
 * License: MIT License (see LICENSE.txt)
 *
 * Compilation Suggestions:
 *   CC = gcc #(or clang)
 *   CFLAGS = -std=gnu11 -O3 #(or, -O2)
 *   OBJS = partition.o
 *   $(CC) $(CFLAGS) -o genpartn genpartn.c $(OBJS)
 *
 * Usage: ./genpartn ALGORITHM ACTION N
 *
 *   ALGORITHM   Algorithm to generate partitions (rule_asc,
 *               rule_desc, accel_asc, accel_desc, merca1, merca2,
 *               merca3, zs1, or zs2).
 *   ACTION      Action for each partition (none, or print).
 *   N           The number to be partitioned.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include "partition.h"

/* Type: Algorithms */
typedef enum {
    E_SUCCESS,
    E_WRONG_NUM_ARGS,
    E_INVALID_N,
    E_INVALID_METHOD,
    E_INVALID_ACTION,
} err_t;

/* Type: algorithms */
typedef enum {
    ALGO_RULE_ASC,
    ALGO_RULE_DESC,
    ALGO_ACCEL_ASC,
    ALGO_ACCEL_DESC,
    ALGO_MERCA1,
    ALGO_MERCA2,
    ALGO_MERCA3,
    ALGO_ZS1,
    ALGO_ZS2,
} algo_t;

/* Type: Actions */
typedef enum {
    ACTION_NONE,
    ACTION_PRINT,
} action_t;

/* Print usage info. */
static inline void usage(const char *command);

/* Parse command line arguments. */
static inline err_t parse_args(
        char *argv[],
        algo_t *algp,
        action_t *axnp,
        int *np);

/* Print (with newline) a visited partition. Ignore `argres`. */
static inline void partn_println(const partition_t *p, void *argres);

partn_visitor_f *visitors[] = {
    NULL,
    partn_println,
};

partn_generator_f *generators[] = {
    rule_asc,
    rule_desc,
    accel_asc,
    accel_desc,
    merca1,
    merca2,
    merca3,
    zs1,
    zs2,
};

int main(int argc, char *argv[])
{
    int n;
    uint64_t count;
    algo_t algo;
    action_t action;
    err_t error;

    if (argc != 4) {
        usage(argv[0]);
        exit(E_WRONG_NUM_ARGS);
    }
    if ((error = parse_args(argv, &algo, &action, &n))) {
        fprintf(stderr, "\n");
        usage(argv[0]);
        exit(error);
    }

    printf("n = %d\n", n);
    if (action == ACTION_PRINT)
        printf("\n");
    count = generators[algo](n, visitors[action], NULL);
    if (action == ACTION_PRINT)
        printf("\n");
    printf("p[%d] = %" PRIu64 "\n", n, count);
    return 0;
}

static inline void usage(const char *com)
{
    fprintf(stderr, "Generate all partitions of N (0-199).\n\n");
    fprintf(stderr, "Usage: %s ALGORITHM ACTION N\n\n", com);
    fprintf(stderr, "  ALGORITHM\tAlgorithm to generate partitions ");
    fprintf(stderr, "(rule_asc, rule_desc,\n\t\taccel_asc, ");
    fprintf(stderr, "accel_desc, merca1, merca2, merca3, zs1, ");
    fprintf(stderr, "or zs2).\n");
    fprintf(stderr, "  ACTION\tAction for each partition ");
    fprintf(stderr, "(none, or print).\n");
    fprintf(stderr, "  N\t\tThe number to be partitioned.\n");
}

static inline err_t parse_args(
        char *argv[],
        algo_t *algp,
        action_t *axnp,
        int *np)
{
    if (strcmp(argv[1], "rule_asc") == 0) {
        *algp = ALGO_RULE_ASC;
    } else if (strcmp(argv[1], "rule_desc") == 0) {
        *algp = ALGO_RULE_DESC;
    } else if (strcmp(argv[1], "accel_asc") == 0) {
        *algp = ALGO_ACCEL_ASC;
    } else if (strcmp(argv[1], "accel_desc") == 0) {
        *algp = ALGO_ACCEL_DESC;
    } else if (strcmp(argv[1], "merca1") == 0) {
        *algp = ALGO_MERCA1;
    } else if (strcmp(argv[1], "merca2") == 0) {
        *algp = ALGO_MERCA2;
    } else if (strcmp(argv[1], "merca3") == 0) {
        *algp = ALGO_MERCA3;
    } else if (strcmp(argv[1], "zs1") == 0) {
        *algp = ALGO_ZS1;
    } else if (strcmp(argv[1], "zs2") == 0) {
        *algp = ALGO_ZS2;
    } else {
        fprintf(stderr, "[Error] Invalid METHOD.\n");
        return E_INVALID_METHOD;
    }

    if (strcmp(argv[2], "none") == 0) {
        *axnp = ACTION_NONE;
    } else if (strcmp(argv[2], "print") == 0) {
        *axnp = ACTION_PRINT;
    } else {
        fprintf(stderr, "[Error] Invalid ACTION.\n");
        return E_INVALID_ACTION;
    }

    if (! sscanf(argv[3], "%d", np)) {
        fprintf(stderr, "[Error] Bad argument for N.\n");
        return E_INVALID_N;
    }
    if (*np < 0) {
        fprintf(stderr, "[Error] Invalid value for N.\n");
        return E_INVALID_N;
    }
    return E_SUCCESS;
}

static inline void partn_println(const partition_t *p, void *argres)
{
    printf("[");
    for (int i = 0; i < p->len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", p->a[i]);
    }
    printf("]");
    printf("\n");
}
