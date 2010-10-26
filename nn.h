#ifndef BSDNT_NN_H
#define BSDNT_NN_H

#include <stdlib.h>
#include "helper.h"

/**********************************************************************
 
    Random generation

**********************************************************************/

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
 
    Printing functions

**********************************************************************/

/*
   Print the nn_t {a, m} in hexadecimal, with a space between each
   word. If m is zero, only 0 is printed.
*/
void nn_printx(nn_src_t a, len_t m);

/*
   Print the nn_t {a, m} in hexadecimal, with a space between each
   word, but only printing at worst, the first two words of a, an 
   ellipsis (if m > 4) and the final two words of a. If m is zero, 
   only 0 is printed.
*/
void nn_printx_short(nn_src_t a, len_t m);

/*
   Print information about which words of a and b differ and actually
   print the first and last words which differ, for both bignums. 
   This function is used for diagnostic purposes in test code. If 
   there is no difference, "don't differ" is printed.
*/
void nn_printx_diff(nn_src_t a, nn_src_t b, len_t m);

/**********************************************************************
 
    Basic manipulation

**********************************************************************/

/*
   Copy the m limbs at b to a. Aliasing is allowed, but is 
   not currently dealt with specially, i.e. the data is 
   actually copied.
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
   effectively subtracted from the result. The carry-in can be at
   most 1.
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
   from the result. The borrow is written out to a[m] and returned. 
   The carry-in can be at most 1.
*/
static inline
word_t nn_s_neg_c(nn_t a, nn_src_t b, len_t m, word_t ci)
{
   return (a[m] = -nn_neg_c(a, b, m, ci));
}

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
   ci is a "carry in". Return any carry out. The carry-in may be
   at most 1.
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
   added to a[m], otherwise a[m] is set to the carry. The carry-in 
   may be at most 1.
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
   bi is a "borrow". Return any borrow. The borrow-in may be at
   most 1.
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
   subtracted from a[m], otherwise a[m] is set to the borrow. The 
   borrow-in may be at most 1.
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
   bm >= cm and ci is a "carry in". We return the carry out. The carry-in 
   may be at most 1.
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
   it is written there. The carry-in may be at most 1.
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
   bm >= cm and ci is a "borrow in". We return any borrow out. The
   borrow-in may be at most 1.
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
   otherwise it is written there. The borrow-in may be at most 1.
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
   < WORD_BITS. Assumes ci is not more than the given number of 
   bits.
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
   a "carry in", and write the carry out to a[m] and return it. 
   Assumes 0 <= bits < WORD_BITS. Assumes ci is not more than the 
   given number of bits.
*/
static inline
word_t nn_s_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t ci)
{
   return (a[m] = nn_shl_c(a, b, m, bits, ci));
}

/*
   Set a = (b << bits) where b is m words in length, writing the 
   carry out to a[m]. Assumes 0 <= bits < WORD_BITS.
*/
#define nn_s_shl(a, b, m, bits) \
   nn_s_shl_c(a, b, m, bits, (word_t) 0)

/*
   Set a = (b >> bits) + ci*B^(m - 1) where b is m words 
   in length, ci is a "carry in". Return any carry out from the low
   end. Assumes 0 <= bits < WORD_BITS. Assumes the at most the top 
   "bits" bits of ci are nonzero.
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
   WORD_BITS. Assumes at most the top "bits" bits of ci are nonzero.
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
   ci is a "carry in". Write any carry out to a[m] and return it.
*/
static inline
word_t nn_s_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   return (a[m] = nn_mul1_c(a, b, m, c, ci)); 
}

/*
   Set a = b * c where b is m words in length and c is a word.
   Write any carry out to a[m]. 
*/
#define nn_s_mul1(a, b, m, c) \
   nn_s_mul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is returned. 
*/
word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a + b * c where a and b are m words in length, and c 
   is a word. The carry out is returned. 
*/
#define nn_addmul1(a, b, m, c) \
   nn_addmul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is added to
   a[m]. 
*/
#define nn_s_addmul1_c(a, b, m, c, ci) \
   do { \
      (a)[m] += nn_addmul1_c(a, b, m, c, ci); \
   } while (0)

/*
   Set a = a + b * c where a and b are m words in length and c 
   is a word. The carry out is added to a[m].  
*/
#define nn_s_addmul1(a, b, m, c) \
   nn_s_addmul1_c(a, b, m, c, (word_t) 0)

/*
   Set r = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is returned. 
*/
word_t nn_muladd1_c(nn_t r, nn_src_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set r = a + b * c where a and b are m words in length, and c 
   is a word. The carry out is returned. 
*/
#define nn_muladd1(r, a, b, m, c) \
   nn_muladd1_c(r, a, b, m, c, (word_t) 0)

/*
   Set r = a + b * c + ci where a and b are m words in length, c 
   is a word and ci is a "carry in". The carry out is added to
   a[m] if r aliases a, otherwise it is written to r[m]. 
*/
#define nn_s_muladd1_c(r, a, b, m, c, ci) \
   do { \
      if ((r) == (a)) \
         (a)[m] += nn_addmul1_c(a, b, m, c, ci); \
      else \
         (r)[m] = nn_muladd1_c(r, a, b, m, c, ci); \
   } while (0)

/*
   Set r = a + b * c where a and b are m words in length and c 
   is a word. The carry out is added to a[m] if r aliases a, 
   otherwise it is written to r[m].   
*/
#define nn_s_muladd1(r, a, b, m, c) \
   nn_s_muladd1_c(r, a, b, m, c, (word_t) 0)

/*
   Set a = a - b * c - ci where a and b are m words in length, c 
   is a word and ci is a "borrow in". The borrow out is returned. 
*/
word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci);

/*
   Set a = a - b * c where a and b are m words in length, and c 
   is a word. The borrow out is returned. 
*/
#define nn_submul1(a, b, m, c) \
   nn_submul1_c(a, b, m, c, (word_t) 0)

/*
   Set a = a - b * c - ci where a and b are m words in length, c 
   is a word and ci is a "borrow in". The borrow out is subtracted
   from a[m]. 
*/
#define nn_s_submul1_c(a, b, m, c, ci) \
   do { \
      (a)[m] -= nn_submul1_c(a, b, m, c, ci); \
   } while (0)

/*
   Set a = a - b * c where a and b are m words in length and c 
   is a word. The borrow out is subtracted from a[m].  
*/
#define nn_s_submul1(a, b, m, c) \
   nn_s_submul1_c(a, b, m, c, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must be
   reduced mod d. The quotient q requires m limbs of space.  An 
   exception will result if d is 0.
*/
word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci);

/*
   Set q = a / d and return the remainder, where a is m words in 
   length and d is a word. The quotient q requires m limbs of space. 
   An exception will result if d is 0.
*/
#define nn_divrem1_simple(q, a, m, d) \
   nn_divrem1_simple_c(q, a, m, d, (word_t) 0)

/*
   Set q = (ci*B^m + a) / d and return the remainder, where a is m 
   words in length, d is a word and ci is a "carry-in" which must be
   reduced mod d. The quotient q requires m limbs of space.  An 
   exception will result if d is 0. Requires that inv is a precomputed 
   inverse of d computed by the function precompute_inverse1. 
*/
word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t inv, word_t ci);

/* 
   As per _nn_divrem1_preinv_c but with no carry-in.
*/
#define nn_divrem1_preinv(q, a, m, d, inv) \
   nn_divrem1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Computes r and q such that r * B^m + a = q * d + ci, where
   q and a are m words in length, d is an odd word_t, inv is a
   precomputed Hensel inverse of d computed by the function
   precompute_hensel_inverse1 and ci is a "carry-in" word.
*/
word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci);

/*
   As per _nn_divrem_hensel1_preinv_c but with no "carry-in".
*/
#define nn_divrem_hensel1_preinv(q, a, m, d, inv) \
   nn_divrem_hensel1_preinv_c(q, a, m, d, inv, (word_t) 0)

/* 
   Return a + ci * B^m mod d given a precomputed mod_preinv1_t,
   where ci is a "carry-in" word and a is m words in length, 
   that ci is reduced mod d and that d is not zero.
*/
word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci);

/* 
   Return a mod d given a precomputed mod_preinv1_t, where a is m 
   words in length and that d is not zero.
*/
#define nn_mod1_preinv(a, m, d, inv) \
   nn_mod1_preinv_c(a, m, d, inv, (word_t) 0)

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

/**********************************************************************
 
    Quadratic arithmetic functions

**********************************************************************/

/*
   Set {r, m1 + m2 - 1} = {a, m1} * {b, m2} and return any carry-out. 
   The output r may not alias either of the inputs a or b. We require 
   m1 >= m2 > 0.
*/
word_t nn_mul_classical(nn_t r, nn_src_t a, len_t m1, nn_src_t b, len_t m2);

/*
   Set {r, m1 + m2 - 1} = {a, m1} * {b, m2} and write any carry-out. 
   The output r may not alias either of the inputs a or b. We require 
   m1 >= m2 > 0.
*/
#define nn_s_mul_classical(r, a, m1, b, m2) \
   do { \
      (r)[(m1) + (m2) - 1] = nn_mul_classical(r, a, m1, b, m2); \
   } while (0)

/*
   Set {r, m1 + m2 - 1} = {a, m1} + {b, m1} * {c, m2} and return any 
   carry-out. The output r may not alias either of the inputs b or c, 
   but a may alias with r as long as the requisite space is available. 
   We require m1 >= m2 > 0.
*/
word_t nn_muladd_classical(nn_t r, nn_src_t a, nn_src_t b, 
                                     len_t m1, nn_src_t c, len_t m2);

/*
   Set {r, m1 + m2 - 1} = {a, m1} + {b, m1} * {c, m2} and write any 
   carry-out. The output r may not alias either of the inputs b or c, 
   but a may alias with r as long as the requisite space is available. 
   We require m1 >= m2 > 0.
*/
#define nn_s_muladd_classical(r, a, b, m1, c, m2) \
   do { \
      (r)[(m1) + (m2) - 1] = nn_muladd_classical(r, a, b, m1, c, m2); \
   } while (0)

/*
   Set ov*B^m1 + {r, m1} to sum_{i + j < m1} a[i]*b[j]*B^{i + j}. In 
   other words, {r, m1} will be the low m1 words of the product 
   {a, m1}*{b, m2} and ov will contain any overflow from word by word 
   multiplications that don't fit in the m1 words of r and is 2 words
   in length. The output r may not alias either of the inputs a or b. 
   We require m1 >= m2 > 0.
*/
void nn_mullow_classical(nn_t ov, nn_t r, nn_src_t a, len_t m1, 
                                              nn_src_t b, len_t m2);

/*
   Set r to ov + sum_{i + j > m1} a[i]*b[j]*B^{i + j - m1}. In other
   words, this returns the high part of the multiplication {a, m1}*
   {b, m2}. The output requires m2 - 1 words and any carry will be 
   returned in a carry-out. We require m1 >= m2 > 0. If ov from mullow
   is passed to mulhigh then a mullow followed by a mulhigh is the same
   as a full mul. The output r may not alias either a or b.
*/
   word_t nn_mulhigh_classical(nn_t r, nn_src_t a, len_t m1, 
                                       nn_src_t b, len_t m2, nn_t ov);

/*
   As per nn_mullhigh_classical except the carry-out is written at
   word r[m2 - 1].
*/
#define nn_s_mulhigh_classical(r, a, m1, b, m2, ov) \
   do { \
      (r)[(m2) - 1] = nn_mulhigh_classical(r, a, m1, b, m2, ov); \
   } while (0)

/*
   Given a of length m and d of length n with a carry-in ci, compute
   the quotient of ci*B^m + a by d, and leave the remainder in the bottom 
   n limbs of a. Requires m >= n > 0. The precomputed inverse inv should 
   be computed from the leading two limbs of d (or the leading limb and 0 
   if n is 1) using precompute_inverse_lead. If a_n is the leading n limbs 
   of a, then ci*B^m + a_n must be less than B * d. The quotient
   must have space for m - n + 1 limbs. The quotient may not alias d.
*/
void nn_divrem_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                     len_t n, preinv1_2_t inv, word_t ci);

/*
   As per nn_divrem_classical_preinv_c except that the carry-in is read 
   from a[m]. The quotient will therefore be {a, m + 1} by {d, n}.
*/
#define nn_r_divrem_classical_preinv(q, a, m, d, n, inv) \
   do { \
      nn_divrem_classical_preinv_c(q, a, m, d, n, inv, (a)[m]); \
   } while (0)

/*
   As per nn_divrem_classical_preinv_c, however no remainder is computed
   and the quotient is either correct or one too large, i.e. |a - q*d| < d.
   The dividend a is destroyed and may not alias q.
*/
void nn_divapprox_classical_preinv_c(nn_t q, nn_t a, len_t m, nn_src_t d, 
                                  len_t n, preinv1_2_t inv, word_t ci);

/*
   As per nn_divapprox_classical_preinv_c except that the carry-in is read 
   from a[m]. The approx. quotient will therefore be {a, m + 1} by {d, n}.
*/
#define nn_r_divapprox_classical_preinv(q, a, m, d, n, inv) \
   do { \
      nn_divapprox_classical_preinv_c(q, a, m, d, n, inv, (a)[m]); \
   } while (0)

/*
   Perform Hensel division of {a, m} by {d, n} with the quotient mod B^m
   being returned in q and any overflow from mullow(q, d) being returned
   in ov. We require m >= n > 0. The quotient q requires m words of space
   and ov requires 2 words. The dividend, a, is destroyed and may not alias
   q. We also require that q does not alias d. The divisor d must be odd 
   and inv must be a precomputed inverse of d[0] computed with
   precompute_hensel_inverse1.
*/
void nn_div_hensel_preinv(nn_t ov, nn_t q, nn_t a, len_t m, 
                            nn_src_t d, len_t n, hensel_preinv1_t inv);

#endif

