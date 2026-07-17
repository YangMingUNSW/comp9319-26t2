/*
 * bwt.c - FM-index construction and query primitives over an RLE-encoded BWT.
 *
 * See bwt.h for the overall approach.  Each RLE byte encodes one run fragment:
 *   code = byte >> 5;              (A=0 C=1 G=2 T=3 \n=4)
 *   run  = (byte & 0x1F) + 1;      (1..32; longer runs span several bytes)
 * Every byte is therefore a complete run boundary, so decoding may start at any
 * byte offset - which is what lets the sampled checkpoints work.
 */
#include "bwt.h"
#include <stdlib.h>
#include <string.h>

/* Aim for ~50k checkpoints (~2MB) regardless of file size; denser for small
 * files (cheaper scans) but never sparser than one per MIN_S bytes. */
#define CP_TARGET 50000LL
#define MIN_S 32LL
#define BUILD_CHUNK (1 << 20)

/* Read up to S bytes at file offset off into fm->rbuf; returns bytes read. */
static long long read_span(FMIndex *fm, long long off) {
    long long len = fm->S;
    if (off + len > fm->fileSize) len = fm->fileSize - off;
    if (len <= 0) return 0;
    fseek(fm->fp, (long)off, SEEK_SET);
    return (long long)fread(fm->rbuf, 1, (size_t)len, fm->fp);
}

/* Largest checkpoint index j with cps[j].pos <= i. */
static long long cp_for_pos(FMIndex *fm, long long i) {
    long long lo = 0, hi = fm->numCP - 1, j = 0;
    while (lo <= hi) {
        long long mid = (lo + hi) / 2;
        if (fm->cps[mid].pos <= i) { j = mid; lo = mid + 1; }
        else hi = mid - 1;
    }
    return j;
}

int fm_build(FMIndex *fm, const char *path) {
    memset(fm, 0, sizeof(*fm));
    fm->fp = fopen(path, "rb");
    if (!fm->fp) return 1;

    fseek(fm->fp, 0, SEEK_END);
    fm->fileSize = ftell(fm->fp);
    fseek(fm->fp, 0, SEEK_SET);

    long long S = fm->fileSize / CP_TARGET;
    if (S < MIN_S) S = MIN_S;
    fm->S = S;

    long long maxCP = fm->fileSize / S + 2;
    fm->cps  = malloc(sizeof(Checkpoint) * maxCP);
    fm->rbuf = malloc((size_t)S + 8);
    unsigned char *buf = malloc(BUILD_CHUNK);
    if (!fm->cps || !fm->rbuf || !buf) { free(buf); return 1; }

    long long pos = 0;                 /* decoded positions seen so far */
    long long cnt[4] = {0, 0, 0, 0};
    long long total[5] = {0, 0, 0, 0, 0};
    long long ncp = 0;

    long long off = 0;
    size_t got;
    while ((got = fread(buf, 1, BUILD_CHUNK, fm->fp)) > 0) {
        for (size_t k = 0; k < got; k++) {
            long long byteOff = off + (long long)k;
            if (byteOff % S == 0) {             /* record state before this byte */
                fm->cps[ncp].pos = pos;
                fm->cps[ncp].c[0] = cnt[0];
                fm->cps[ncp].c[1] = cnt[1];
                fm->cps[ncp].c[2] = cnt[2];
                fm->cps[ncp].c[3] = cnt[3];
                ncp++;
            }
            unsigned char b = buf[k];
            int code = b >> 5;
            long long len = (b & 0x1F) + 1;
            total[code] += len;
            if (code < 4) cnt[code] += len;
            pos += len;
        }
        off += (long long)got;
    }
    free(buf);

    fm->numCP = ncp;
    fm->n = pos;
    for (int i = 0; i < 5; i++) fm->count[i] = total[i];

    /* C[] by code, collation \n < A < C < G < T. */
    fm->C[4] = 0;                          /* \n */
    fm->C[0] = total[4];                   /* A */
    fm->C[1] = fm->C[0] + total[0];        /* C */
    fm->C[2] = fm->C[1] + total[1];        /* G */
    fm->C[3] = fm->C[2] + total[2];        /* T */
    return 0;
}

void fm_free(FMIndex *fm) {
    if (fm->fp) fclose(fm->fp);
    free(fm->cps);
    free(fm->rbuf);
    fm->fp = NULL; fm->cps = NULL; fm->rbuf = NULL;
}

long long fm_rank(FMIndex *fm, int c, long long i) {
    if (i <= 0) return 0;
    if (i > fm->n) i = fm->n;

    long long j = cp_for_pos(fm, i);
    long long pos = fm->cps[j].pos;
    long long cnt = fm->cps[j].c[c];
    long long got = read_span(fm, j * fm->S);

    for (long long k = 0; k < got && pos < i; k++) {
        unsigned char b = fm->rbuf[k];
        int code = b >> 5;
        long long len = (b & 0x1F) + 1;
        if (pos + len <= i) {
            if (code == c) cnt += len;
            pos += len;
        } else {
            if (code == c) cnt += (i - pos);
            pos = i;                       /* loop exits */
        }
    }
    return cnt;
}

int fm_access(FMIndex *fm, long long i) {
    long long j = cp_for_pos(fm, i);
    long long pos = fm->cps[j].pos;
    long long got = read_span(fm, j * fm->S);

    for (long long k = 0; k < got; k++) {
        unsigned char b = fm->rbuf[k];
        int code = b >> 5;
        long long len = (b & 0x1F) + 1;
        if (i < pos + len) return code;
        pos += len;
    }
    return -1;   /* unreachable for valid i */
}
