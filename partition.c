/*
 * partition.c - Partition generation and utilities.
 *
 * Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
 * Created:  2016-07-05
 * Modified: 2016-07-13
 * License:  MIT License (see LICENSE.txt)
 *
 * Compilation Suggestions:
 *   CC = gcc #( or clang)
 *   CFLAGS = -std=gnu11 -O3 #(or, -O2)
 *   $(CC) $(CFLAGS) -c partition.c
 */

#include <stdio.h>
#include <string.h>
/*#include <stdlib.h>*/
#include <stdbool.h>
/*#include <inttypes.h>*/
#include "partition.h"

/*******************************************************************\
 *  Utilities                                                      *
\*******************************************************************/

void print_partition(const partition_t *p)
{
    printf("[");
    for (size_t i = 0; i < p->len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", p->a[i]);
    }
    printf("]");
}

void println_partition(const partition_t *p)
{
    printf("[");
    for (size_t i = 0; i < p->len; i++) {
        if (i != 0)
            printf(", ");
        printf("%d", p->a[i]);
    }
    printf("]");
    printf("\n");
}

void init_partition(partition_t *p, int initial_val)
{
    for (size_t i = 0; i <= MAXNUMP; i++)
        p->a[i] = initial_val;
}

void mk_partition(partition_t *p, const int a[],
        size_t start, size_t len)
{
    for (size_t i = 0; i < len; i++)
        p->a[i] = a[start + i];
    p->len = len;
}

void cp_partition(const partition_t *p, partition_t *q)
{
    memcpy(q, p, sizeof(partition_t));
}


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

uint64_t rule_asc(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, x, y;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
    p.a[1] = n;
    k = 1;
    while (k != 0) {
        y = p.a[k] - 1;
        k--;
        x = p.a[k] + 1;
        while (x <= y) {
            p.a[k] = x;
            y -= x;
            k++;
        }
        p.a[k] = x + y;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}

uint64_t rule_desc(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, l, m, n1;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
    p.a[0] = n;
    p.len = 1;
    if (visit)
        visit(&p, argres);
    count++;
    k = 0;
    while (k != n-1) {
        l = k;
        m = p.a[k];
        while (m == 1) {
            k--;
            m = p.a[k];
        }
        n1 = m + l - k;
        m--;
        while (m < n1) {
            p.a[k] = m;
            n1 -= m;
            k++;
        }
        p.a[k] = n1;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}

uint64_t accel_asc(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, l, x, y;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
    k = 1;
    y = n - 1;
    while (k != 0) {
        k--;
        x = p.a[k] + 1;
        while (2*x <= y) {
            p.a[k] = x;
            y -= x;
            k++;
        }
        l = k + 1;
        while (x <= y) {
            p.a[k] = x;
            p.a[l] = y;
            p.len = l + 1;
            if (visit)
                visit(&p, argres);
            count++;
            x++;
            y--;
        }
        y += x - 1;
        p.a[k] = y + 1;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}

uint64_t accel_desc(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, m, n1, q;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    } else if (n == 1) {
        p.a[0] = 1;
        p.len = 1;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 1);
    p.a[0] = n;
    p.len = 1;
    if (visit)
        visit(&p, argres);
    count++;
    k = q = 0;
    while (q != -1) {
        if (p.a[q] == 2) {
            k++;
            p.a[q] = 1;
            q--;
        } else {
            m = p.a[q] - 1;
            n1 = k - q + 1;
            p.a[q] = m;
            while (n1 >= m) {
                q++;
                p.a[q] = m;
                n1 -= m;
            }
            if (n1 == 0) {
                k = q;
            } else {
                k = q + 1;
                if (n1 > 1) {
                    q++;
                    p.a[q] = n1;
                }
            }
        }
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}


/*
 * Merca's Algorithms (ascending compositions).
 *
 * References:
 *  [1]: Mircea Merca,
 *       "Fast algorithms for generating ascending compositions",
 *       J Math Model Algor (2012) 11:89--104, 2012
 *       DOI:10.1007/s10852-011-9168-y
 */

uint64_t merca1(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, x, y;
    bool c;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
    k = -1;
    x = 1;
    y = n - 1;
    c = true;
    while (c) {
        while (2*x <= y) {
            k++;
            p.a[k] = x;
            y -= x;
        }
        while (x <= y) {
            k++;
            p.a[k] = x;
            k++;
            p.a[k] = y;
            p.len = k + 1;
            if (visit)
                visit(&p, argres);
            count++;
            k -= 2;
            x++;
            y--;
        }
        k++;
        p.a[k] = x + y;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
        k--;
        if (k >= 0) {
            y += x;
            x = p.a[k];
            k--;
            x++;
            y--;
        } else {
            c = false;
        }
    }
    return count;
}

uint64_t merca2(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, t, x, y;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
    k = 0;
    x = 1;
    y = n - 1;
    while (k >= 0) {
        while (2*x <= y) {
            p.a[k] = x;
            y -= x;
            k++;
        }
        t = k + 1;
        while (x <= y) {
            p.a[k] = x;
            p.a[t] = y;
            p.len = t + 1;
            if (visit)
                visit(&p, argres);
            count++;
            x++;
            y--;
        }
        y += x - 1;
        p.a[k] = y + 1;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
        k--;
        x = p.a[k] + 1;
    }
    return count;
}

uint64_t merca3(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int k, r, s, t, u, x, y;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 0);
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
            if (visit)
                visit(&p, argres);
            count++;
            r = x + 1;
            s = y - r;
            while (r <= s) {
                p.a[t] = r;
                p.a[u] = s;
                p.len = u + 1;
                if (visit)
                    visit(&p, argres);
                count++;
                r++;
                s--;
            }
            p.a[t] = y;
            p.len = t + 1;
            if (visit)
                visit(&p, argres);
            count++;
            x++;
            y--;
        }
        while (x <= y) {
            p.a[k] = x;
            p.a[t] = y;
            p.len = t + 1;
            if (visit)
                visit(&p, argres);
            count++;
            x++;
            y--;
        }
        y += x - 1;
        p.a[k] = y + 1;
        p.len = k + 1;
        if (visit)
            visit(&p, argres);
        count++;
        k--;
        x = p.a[k] + 1;
    }
    return count;
}


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

uint64_t zs1(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int h, m, r, t;
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    init_partition(&p, 1);
    p.a[0] = n;
    m = 1;
    h = 0;
    p.len = 1;
    if (visit)
        visit(&p, argres);
    count++;
    while (p.a[0] != 1) {
        if (p.a[h] == 2) {
            m++;
            p.a[h] = 1;
            h--;
        } else {
            r = p.a[h] - 1;
            t = m - h;
            p.a[h] = r;
            while (t >= r) {
                h++;
                p.a[h] = r;
                t -= r;
            }
            if (t == 0) {
                m = h + 1;
            } else {
                m = h + 2;
                if (t > 1) {
                    h++;
                    p.a[h] = t;
                }
            }
        }
        p.len = m;
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}

uint64_t zs2(int n, partn_visitor_f *visit, void *argres)
{
    uint64_t count = 0;
    int h, j, m, r;
    int x[MAXNUMP+1];
    partition_t p;

    p.n = n;
    if (n < 0) {
        return count;
    } else if (n == 0) {
        p.len = 0;
        if (visit)
            visit(&p, argres);
        return ++count;
    } else if (n == 1) {
        p.len = 1;
        p.a[0] = 1;
        if (visit)
            visit(&p, argres);
        return ++count;
    }
    for (int i = 1; i < n+1; i++)
        x[i] = 1;
    mk_partition(&p, x, 1, n);
    if (visit)
        visit(&p, argres);
    count++;
    x[0] = -1;
    x[1] = 2;
    h = 1;
    m = n - 1;
    mk_partition(&p, x, 1, m);
    if (visit)
        visit(&p, argres);
    count++;
    while (x[1] != n) {
        if (m - h > 1) {
            h++;
            x[h] = 2;
            m --;
        } else {
            j = m - 2;
            while (x[j] == x[m-1]) {
                x[j] = 1;
                j--;
            }
            h = j + 1;
            x[h] = x[m-1] + 1;
            r = x[m] + x[m-1]*(m - h - 1);
            x[m] = 1;
            if (m - h > 1)
                x[m-1] = 1;
            m = h + r - 1;
        }
        mk_partition(&p, x, 1, m);
        if (visit)
            visit(&p, argres);
        count++;
    }
    return count;
}
