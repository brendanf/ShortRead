#include "ShortRead.h"
#include <stdlib.h>

/*
 * visit all sequences in a set, tallying character frequency as a
 * function of nucleotide position in the read.
 */
SEXP
alphabet_by_cycle(SEXP stringSet, SEXP width, SEXP alphabet)
{
    const int MAX_MAP = 256;
    /* FIXME: check types of incoming arguments */
    if (!IS_INTEGER(width) || LENGTH(width) != 1)
        Rf_error("'width' must be integer(1)");
    if (!IS_CHARACTER(alphabet))
        Rf_error("'alphabet' must be character");

    /* allocate and initialize the answer matrix */
    const int nrow = LENGTH(alphabet), ncol = INTEGER(width)[0];
    SEXP ans, dimnms, nms;
    PROTECT(ans = allocMatrix(INTSXP, nrow, ncol));
    PROTECT(dimnms = NEW_LIST(2));
    SET_VECTOR_ELT(dimnms, 0, alphabet);
    /* FIXME: Cycle dimnames? */
    PROTECT(nms = NEW_STRING(2));
    SET_STRING_ELT(nms, 0, mkChar("alphabet"));
    SET_STRING_ELT(nms, 1, mkChar("cycle"));
    SET_NAMES(dimnms, nms);
    SET_DIMNAMES(ans, dimnms);
    UNPROTECT(2);

    int *ansp = INTEGER(ans);   /* convenient pointer to data */
    memset(ansp, 0, LENGTH(ans) * sizeof(int)); /* initialize to 0 */

    /* set up a decoder for the string */
    const char *base = get_XStringSet_baseClass(stringSet);
    DECODE_FUNC decode = decoder(base);

    /* map between decoded character and offset into 'ans' */
    int i, j;
    int map[MAX_MAP];
    memset(map, -1, MAX_MAP*sizeof(int)); /* default; ignore */
    for (i = 0; i < LENGTH(alphabet); ++i) {
        unsigned char c = (unsigned char) *CHAR(STRING_ELT(alphabet, i));
        map[c] = i;
    }

    /* The main loop. Cache the string set for fast access, then
     * iterate over all strings, and over all characters in the
     * string. For each character, decode and map into the answer
     * matrix.
     *
     * FIXME:
     */
    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    const int len = get_XStringSet_length(stringSet);
    for (i = 0; i < len; ++i) {
        RoSeq seq = get_CachedXStringSet_elt_asRoSeq(&cache, i);
        for (j = 0; j < seq.nelt; ++j) {
            int idx = map[decode(seq.elts[j])];
            if (idx >= 0)
                ansp[j * nrow + idx] += 1;
        }
    }

    UNPROTECT(1);
    return ans;
}

SEXP
alphabet_pair_by_cycle(SEXP stringSet1, SEXP stringSet2, SEXP width, SEXP alphabet1, SEXP alphabet2)
{
    const int MAX_MAP = 256;
    /* FIXME: check types of incoming arguments */
    if (get_XStringSet_length(stringSet1) != get_XStringSet_length(stringSet2))
    	Rf_error("'stringSet1' and 'stringSet2' must have the same length");
    if (!IS_INTEGER(width) || LENGTH(width) != 1)
        Rf_error("'width' must be integer(1)");
    if (!IS_CHARACTER(alphabet1) || !IS_CHARACTER(alphabet2))
        Rf_error("'alphabet' must be list of character vectors");

    /* allocate and initialize the answer matrix */
    const int dim1 = LENGTH(alphabet1), dim2 = LENGTH(alphabet2), dim3 = INTEGER(width)[0];
    const int dim1xdim2 = dim1 * dim2;
    SEXP ans, dimnms, nms;
    PROTECT(ans = alloc3DArray(INTSXP, dim1, dim2, dim3));
    PROTECT(dimnms = NEW_LIST(3));
    SET_VECTOR_ELT(dimnms, 0, alphabet1);
    SET_VECTOR_ELT(dimnms, 1, alphabet2);
    /* FIXME: Cycle dimnames? */
    PROTECT(nms = NEW_STRING(3));
    SET_STRING_ELT(nms, 0, mkChar("base"));
    SET_STRING_ELT(nms, 1, mkChar("quality"));
    SET_STRING_ELT(nms, 3, mkChar("cycle"));
    SET_NAMES(dimnms, nms);
    SET_DIMNAMES(ans, dimnms);
    UNPROTECT(2);

    int *ansp = INTEGER(ans);   /* convenient pointer to data */
    memset(ansp, 0, LENGTH(ans) * sizeof(int)); /* initialize to 0 */

    /* set up a decoder for string1 and string2 */
    const char *base1 = get_XStringSet_baseClass(stringSet1);
    const char *base2 = get_XStringSet_baseClass(stringSet2);
    DECODE_FUNC decode1 = decoder(base1);
    DECODE_FUNC decode2 = decoder(base2);

    /* map between decoded character and offset into 'ans' */
    int i, j;
    int map1[MAX_MAP], map2[MAX_MAP];
    memset(map1, -1, MAX_MAP*sizeof(int)); /* default; ignore */
    memset(map2, -1, MAX_MAP*sizeof(int)); /* default; ignore */
    for (i = 0; i < LENGTH(alphabet1); ++i) {
        unsigned char c = (unsigned char) *CHAR(STRING_ELT(alphabet1, i));
        map1[c] = i;
    }
    for (i = 0; i < LENGTH(alphabet2); ++i) {
        unsigned char c = (unsigned char) *CHAR(STRING_ELT(alphabet2, i));
        map2[c] = i;
    }

    /* The main loop. Cache the string set for fast access, then
     * iterate over all strings, and over all characters in the
     * string. For each character, decode and map into the answer
     * matrix.
     *
     * FIXME:
     */
    CachedXStringSet cache1 = new_CachedXStringSet(stringSet1);
    CachedXStringSet cache2 = new_CachedXStringSet(stringSet2);
    const int len = get_XStringSet_length(stringSet1);
    for (i = 0; i < len; ++i) {
        RoSeq seq1 = get_CachedXStringSet_elt_asRoSeq(&cache1, i);
        RoSeq seq2 = get_CachedXStringSet_elt_asRoSeq(&cache2, i);
        for (j = 0; j < seq1.nelt; ++j) {
            int idx1 = map1[decode1(seq1.elts[j])];
            int idx2 = map2[decode2(seq2.elts[j])];
            if (idx1 >= 0 && idx2 >= 0)
                ansp[j * dim1xdim2 + idx2 * dim1 + idx1] += 1;
        }
    }

    UNPROTECT(1);
    return ans;
}

SEXP
alphabet_score(SEXP stringSet, SEXP score)
{
    /* FIXME: stringSet is XStringSet */
    const char *base = get_XStringSet_baseClass(stringSet);
    if (strcmp(base, "BString") != 0)
        Rf_error("'stringSet' must contain BString elements");
    if (!IS_NUMERIC(score) || LENGTH(score) != 256)
        Rf_error("'%s' must be '%s'", "score", "integer(256)");

    DECODE_FUNC decode = decoder(base);
    const int len = get_XStringSet_length(stringSet);
    int i, j;
    const double *dscore = REAL(score);

    SEXP ans;
    PROTECT(ans = NEW_NUMERIC(len));
    double *dans = REAL(ans);

    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    for (i = 0; i < len; ++i) {
        RoSeq seq = get_CachedXStringSet_elt_asRoSeq(&cache, i);
        dans[i] = 0;
        for (j = 0; j < seq.nelt; ++j)
            dans[i] +=  dscore[decode(seq.elts[j])];
    }

    UNPROTECT(1);
    return ans;
}

SEXP
alphabet_as_int(SEXP stringSet, SEXP score)
{
    /* FIXME: stringSet is XStrinSet(1) or longer? */
    const char *base = get_XStringSet_baseClass(stringSet);
    if (strcmp(base, "BString") != 0)
        Rf_error("'stringSet' must contain BString elements");
    if (!IS_INTEGER(score) || LENGTH(score) != 256)
        Rf_error("'%s' must be '%s'", "score", "integer(256)");
    DECODE_FUNC decode = decoder(base);
    const int len = get_XStringSet_length(stringSet);

    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    int i;

    RoSeq seq = get_CachedXStringSet_elt_asRoSeq(&cache, 0);
    int width = seq.nelt;
    int *ians;
    SEXP ans;
    for (i = 1; i < len && width > 0; ++i) {
        seq = get_CachedXStringSet_elt_asRoSeq(&cache, i);
        if (seq.nelt != width) width = -1;
    }
    if (width >= 0) {           /* matrix */
        ans = PROTECT(allocMatrix(INTSXP, len, width));
        ians = INTEGER(ans);
    } else {                    /* list of int */
        ans = PROTECT(NEW_LIST(len));
    }

    const int *iscore = INTEGER(score);
    int j;
    for (i = 0; i < len; ++i) {
        seq = get_CachedXStringSet_elt_asRoSeq(&cache, i);
        if (width >= 0) { /* int matrix */
            for (j = 0; j < seq.nelt; ++j)
                ians[len*j + i] =  iscore[decode(seq.elts[j])];
        } else {                /* list of ints */
            SET_VECTOR_ELT(ans, i, NEW_INTEGER(seq.nelt));
            ians = INTEGER(VECTOR_ELT(ans, i));
            for (j = 0; j < seq.nelt; ++j)
                ians[j] =  iscore[decode(seq.elts[j])];
        }
    }

    UNPROTECT(1);
    return ans;
}

/* rank / order / sort / duplicated */

typedef struct {
    int offset;
    RoSeq ref;
} XSort;

int
compare_RoSeq(const void *a, const void *b)
{
    const RoSeq ra = ((const XSort*) a)->ref;
    const RoSeq rb = ((const XSort*) b)->ref;

    const int diff = ra.nelt - rb.nelt;
    size_t len = diff < 0 ? ra.nelt : rb.nelt;
    int res = memcmp(ra.elts, rb.elts, len);
    return res == 0 ? diff : res;
}

void
_alphabet_order(CachedXStringSet cache, XSort *xptr, const int len)
{
    int i;

    for (i = 0; i < len; ++i) {
        xptr[i].offset=i;
        xptr[i].ref = get_CachedXStringSet_elt_asRoSeq(&cache, i);
    }
    qsort(xptr, len, sizeof(XSort), compare_RoSeq);
}

SEXP
alphabet_order(SEXP stringSet)
{
    /* FIXME: stringSet is XStringSet; non-zero len? */
    const int len = get_XStringSet_length(stringSet);
    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    XSort *xptr = (XSort*) R_alloc(len, sizeof(XSort));
    _alphabet_order(cache, xptr, len);

    SEXP ans;
    PROTECT(ans = NEW_INTEGER(len));
    int *ians = INTEGER(ans);
    int i;
    for (i = 0; i < len; ++i)
        ians[i] = xptr[i].offset + 1;
    UNPROTECT(1);
    return ans;
}

SEXP
alphabet_duplicated(SEXP stringSet)
{
    /* FIXME: stringSet is XStringSet; non-zero len? */
    const int len = get_XStringSet_length(stringSet);
    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    XSort *xptr = (XSort*) R_alloc(len, sizeof(XSort));
    _alphabet_order(cache, xptr, len);

    SEXP ans;
    PROTECT(ans = NEW_LOGICAL(len));
    int *ians = INTEGER(ans);
    ians[xptr[0].offset]=0;
    int i;
    for (i = 1; i < len; ++i)
        ians[xptr[i].offset] = compare_RoSeq(xptr+i-1, xptr+i) == 0;

    UNPROTECT(1);
    return ans;
}

SEXP
alphabet_rank(SEXP stringSet)
{
    /* integer vector of unique indices into sorted set */
    const int len = get_XStringSet_length(stringSet);
    CachedXStringSet cache = new_CachedXStringSet(stringSet);
    XSort *xptr = (XSort*) R_alloc(len, sizeof(XSort));
    _alphabet_order(cache, xptr, len);

    SEXP rank = PROTECT(NEW_INTEGER(len));
    int *irank = INTEGER(rank), i;
    irank[xptr[0].offset] = 1;
    for (i = 1; i < len; ++i) {
        if (compare_RoSeq(&xptr[i-1], &xptr[i]) == 0) {
            irank[xptr[i].offset] = irank[xptr[i-1].offset];
        } else {
            irank[xptr[i].offset] = i + 1;
        }
    }

    UNPROTECT(1);
    return rank;
}

SEXP
aligned_read_rank(SEXP alignedRead, SEXP order, SEXP rho)
{
    SEXP chr, str, pos, sread;
    PROTECT(chr = _get_SEXP(alignedRead, rho, "chromosome"));
    PROTECT(str = _get_SEXP(alignedRead, rho, "strand"));
    PROTECT(pos = _get_SEXP(alignedRead, rho, "position"));
    PROTECT(sread = _get_SEXP(alignedRead, rho, "sread"));
    int *c = INTEGER(chr), *s = INTEGER(str), *p = INTEGER(pos),
        *o = INTEGER(order), len = LENGTH(order);
    CachedXStringSet cache = new_CachedXStringSet(sread);
    XSort *xptr = (XSort*) R_alloc(2, sizeof(XSort));
    SEXP rank;
    PROTECT(rank = NEW_INTEGER(len));
    int *r = INTEGER(rank), i;
    xptr[0].ref = get_CachedXStringSet_elt_asRoSeq(&cache, 0);
    r[o[0]-1] = 1;
    for (i = 1; i < len; ++i) {
        const int this = o[i]-1, prev=o[i-1]-1;
        xptr[i%2].ref = get_CachedXStringSet_elt_asRoSeq(&cache, this);
        if (c[this] != c[prev] || s[this] != s[prev] ||
            p[this] != p[prev] || compare_RoSeq(xptr, xptr+1) != 0)
            r[this] = i + 1;
        else
            r[this] = r[prev];
    }
    UNPROTECT(5);
    return rank;
}
