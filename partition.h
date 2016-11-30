/*
 * partition.h - Partition generation and utilities (header file).
 *
 * Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created:  2016-07-05
 * Modified: 2016-07-20
 * License:  MIT License (see LICENSE.txt)
 *
 * Algorithms:
 *   [Kelleher]
 *      - rule_asc (ascending composition, lexicographically inc)
 *      - rule_desc (descending composition, lexicographic dec)
 *      - accel_asc (ascending, lexicographically inc)
 *      - accel_desc (descending, reverse lexicographically dec)
 *
 *   [Merca]
 *      - merca1 (ascending, lexicographically inc)
 *      - merca2 (ascending, lexicographically inc)
 *      - merca3 (ascending, lexicographically inc)
 *
 *   [Zoghbi-Stojmenovic]
 *      - zs1 (descending composition, lexicographically dec)
 *      - zs2 (descending composition, lexicographically inc)
 */

#pragma once

#include <stdint.h>

/*******************************************************************\
 *  Constants and Types                                            *
\*******************************************************************/

/* The maximum number to be partitioned. */
#define MAXNUMP 200

/* Data type for partitions. */
typedef struct {
    int n;
    size_t len;
    int a[MAXNUMP+1];
} partition_t;

/*
 * Visitor:
 *   A function type for visitor fucntions that gets called each
 *   time a partition is created.
 *   Arguments and/or results to this function can be passed
 *   using the generic pointer `argres`.
 */
typedef void partn_visitor_f(const partition_t *p, void *argres);

/*
 * Generator:
 *   This is the function type for a partition generating function.
 *   `n`      - The number to be partitioned
 *   `visit`  - A visitor function.
 *   `argres` - A generic pointer for argument/result for the
 *              visitor.
 */
typedef uint64_t partn_generator_f(int n,
       partn_visitor_f *visit, void *argres);


/*******************************************************************\
 *  Utilities                                                      *
\*******************************************************************/

/* Print a partition. */
void print_partition(const partition_t *p);

/* Print a partition with a newline. */
void println_partition(const partition_t *p);

/* Initialize a partition with an initial value for all parts. */
void init_partition(partition_t *p, int initial_val);

/*
 * Make a partition of length `len` by copying `len` elements of
 * array `a` from the index `start`.  It is assumed that `a` is
 * sorted (in either direction).
 */
void mk_partition(partition_t *p, const int a[],
                                size_t start, size_t len);

/* Copy partition `p` into `q`. */
void cp_partition(const partition_t *p, partition_t *q);


/*******************************************************************\
 *  Partition Generators                                           *
\*******************************************************************/

/*
 * Kelleher's Algorithms (ascending and descending compositions).
 *
 * References:
 *  [1]: Jerome Kelleher,
 *       "Generating partitions as ascending compositions",
 *       PhD thesis, University College Cork, 2006,
 *       http://jeromekelleher.net/downloads/k06.pdf
 *
 *  [2]: Jerome Kelleher and Barry O'Sullivan,
 *       "Generating all partitions: A comparison of two encodings",
 *       ArXiv:0909.2331, 2009,
 *       https://arxiv.org/abs/0909.2331
 */
uint64_t rule_asc(int n, partn_visitor_f *visit, void *argres);
uint64_t rule_desc(int n, partn_visitor_f *visit, void *argres);
uint64_t accel_asc(int n, partn_visitor_f *visit, void *argres);
uint64_t accel_desc(int n, partn_visitor_f *visit, void *argres);

/*
 * Merca's Algorithms (ascending compositions).
 *
 * References:
 *  [1]: Mircea Merca,
 *       "Fast algorithms for generating ascending compositions",
 *       J Math Model Algor (2012) 11:89--104, 2012
 *       DOI:10.1007/s10852-011-9168-y
 */
uint64_t merca1(int n, partn_visitor_f *visit, void *argres);
uint64_t merca2(int n, partn_visitor_f *visit, void *argres);
uint64_t merca3(int n, partn_visitor_f *visit, void *argres);

/*
 * Zoghbi-Stojmenovic's algorithms (descending compositions).
 *
 * References:
 *  [1]: Antoine Zoghbi and Ivan Stojmenovic,
 *       "Fast Algorithms for generating integer partitions",
 *       Intern J Computer Math, Vol 70, pp 319--332, 1998,
 *       DOI:10.1080/00207169808804755
 *       http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.42.1287&rep1&type=pdf
 */
uint64_t zs1(int n, partn_visitor_f *visit, void *argres);
uint64_t zs2(int n, partn_visitor_f *visit, void *argres);
