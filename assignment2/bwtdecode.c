/*
 * bwtdecode.c - reconstruct the original DNA text from an RLE-encoded BWT.
 *
 * Usage: ./bwtdecode <rbwt_file>
 *
 * Prints the original DNA sequence (including its terminating '\n') to stdout.
 * Reconstruction runs forward with the psi (LF-inverse) mapping so output can
 * be streamed a character at a time - the full text is never buffered, keeping
 * memory bounded even for large files.
 */
#include "bwt.h"
#include <stdlib.h>

static const char CODE2CH[5] = {'A', 'C', 'G', 'T', '\n'};

/* psi(i): row for the suffix one position later than row i's suffix. */
static long long psi(FMIndex *fm, long long i) {
    int c = fm_fchar(fm, i);
    if (c == 4) return fm->nlPos;          /* only the '\n' row (row 0) */
    long long rankInBucket = i - fm->C[c]; /* 0-based rank within F bucket */
    return fm_select(fm, c, rankInBucket + 1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <rbwt_file>\n", argv[0]);
        return 1;
    }

    FMIndex fm;
    if (fm_build(&fm, argv[1]) != 0) {
        fprintf(stderr, "Cannot read %s\n", argv[1]);
        return 1;
    }

    setvbuf(stdout, NULL, _IOFBF, 1 << 20);

    /* Row 0 is the '\n' suffix (text position n-1); psi(0) is text position 0. */
    long long p = psi(&fm, 0);
    for (long long k = 0; k < fm.n; k++) {
        putchar(CODE2CH[fm_fchar(&fm, p)]);
        p = psi(&fm, p);
    }

    fm_free(&fm);
    return 0;
}
