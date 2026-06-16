/*
 * COMP9319 2026T2 Assignment 1 -- LZW encoder (lencode)
 *
 * Implements a 15-bit (32,768-entry) LZW dictionary with reset-when-full.
 * Output format (bytes):
 *   - an ASCII character is written as one byte with the most-significant
 *     bit cleared (the input is 7-bit ASCII);
 *   - a dictionary index is written as two bytes, high byte first, with the
 *     most-significant bit of the HIGH byte set as a flag, leaving 15 index
 *     bits:  byte0 = 0x80 | (idx >> 8),  byte1 = idx & 0xFF.
 *
 * A matched phrase p is only written as an index when it is at least 3
 * characters long; shorter phrases (length 1 or 2) are written as their
 * literal ASCII bytes, because a 2-byte index would not be smaller.
 *
 * Individual ASCII characters do NOT occupy dictionary index space; the
 * 32,768 indices are used solely for multi-character phrases.  When the
 * dictionary fills up it is reset (after the current phrase has been
 * emitted) and rebuilt from scratch, exactly as at the start of the input.
 *
 * Build:  gcc -O2 -o lencode lencode.c
 * Usage:  lencode <input_file> <output_file>
 */

#include <stdio.h>
#include <stdlib.h>

#define DICT_SIZE 32768          /* number of 15-bit dictionary indices    */
#define NODE_BASE 128            /* ids 0..127 are single ASCII chars,      */
                                 /* ids 128.. are multi-character entries   */
#define HASH_BITS 18
#define HASH_SIZE (1 << HASH_BITS)
#define HASH_MASK (HASH_SIZE - 1)

/* ---- dictionary entries, indexed by entry id e (0..DICT_SIZE-1) ---------
 * The node id of entry e is e + NODE_BASE; its output index is e itself. */
static int           ent_parent[DICT_SIZE]; /* node id of the prefix (p)   */
static unsigned char ent_char[DICT_SIZE];   /* the last character (c)      */
static unsigned char ent_first[DICT_SIZE];  /* the first character         */
static int           ent_len[DICT_SIZE];    /* length in characters        */
static int           nextIndex;             /* next entry id to allocate   */

/* ---- phrase hash table:  key (parentNode<<7 | char)  ->  entry id -------
 * Reset in O(1) by bumping curEpoch; slots whose epoch differs are empty. */
static int htKey[HASH_SIZE];
static int htVal[HASH_SIZE];
static int htEpoch[HASH_SIZE];
static int curEpoch;

static unsigned hashKey(int key)
{
    unsigned x = (unsigned)key * 2654435761u;
    return (x >> (32 - HASH_BITS)) & HASH_MASK;
}

static int ht_lookup(int key)
{
    unsigned h = hashKey(key);
    while (htEpoch[h] == curEpoch) {
        if (htKey[h] == key)
            return htVal[h];
        h = (h + 1) & HASH_MASK;
    }
    return -1;
}

static void ht_insert(int key, int val)
{
    unsigned h = hashKey(key);
    while (htEpoch[h] == curEpoch && htKey[h] != key)
        h = (h + 1) & HASH_MASK;
    htKey[h] = key;
    htVal[h] = val;
    htEpoch[h] = curEpoch;
}

static void dict_reset(void)
{
    curEpoch++;
    nextIndex = 0;
}

/* ---- node helpers (a node is a single char id<128 or an entry) ---------- */
static int node_len(int node)
{
    return node < NODE_BASE ? 1 : ent_len[node - NODE_BASE];
}
static int node_first(int node)
{
    return node < NODE_BASE ? node : ent_first[node - NODE_BASE];
}
static int node_last(int node)
{
    return node < NODE_BASE ? node : ent_char[node - NODE_BASE];
}

/* ---- output buffer ------------------------------------------------------ */
static unsigned char *out;
static size_t outlen, outcap;

static void out_byte(int b)
{
    if (outlen == outcap) {
        outcap = outcap ? outcap * 2 : (1 << 20);
        out = realloc(out, outcap);
        if (!out) { fprintf(stderr, "out of memory\n"); exit(1); }
    }
    out[outlen++] = (unsigned char)b;
}

/* Emit phrase p: a 2-byte index if it holds >= 3 characters, else literal. */
static void emit(int node)
{
    int len = node_len(node);
    if (len >= 3) {
        int idx = node - NODE_BASE;
        out_byte(0x80 | (idx >> 8));
        out_byte(idx & 0xFF);
    } else if (len == 1) {
        out_byte(node_last(node));
    } else { /* len == 2 */
        out_byte(node_first(node));
        out_byte(node_last(node));
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s <input_file> <output_file>\n", argv[0]);
        return 1;
    }

    FILE *fi = fopen(argv[1], "rb");
    if (!fi) { perror(argv[1]); return 1; }
    fseek(fi, 0, SEEK_END);
    long sz = ftell(fi);
    fseek(fi, 0, SEEK_SET);
    if (sz < 0) { fprintf(stderr, "cannot size %s\n", argv[1]); fclose(fi); return 1; }
    unsigned char *buf = malloc(sz ? (size_t)sz : 1);
    if (!buf) { fprintf(stderr, "out of memory\n"); fclose(fi); return 1; }
    size_t n = fread(buf, 1, (size_t)sz, fi);
    fclose(fi);

    FILE *fo = fopen(argv[2], "wb");
    if (!fo) { perror(argv[2]); free(buf); return 1; }

    if (n > 0) {
        curEpoch = 1;
        nextIndex = 0;
        int pendingReset = 0;                  /* reset on next character step */
        int p = buf[0];                       /* current phrase, starts as 1 char */

        for (size_t i = 1; i < n; i++) {
            int c = buf[i];
            if (pendingReset) {
                dict_reset();
                pendingReset = 0;
            }
            int key = (p << 7) | c;
            int child = ht_lookup(key);
            if (child >= 0) {                 /* p+c is in the dictionary: extend */
                p = child + NODE_BASE;
            } else {                          /* mismatch: emit p, then add p+c   */
                emit(p);
                if (nextIndex == DICT_SIZE) {
                    /* full: reset only after output for processed chars emitted */
                    pendingReset = 1;
                    p = c;
                } else {
                    int e = nextIndex++;
                    ent_parent[e] = p;
                    ent_char[e]   = (unsigned char)c;
                    ent_first[e]  = (unsigned char)node_first(p);
                    ent_len[e]    = node_len(p) + 1;
                    ht_insert(key, e);
                    p = c;
                }
            }
        }
        emit(p);                              /* flush the final phrase           */
    }

    if (outlen)
        fwrite(out, 1, outlen, fo);
    fclose(fo);
    free(buf);
    free(out);
    return 0;
}
