# Programs Related to Partitions and q-Series

## Table of Contents

* Generating Partitions
* q-Series and Other Utilities
* Compilation and Benchmark
* TODO


## Generating Partitions

Various algorithms to generate all integer partitions either in
ascending or descending composition.


### Files:

* `partition.h`   Header file for C
* `partition.c`   Partition programs in C
* `partitions.py` Partition programs in Python
* `partition-examples.ipynb`  Python example as a jupyter notebook.


### Algorithms:

* Kellher (2006)
* Merca (2012)
* Zoghbi-Stojmenovic (1998)


### References:

* **[Kelleher-2006]**
  Jerome Kelleher,
  "Generating partitions as ascending compositions",
  PhD thesis, University College Park, 2006,
  (http://jeromekelleher.net/downloads/k06.pdf)

* **[Kelleher-Sullivan-2009]**
  Jerome Kelleher and Barry O'Sullivan,
  "Generating all partitions: A comparison of two encodings",
  ArXiv:0909.2331, 2009,
  (https://arxiv.org/abs/0909.2331)

* **[Merca-2012]**
  Mircea Merca,
  "Fast algorithms for generating ascending compositions",
  J Math Model Algor (2012) 11:89--104, 2012,
  DOI:10.1007/s10852-011-9168-y

* **[Zoghbi-Stojmenovic-1998]**
  Antoine Zoghbi and  Ivan Stojmenovic,
  "Fast algorithms for generating integer partitions",
  Intern J Computer Math, Vol 70, pp 319--332, 1998,
  DOI:10.1080/00207169808804755
  (http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.42.1287&rep=rep1&type=pdf)


## q-Series and Other Utilities

C programs for q-series and other miscellaneous utilities are in
`qseries.h`, `qseries.c`, `util.h` and `util.c`.


## Examples

Examples of using these libraries in an executable C program can be
seen in `genpartn.c` and `partnid.c`.


## Compilation and Benchmark

You can simply run `make` at the root of this directory. (The
`Makefile` may need to be edited for the line with `CC = ... `
and change it to the appropriate compiler).

I have benchmarked the peformance of various partition generating
programs using different C compilers and different optimization
levels on an x86_64 machine running on Ubuntu 14.04.4 LTS. The
results are recorded in `benchmark.txt`.

Compilers: gcc-4.8.4, clang-3.6
Optimization levels: -O2, -O3
C Standard: -std=gnu11

The result is not very clear.  Without optimization, Merca3
is the fastest algorithm for generating ascending compositions.
But with `-O3` optimization turned on, it seems that Merca1 is
the fastest.  For generating descending composition, it seems that
ZS1 is the fastest.


## TODO

Try wrapping around the C program library as a python library.  The
pure python implementation (`partition.py`) is very slow. I may also
try other methods of optimizations.
