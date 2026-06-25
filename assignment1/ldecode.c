/*
 * COMP9319 2026T2 Assignment 1 -- LZW decoder (ldecode)
 *
 * Reverses lencode.  Because a phrase shorter than 3 characters is written
 * by the encoder as its literal ASCII bytes (not as an index), the usual
 * "one output token == one LZW code" decoder loses track of where phrase
 * boundaries are and its dictionary numbering drifts out of step with the
 * encoder.  To stay perfectly in sync we instead rebuild the original
 * character stream and feed every recovered character through the *encoder's*
 * automaton (feed_char): this reproduces the encoder's dictionary, with the
 * same indices and the same reset timing, char for char.
 *
 * The token stream only supplies characters:
 *   - a byte with the high bit clear is one literal ASCII character;
 *   - a byte with the high bit set is the high byte of a 15-bit index,
 *     idx = ((byte0 & 0x7F) << 8) | byte1, which expands to a stored phrase.
 *     If the index is the one the encoder is about to create (the classic
 *     LZW KwKwK case) the phrase is (current phrase) + (its first char).
 *
 * Build:  gcc -O2 -o ldecode ldecode.c
 * Usage:  ldecode <input_file> <output_file>
 */

#include <stdio.h>
#include <stdlib.h>

#define DICT_SIZE 32768
#define NODE_BASE 128
#define HASH_BITS 18
#define HASH_SIZE (1 << HASH_BITS)
#define HASH_MASK (HASH_SIZE - 1)

/* ---- dictionary entries (identical layout/logic to the encoder) -------- */
static int           ent_parent[DICT_SIZE];
static unsigned char ent_char[DICT_SIZE];
static unsigned char ent_first[DICT_SIZE];
static int           ent_len[DICT_SIZE];
static int           nextIndex;

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

static int node_len(int node)
{
    return node < NODE_BASE ? 1 : ent_len[node - NODE_BASE];
}
static int node_first(int node)
{
    return node < NODE_BASE ? node : ent_first[node - NODE_BASE];
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

/* ---- encoder automaton state (driven by the recovered characters) ------ */
static int  p;          /* current phrase node */
static int  started;    /* have we consumed the first character yet?        */
static int  resetPending;

/* Run one encoder step for character c, emitting c as reconstructed output
 * and updating the dictionary exactly as the encoder did. */
static void feed_char(int c)
{
    out_byte(c);
    if (!started) {
        p = c;
        started = 1;
        return;
    }
    int key = (p << 7) | c;
    int child = ht_lookup(key);
    if (child >= 0) {
        p = child + NODE_BASE;
    } else {
        if (resetPending) {
            dict_reset();
            resetPending = 0;
            p = c;
        } else {
            if (nextIndex < DICT_SIZE) {
                int e = nextIndex++;
                ent_parent[e] = p;
                ent_char[e]   = (unsigned char)c;
                ent_first[e]  = (unsigned char)node_first(p);
                ent_len[e]    = node_len(p) + 1;
                ht_insert(key, e);
                if (nextIndex == DICT_SIZE)
                    resetPending = 1;
            }
            p = c;
        }
    }
}

/* Write the characters of a node (single char or entry) into buf, forward
 * order, and return its length. */
static int node_string(int node, unsigned char *buf)
{
    int len = node_len(node);
    int pos = len;
    int cur = node;
    while (cur >= NODE_BASE) {
        buf[--pos] = ent_char[cur - NODE_BASE];
        cur = ent_parent[cur - NODE_BASE];
    }
    buf[--pos] = (unsigned char)cur;          /* single-char node */
    return len;
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

    /* a phrase can be at most DICT_SIZE+1 characters long; +2 for KwKwK */
    unsigned char *tmp = malloc(DICT_SIZE + 8);
    if (!tmp) {
        fprintf(stderr, "out of memory\n");
        fclose(fo);
        free(buf);
        free(tmp);
        return 1;
    }

    curEpoch = 1;
    nextIndex = 0;
    started = 0;
    resetPending = 0;

    for (size_t i = 0; i < n; ) {
        int b0 = buf[i++];
        if (b0 & 0x80) {                          /* two-byte dictionary index */
            if (i >= n) break;                    /* malformed; ignore trailing */
            int b1 = buf[i++];
            int idx = ((b0 & 0x7F) << 8) | b1;
            int len;
            if (idx < nextIndex) {                /* entry already exists       */
                len = node_string(idx + NODE_BASE, tmp);
            } else {                              /* KwKwK: idx == nextIndex    */
                len = node_string(p, tmp);
                tmp[len] = tmp[0];
                len++;
            }
            for (int k = 0; k < len; k++)
                feed_char(tmp[k]);
        } else {                                  /* literal ASCII character    */
            feed_char(b0);
        }
    }

    if (outlen)
        fwrite(out, 1, outlen, fo);
    fclose(fo);
    free(tmp);
    free(buf);
    free(out);
    return 0;
}
