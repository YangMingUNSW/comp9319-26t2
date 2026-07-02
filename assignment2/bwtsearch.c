/*
 * bwtsearch.c - BWT backward search over an RLE-encoded BWT DNA file.
 *
 * Usage: ./bwtsearch <rbwt_file> <search_term>
 *
 * Prints, for every occurrence of <search_term> (including overlapping ones),
 * a line of  up-to-2 preceding chars + the match + up-to-2 following chars.
 * Context stops at the single '\n' that terminates the DNA sequence (i.e. it
 * never wraps around and never prints the '\n').
 *
 * Method:
 *   - Backward search gives the row interval [sp, ep]; the count is ep-sp+1.
 *   - Preceding context per match: LF-mapping (a couple of steps each).
 *   - Following context: extended backward searches for P.c1 and P.c1.c2
 *     partition [sp, ep] into sub-intervals that share the same following
 *     chars, so the whole query costs O(pattern_len) rather than O(matches
 *     * pattern_len) forward walks.
 */
#include "bwt.h"
#include <stdlib.h>
#include <string.h>

static const char CODE2CH[5] = {'A', 'C', 'G', 'T', '\n'};

static int ch2code(char ch) {
    switch (ch) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:  return -1;
    }
}

/* Backward search for codes[0..len-1]. Returns 1 and sets [*sp,*ep] if found. */
static int bwsearch(FMIndex *fm, const int *codes, int len,
                    long long *sp, long long *ep) {
    int c = codes[len - 1];
    long long lo = fm->C[c];
    long long hi = fm->C[c] + fm->count[c] - 1;
    for (int i = len - 2; i >= 0; i--) {
        c = codes[i];
        lo = fm->C[c] + fm_rank(fm, c, lo);
        hi = fm->C[c] + fm_rank(fm, c, hi + 1) - 1;
        if (lo > hi) return 0;
    }
    *sp = lo;
    *ep = hi;
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <rbwt_file> <search_term>\n", argv[0]);
        return 1;
    }

    FMIndex fm;
    if (fm_build(&fm, argv[1]) != 0) {
        fprintf(stderr, "Cannot read %s\n", argv[1]);
        return 1;
    }

    const char *term = argv[2];
    int m = (int)strlen(term);
    int *codes = malloc(sizeof(int) * (m + 2));   /* room for 2 forward chars */
    for (int i = 0; i < m; i++) {
        codes[i] = ch2code(term[i]);
        if (codes[i] < 0) { free(codes); fm_free(&fm); return 0; }
    }

    long long sp, ep;
    if (!bwsearch(&fm, codes, m, &sp, &ep)) {
        free(codes); fm_free(&fm);
        return 0;                              /* no matches */
    }
    long long cnt = ep - sp + 1;

    /* Following context: 5 == "none" (sequence ends here). */
    char *f1 = malloc((size_t)cnt);
    char *f2 = malloc((size_t)cnt);
    memset(f1, 5, (size_t)cnt);
    memset(f2, 5, (size_t)cnt);

    for (int c1 = 0; c1 < 4; c1++) {
        codes[m] = c1;
        long long a1, b1;
        if (!bwsearch(&fm, codes, m + 1, &a1, &b1)) continue;
        for (long long r = a1; r <= b1; r++) f1[r - sp] = (char)c1;
        for (int c2 = 0; c2 < 4; c2++) {
            codes[m + 1] = c2;
            long long a2, b2;
            if (!bwsearch(&fm, codes, m + 2, &a2, &b2)) continue;
            for (long long r = a2; r <= b2; r++) f2[r - sp] = (char)c2;
        }
    }

    setvbuf(stdout, NULL, _IOFBF, 1 << 20);
    char line[160];
    for (long long r = sp; r <= ep; r++) {
        int len = 0;

        /* Preceding context via LF-mapping (text[t-2] then text[t-1]). */
        int p1 = fm_access(&fm, r);            /* text[t-1] */
        if (p1 != 4) {
            long long r2 = fm.C[p1] + fm_rank(&fm, p1, r + 1) - 1;   /* LF(r) */
            int p2 = fm_access(&fm, r2);        /* text[t-2] */
            if (p2 != 4) line[len++] = CODE2CH[p2];
            line[len++] = CODE2CH[p1];
        }

        /* The matched term. */
        for (int i = 0; i < m; i++) line[len++] = CODE2CH[codes[i]];

        /* Following context. */
        int a = f1[r - sp];
        if (a != 5) {
            line[len++] = CODE2CH[a];
            int b = f2[r - sp];
            if (b != 5) line[len++] = CODE2CH[b];
        }

        line[len++] = '\n';
        fwrite(line, 1, (size_t)len, stdout);
    }

    free(f1); free(f2); free(codes);
    fm_free(&fm);
    return 0;
}
