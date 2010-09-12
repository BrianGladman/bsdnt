#ifndef BSDNT_NN_H
#define BSDNT_NN_H

#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#if ULONG_MAX == 4294967295U

typedef uint32_t word_t;
typedef unsigned int dword_t __attribute__((mode(DI)));
#define WORD_BITS 32

#else

typedef uint64_t word_t;
typedef unsigned int dword_t __attribute__((mode(TI)));
#define WORD_BITS 64

#endif

typedef word_t * nn_t;
typedef const word_t * nn_src_t;

typedef long len_t;
typedef long bits_t;

typedef void * rand_t;

/**********************************************************************
 
    Random generation

**********************************************************************/

/*
   Initialise a random state for use. 
*/
void randinit(rand_t state);

/*
   Clear a random state after use. 
*/
void randclear(rand_t state);

/*
   Generate a random word of data. 
*/
word_t randword(rand_t state);

/*
   Generate a random word in the range [0, m). 
*/
word_t randint(word_t m, rand_t state);

/*
   Set the first m limbs of a to random words. 
*/
void nn_random(nn_t a, rand_t state, len_t m);

/**********************************************************************
 
    Memory management

**********************************************************************/

/*
   Allocates an array of m words and returns it. Despite the
   name, the limbs are not initialised to zero.
*/
static inline
nn_t nn_init(len_t m)
{
   if (m) return malloc(m*sizeof(word_t));
   else return NULL;
}

/*
   Free the memory used by the nn_t a.
*/
static inline
void nn_clear(nn_t a)
{
   if (a) free(a);
}

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

/*
   Copy the m limbs at b to a.
*/
static inline
void nn_copy(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = b[i];
}

/*
   Set the m limbs at b to 0.
*/
static inline
void nn_zero(nn_t a, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = 0;
}

/*
   The pair {a, m} is said to be normalised if either m is
   zero, or a[m-1] is non-zero. In other words, {a, m} has
   no leading zero limbs. This function normalises {a, m}
   by returning the largest value m0 <= m for which {a, m0}
   is normalised.
*/
static inline
len_t nn_normalise(nn_t a, len_t m)
{
   while ((m != 0) && (a[m - 1] == 0))
      m--;

   return m;
}

/**********************************************************************
 
    Linear arithmetic functions

**********************************************************************/

/*
   Set a to the twos complement negation of b, where b is m words 
   in length. Return any borrow. The word ci is a carry-in. It is
   effectively subtracted from the result.
*/
word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci);

/*
   Set a to the twos complement negation of b, where b is m words
   in length. Return any borrow.
*/
#define nn_neg(a, b, m) \
    nn_neg_c(a, b, m, 0)

/*
   Set a to the twos complement negation of b, where b is m words
   in length. The word ci is a carry-in. It is effectively subtracted
   from the result. The borrow is written out to a[m]. 
*/
#define nn_s_neg_c(a, b, m, ci) \
   do { \
      (a)[m] = -nn_neg_c(a, b, m, ci); \
   } while (0)

/*
   Set a to the twos complement negation of b, where b is m words
   in length. The borrow is written out to a[m].
*/
#define nn_s_neg(a, b, m) \
   nn_s_neg_c(a, b, m, 0)

/*
   Set a = b + c where b is m words in length, and c is a word. 
   Return any carry out. 
*/
word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b + c where b is m words in length, and c is a word. 
   Write any carry out to a[m]. If a and b are aliased, the 
   carry out is added to a[m], otherwise it is written to a[m].
*/
#define nn_s_add1(a, b, m, c) \
   do { \
      if ((a) == (b)) \
         (a)[m] += nn_add1(a, b, m, c); \
      else \
         (a)[m] = nn_add1(a, b, m, c); \
   } while (0)

/*
   Set a = b - c where b is m words in length, and c is a word. 
   Return any borrow out. 
*/
word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t c);

/*
   Set a = b - c where b is m words in length, and c is a word. 
   Write any borrow out to a[m]. If a and b are aliased, the 
   borrow out is subtracted from a[m], otherwise it is written 
   to a[m].
*/
#define nn_s_sub1(a, b, m, c) \
   do { \
      if ((a) == (b)) \
         (a)[m] -= nn_sub1(a, b, m, c); \
      else \
         (a)[m] = -nn_sub1(a, b, m, c); \
   } while (0)

/*
   Set a = b + c + ci where b and c are both m words in length,
   ci is a "carry in". Return any carry out. 
*/
word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci);

/*
   Set a = b + c where b and c are both m words in length. Return 
   any carry out. 
*/
#define nn_add_m(a, b, c, m) \
   nn_add_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b + c + ci where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_s_add_mc(a, b, c, m, ci) \
   do { \
      if ((a) == (b)) \
         (a)[m] += nn_add_mc(a, b, c, m, ci); \
      else \
         (a)[m] = nn_add_mc(a, b, c, m, ci); \
   } while (0)

/*
   Set a = b + c where b and c are both m words in length, 
   writing the carry to a. If a and b are aliased, the carry is 
   added to a[m], otherwise a[m] is set to the carry.
*/
#define nn_s_add_m(a, b, c, m) \
   nn_s_add_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length,
   bi is a "borrow". Return any borrow. 
*/
word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t bi);

/*
   Set a = b - c where b and c are both m words in length. Return 
   any borrow. 
*/
#define nn_sub_m(a, b, c, m) \
   nn_sub_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b - c - bi where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the boorow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_s_sub_mc(a, b, c, m, bi) \
   do { \
      if ((a) == (b)) \
         (a)[m] -= nn_sub_mc(a, b, c, m, bi); \
      else \
         (a)[m] = -nn_sub_mc(a, b, c, m, bi); \
   } while (0)

/*
   Set a = b - c where b and c are both m words in length, 
   writing the borrow to a. If a and b are aliased, the borrow is 
   subtracted from a[m], otherwise a[m] is set to the borrow.
*/
#define nn_s_sub_m(a, b, c, m) \
   nn_s_sub_mc(a, b, c, m, (word_t) 0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "carry in". We return the carry out. 
*/
static inline
word_t nn_add_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ci = nn_add_mc(a, b, c, cm, ci);
   return nn_add1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   and bm >= cm. We return the carry out. 
*/
#define nn_add(a, b, bm, c, cm) \
   nn_add_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "carry in". We write the carry out to a[bm].
   If a and b are aliased the carry out is added to a[bm], otherwise
   it is written there.
*/
#define nn_s_add_c(a, b, bm, c, cm, ci) \
   do { \
      if ((a) == (b)) \
         (a)[bm] += nn_add_c(a, b, bm, c, cm, ci); \
      else \
         (a)[bm] = nn_add_c(a, b, bm, c, cm, ci); \
   } while (0)

/*
   Set a = b + c + ci where b is bm words, c is cm words in length,
   and bm >= cm. We write the carry out to a[bm]. If a and b are 
   aliased the carry out is added to a[bm], otherwise it is written 
   there.
*/
#define nn_s_add(a, b, bm, c, cm) \
   nn_s_add_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "borrow in". We return any borrow out. 
*/
static inline
word_t nn_sub_c(nn_t a, nn_src_t b, len_t bm, 
                            nn_src_t c, len_t cm, word_t ci)
{
   ci = nn_sub_mc(a, b, c, cm, ci);
   return nn_sub1(a + cm, b + cm, bm - cm, ci);
}

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   and bm >= cm. We return any borrow out. 
*/
#define nn_sub(a, b, bm, c, cm) \
   nn_sub_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   bm >= cm and ci is a "borrow in". We write the borrow out to a[bm].
   If a and b are aliased the borrow out is subtracted from a[bm], 
   otherwise it is written there.
*/
#define nn_s_sub_c(a, b, bm, c, cm, ci) \
   do { \
      if ((a) == (b)) \
         (a)[bm] -= nn_sub_c(a, b, bm, c, cm, ci); \
      else \
         (a)[bm] = -nn_sub_c(a, b, bm, c, cm, ci); \
   } while (0)

/*
   Set a = b - c - ci where b is bm words, c is cm words in length,
   and bm >= cm. We write the borrow out to a[bm]. If a and b are 
   aliased the borrow out is subtracted from a[bm], otherwise it is 
   written there.
*/
#define nn_s_sub(a, b, bm, c, cm) \
   nn_s_sub_c(a, b, bm, c, cm, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length,
   ci is a "carry in". Return any carry out. Assumes 0 <= bits
   < WORD_BITS.
*/
word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b << bits) where b is m words in length. Return 
   any carry out. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shl(a, b, m, bits) \
   nn_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b << bits) + ci where b is m words in length, ci is
   a "carry in", and writing the carry out to a[m]. Assumes 0 <= 
   bits < WORD_BITS.
*/
#define nn_s_shl_c(a, b, m, bits, ci) \
   do { \
      (a)[m] = nn_shl_c(a, b, m, bits, ci); \
   } while (0)

/*
   Set a = (b << bits) where b is m words in length, writing the 
   carry out to a[m]. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_s_shl(a, b, m, bits) \
   nn_s_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, ci is a "carry in". Return any carry out from the low
   end. Assumes 0 <= bits < WORD_BITS.
*/
word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci);

/*
   Set a = (b >> bits) where b is m words in length. Return 
   any carry out from the low end. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_shr(a, b, m, bits) \
   nn_shr_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, and ci is a[m]*2^(WORD_BITS - bits). Assumes 0 <= bits < 
   WORD_BITS.
*/
static inline
word_t nn_r_shr(nn_t a, nn_src_t b, len_t m, bits_t bits)
{
   if (bits)
      return nn_shr_c(a, b, m, bits, (b)[m] << (WORD_BITS - (bits))); \
   else
      return nn_shr_c(a, b, m, bits, (word_t) 0); \
}

/*
   Set a = b * c + ci where b is m words in length, c is a word and
   ci is a "carry in". Return any carry out. 
*/
word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = b * c where b is m words in length and c is a word. 
   Return any carry out.
*/
#define nn_mul1(a, b, m, c) \
   nn_mul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = b * c + ci where b is m words in length, c is a word and
   ci is a "carry in". Write any carry out to a[m]. 
*/
#define nn_s_mul1_c(a, b, m, c, ci) \
   do { \
      (a)[m] = nn_mul1_c(a, b, m, c, ci); \
   } while (0)

/*
   Set a = b * c where b is m words in length and c is a word.
   Write any carry out to a[m]. 
*/
#define nn_s_mul1(a, b, m, c) \
   nn_s_mul1_c(a, b, m, c, (word_t) 0)


/**********************************************************************
 
    Comparison

**********************************************************************/

/* 
   Return 1 if the m limbs at a match the n limbs at b,
   otherwise return 0.
*/
static inline
int nn_equal_m(nn_src_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      if (a[i] != b[i]) return 0;

   return 1;
}

/* 
   Return 1 if am == bm and the am limbs at a match the am 
   limbs at b, otherwise return 0.
*/
static inline
int nn_equal(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
      return 0;
   else 
      return nn_equal_m(a, b, am);
}

/* 
   Return a positive value if {a, m} > {b, m}, a negative
   value if {a, m} < {b, m}, otherwise returns 0.
*/
int nn_cmp_m(nn_src_t a, nn_src_t b, len_t m);

/* 
   Return a positive value if {a, am} > {b, bm}, a negative
   value if {a, am} < {b, bm}, otherwise returns 0.
*/
static inline
int nn_cmp(nn_src_t a, len_t am, nn_src_t b, len_t bm)
{
   if (am != bm)
   {
      if (am > bm)
         return 1;
      else
         return -1;
   } else
      return nn_cmp_m(a, b, am);
}

/**********************************************************************
 
    Logical operations

**********************************************************************/

/* 
   Set b to the bitwise logical not of a.
*/
static inline
void nn_not(nn_t a, nn_src_t b, len_t m)
{
   long i;

   for (i = 0; i < m; i++)
      a[i] = ~b[i];
}

#endif

