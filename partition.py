# partition.py - Generate all partitions.
#
# Author:   Debajyoti Nandi <debajyoti.nandi@gmail.com>
# Created:  2016-07-05
# Modified: 2016-07-09
# License:  MIT License (see LICENSE.txt)
#
# Algorithms:
#   [Kelleher]
#      - rule_asc (ascending composition, lexicographically inc)
#      - rule_desc (descending composition, lexicographic dec)
#      - accel_asc (ascending, lexicographically inc)
#      - accel_desc (descending, reverse lexicographically dec)
#
#   [Merca]
#      - merca1 (ascending, lexicographically inc)
#      - merca2 (ascending, lexicographically inc)
#      - merca3 (ascending, lexicographically inc)
#
#   [Zoghbi-Stojmenovic]
#      - zs1 (descending composition, lexicographically dec)
#      - zs2 (descending composition, lexicographically inc)

#####################################################################

# Kelleher's Algorithms (ascending and descending compositions).
#
# References:
#  [1]: Jerome Kelleher,
#       "Generating partitions as ascending compositions",
#       PhD thesis, University College Cork, 2006,
#       http://jeromekelleher.net/downloads/k06.pdf
#
#  [2]: Jerome Kelleher and Barry O'Sullivan,
#       "Generating all partitions: A comparison of two encodings",
#       ArXiv:0909.2331, 2009,
#       https://arxiv.org/abs/0909.2331

def rule_asc(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield [];
        raise StopIteration
    a = [0 for _ in xrange(n+1)]
    a[1] = n
    k = 1
    while k != 0:
        y = a[k] - 1
        k -= 1
        x = a[k] + 1
        while x <= y:
            a[k] = x
            y -= x
            k += 1
        a[k] = x + y
        yield a[:k+1]

def rule_desc(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield [];
        raise StopIteration
    d = [0 for _ in xrange(n+1)]
    d[0] = n
    k = 0
    yield d[:1]
    while k != n-1:
        l = k
        m = d[k]
        while m == 1:
            k -= 1
            m = d[k]
        n1 = m + l - k
        m -= 1
        while m < n1:
            d[k] = m
            n1 -= m
            k += 1
        d[k] = n1
        yield d[:k+1]

def accel_asc(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    a = [0 for _ in xrange(n+1)]
    k = 1
    y = n - 1
    while k != 0:
        k -= 1
        x = a[k] + 1
        while 2*x <= y:
            a[k] = x
            y -= x
            k += 1
        l = k + 1
        while x <= y:
            a[k] = x
            a[l] = y
            yield a[:l+1]
            x += 1
            y -= 1
        y += x - 1
        a[k] = y + 1
        yield a[:k+1]

def accel_desc(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    elif n == 1:
        yield [1]
        raise StopIteration
    d = [1 for _ in xrange(n+2)]
    d[0] = n
    yield d[:1]
    k = q = 0
    while q != -1:
        if d[q] == 2:
            k += 1
            d[q] = 1
            q -= 1
        else:
            m = d[q] - 1
            n1 = k - q + 1
            d[q] = m
            while n1 >= m:
                q += 1
                d[q] = m
                n1 -= m
            if n1 == 0:
                k = q
            else:
                k = q + 1
                if n1 > 1:
                    q += 1
                    d[q] = n1
        yield d[:k+1]

#####################################################################

# Merca's Algorithms (ascending compositions).
#
# References:
#   [1]: Mircea Merca,
#        "Fast algorithm for generating ascending compositions",
#        J Math Model Algor (2012) 11:89--104,
#        DOI: 10.1007/s10852-011-9168-y,
#        http://link.springer.com/article/10.1007%2Fs10852-011-9168-y

def merca1(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    a = [0 for _ in xrange(n)]
    k = -1
    x = 1
    y = n - 1
    c = True
    while c:
        while 2*x <= y:
            k += 1
            a[k] = x
            y -= x
        while x <= y:
            k += 1
            a[k] = x
            k += 1
            a[k] = y
            yield a[:k+1]
            k -= 2
            x += 1
            y -= 1
        k += 1
        a[k] = x + y
        yield a[:k+1]
        k -= 1
        if k >= 0:
            y += x
            x = a[k]
            k -= 1
            x += 1
            y -= 1
        else:
            c = False

def merca2(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    a = [0 for _ in xrange(n)]
    k = 0
    x = 1
    y = n - 1
    while k >= 0:
        while 2*x <= y:
            a[k] = x
            y -= x
            k += 1
        t = k + 1
        while x <= y:
            a[k] = x
            a[t] = y
            yield a[:t+1]
            x += 1
            y -= 1
        y += x - 1
        a[k] = y + 1
        yield a[:k+1]
        k -= 1
        x = a[k] + 1

def merca3(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    a = [0 for _ in xrange(n)]
    k = 0
    x = 1
    y = n - 1
    while k >= 0:
        while 3*x <= y:
            a[k] = x
            y -= x
            k += 1
        t = k + 1
        u = k + 2
        while 2*x <= y:
            a[k] = x
            a[t] = x
            a[u] = y - x
            yield a[:u+1]
            p = x + 1
            q = y - p
            while p <= q:
                a[t] = p
                a[u] = q
                yield a[:u+1]
                p += 1
                q -= 1
            a[t] = y
            yield a[:t+1]
            x += 1
            y -= 1
        while x <= y:
            a[k] = x
            a[t] = y
            yield a[:t+1]
            x += 1
            y -= 1
        y += x - 1
        a[k] = y + 1
        yield a[:k+1]
        k -= 1
        x = a[k] + 1

#####################################################################

# Zoghbi-Stojmenovic's Algorithms (descending compositions).
#
# References:
#  [1]: Antoine Zoghbi and  Ivan Stomenovic,
#       "Fast algorithms for generating integer partitions",
#       Intern J Computer Math, Vol 70, pp 319--332, 1998,
#       DOI:10.1080/00207169808804755
#       http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.42.1287&rep=rep1&type=pdf

def zs1(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    x = [1 for _ in xrange(n)]
    x[0] = n
    m = 1
    h = 0
    yield x[:1]
    while x[0] != 1:
        if x[h] == 2:
            m += 1
            x[h] = 1
            h -= 1
        else:
            r = x[h] - 1
            t = m - h
            x[h] = r
            while t >= r:
                h += 1
                x[h] = r
                t -= r
            if t == 0:
                m = h + 1
            else:
                m = h + 2
                if t > 1:
                    h += 1
                    x[h] = t
        yield x[:m]

def zs2(n):
    if n < 0:
        raise StopIteration
    elif n == 0:
        yield []
        raise StopIteration
    elif n == 1:
        yield [1]
        raise StopIteration
    x = [1 for _ in xrange(n+1)]
    yield x[1:n+1]
    x[0] = -1
    x[1] = 2
    h = 1
    m = n - 1
    yield x[1:m+1]
    while x[1] != n:
        if m - h > 1:
            h += 1
            x[h] = 2
            m -= 1
        else:
            j = m - 2
            while x[j] == x[m-1]:
                x[j] = 1
                j -= 1
            h = j + 1
            x[h] = x[m-1] + 1
            r = x[m] + x[m-1]*(m - h - 1)
            x[m] = 1
            if m - h > 1:
                x[m-1] = 1
            m = h + r - 1
        yield x[1:m+1]
