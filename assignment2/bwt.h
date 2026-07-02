/*
 * bwt.h - FM-index over a run-length-encoded BWT (.rbwt) DNA file.
 *
 * COMP9319 2026T2 Assignment 2. Shared by bwtsearch and bwtdecode.
 *
 * The index never holds the decoded BWT (which can be far larger than the
 * <=110MB .rbwt).  It keeps only sampled "checkpoints" of cumulative character
 * counts (~2MB) and reads short RLE spans from the file on demand.  Because the
 * file bytes are read via stdio (not mapped into our address space) they are
 * cached by the OS and do NOT count against the 16MB budget measured by
 * `valgrind --tool=massif --pages-as-heap=yes`.
 *
 * Character codes match the 3 MSBs of each RLE byte:
 *   A=0  C=1  G=2  T=3  \n=4
 * Collation order for the FM C[] table is by ASCII: \n < A < C < G < T.
 */
#ifndef BWT_H
#define BWT_H

#include <stdio.h>

/* Cumulative state sampled every S bytes of the .rbwt file. */
typedef struct {
    long long pos;    /* decoded BWT positions in file bytes [0, j*S)      */
    long long c[4];   /* cumulative counts of A,C,G,T in those positions   */
} Checkpoint;

typedef struct {
    FILE *fp;              /* open .rbwt file (read on demand)             */
    long long fileSize;    /* size of the .rbwt file in bytes              */
    long long S;           /* checkpoint sampling interval (bytes)         */
    long long numCP;       /* number of checkpoints                        */
    Checkpoint *cps;       /* checkpoint array (~2MB)                      */
    long long n;           /* total decoded BWT length                     */
    long long count[5];    /* total counts by code A,C,G,T,\n              */
    long long C[5];        /* FM-index C[] by code                         */
    long long nlPos;       /* BWT position of the single '\n'              */
    unsigned char *rbuf;   /* reusable read buffer of size S (+slack)      */
} FMIndex;

/* Build the index by a single sequential pass over the file.
 * Returns 0 on success, non-zero on error. */
int  fm_build(FMIndex *fm, const char *path);
void fm_free(FMIndex *fm);

/* Number of occurrences of code c (0..3) in L[0..i-1] (the first i chars). */
long long fm_rank(FMIndex *fm, int c, long long i);

/* Code (0..4) of the BWT character at position i. */
int fm_access(FMIndex *fm, long long i);

/* Position of the k-th (1-based) occurrence of code c (0..3) in the BWT. */
long long fm_select(FMIndex *fm, int c, long long k);

/* Code of the F-column (first column) character at row i. */
int fm_fchar(FMIndex *fm, long long i);

#endif /* BWT_H */
