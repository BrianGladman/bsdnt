/* 
  Copyright (C) 2010, 2013 William Hart

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, 
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
	 documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "nn.h"
#include "zz0.h"
#include "zz.h"
#include "test.h"

rand_t state;

#undef ITER
#define ITER 50000

int test_gc(void)
{
   int result = 1;
   len_t m;
   word_t w1, w2, w3, w4;
   zz_t a, b, c;

   printf("gc...");

   TEST_START(1, ITER) 
   {
      randoms_upto(100, NONZERO, state, &m, NULL);
      randoms_upto(m, ANY, state, &w1, &w2, &w3, &w4, NULL);
      
      randoms_signed(w1, ANY, state, &a, &b, &c, NULL);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_seti_equali(void)
{
   int result = 1;
   zz_t r1;
   sword_t c1, c2;

   printf("zz_seti/equali...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);

      zz_seti(r1, c1);
      
      result = zz_equali(r1, c1);

      if (!result) 
      {
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a != a + b if b != 0 */
   TEST_START(2, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      do {
         c2 = (sword_t) randword(state);
      } while (c2 == 0);

      zz_seti(r1, c1 + c2);
      
      result = !zz_equali(r1, c1);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_set_equal(void)
{
   int result = 1;
   zz_t r1, a, b;
   len_t m1, m2;

   printf("zz_set/equal...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_set(r1, a);
      
      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a != a + b if b != 0 */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);
      randoms_upto(10, NONZERO, state, &m2, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, NONZERO, state, &b, NULL);
     
      zz_add(r1, a, b);
      
      result = !zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_neg(void)
{
   int result = 1;
   zz_t r1, a;
   len_t m1;

   printf("zz_neg...");

   /* test a = -(-a) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_neg(r1, a);
      zz_neg(r1, r1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_swap(void)
{
   int result = 1;
   zz_t r1, r2, a, b;
   len_t m1, m2;

   printf("zz_swap...");

   /* test swap */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_set(r1, a);
      zz_set(r2, b);
      zz_swap(a, b);

      result = (zz_equal(r1, b) && zz_equal(r2, a));

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_is_zero(void)
{
   int result = 1;
   zz_t r1;
   
   printf("zz_zero/is_zero...");

   /* test 0 = 0 */
   TEST_START(1, ITER) 
   {
      randoms_signed(10, ANY, state, &r1, NULL);
      
      zz_zero(r1);

      result = zz_is_zero(r1);

      if (!result) 
      {
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmpi(void)
{
   int result = 1;
   zz_t a, r1;
   sword_t c1;
   len_t m1;

   printf("zz_cmpi...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);

      zz_seti(r1, c1);
      
      result = (zz_cmpi(r1, c1) == 0);

      if (!result) 
      {
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a > b if a is positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_seti(r1, c1);
      zz_add(r1, r1, a);

      result = (zz_cmpi(r1, c1) > 0);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1);
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a < b if a is negative */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_seti(r1, c1);
      zz_neg(a, a);
      zz_add(r1, r1, a);

      result = (zz_cmpi(r1, c1) < 0);

      if (!result) 
      {
         bsdnt_printf("%w, %w\n", c1);
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmp(void)
{
   int result = 1;
   zz_t a, r1, r2;
   len_t m1, m2;

   printf("zz_cmp...");

   /* test a = a */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      
      zz_set(r1, a);
      
      result = (zz_cmp(r1, a) == 0);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a > b if a is positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, ANY, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);

      result = (zz_cmp(r2, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test b + a < b if a is negative */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, ANY, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_neg(a, a);
      zz_add(r2, r1, a);

      result = (zz_cmp(r2, r1) < 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_cmpabs(void)
{
   int result = 1;
   zz_t a, r1, r2;
   len_t m1, m2;

   printf("zz_cmpabs...");

   /* test |a| = |+/- a| */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(0, ANY, state, &r1, NULL);
      randoms_signed(m1, ANY, state, &a, NULL);
      
      zz_set(r1, a);
      if (randint(2, state) == 0)
         zz_neg(a, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);

      result = (zz_cmpabs(r1, a) == 0);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- (b + a)| > |+/- b| if a and b are positive */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, POSITIVE, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);
      if (randint(2, state) == 0)
         zz_neg(r2, r2);

      result = (zz_cmpabs(r2, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- b| < |+/- (b + a)| if a and b are positive */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, &m2, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(m2, POSITIVE, state, &r1, NULL);
      randoms_signed(0, ANY, state, &r2, NULL);
      
      zz_add(r2, r1, a);
      if (randint(2, state) == 0)
         zz_neg(r1, r1);
      if (randint(2, state) == 0)
         zz_neg(r2, r2);

      result = (zz_cmpabs(r1, r2) < 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test |+/- a| > 0 */
   TEST_START(4, ITER) 
   {
      randoms_upto(10, NONZERO, state, &m1, NULL);

      randoms_signed(m1, POSITIVE, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      if (randint(2, state) == 0)
         zz_neg(a, a);

      result = (zz_cmpabs(a, r1) > 0);

      if (!result) 
      {
         zz_print_debug(a); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_addi(void)
{
   int result = 1;
   zz_t a, b, r1, r2;
   len_t m1, m2;
   sword_t c1, c2;
   
   printf("zz_addi...");

   /* test (a + b) + c = a + (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_addi(r1, a, c1);
      zz_addi(r1, r1, c2);

      zz_seti(r2, c1);
      zz_addi(r2, r2, c2);
      zz_add(r2, r2, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_addi, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_subi(void)
{
   int result = 1;
   zz_t a, b, r1, r2;
   len_t m1, m2;
   sword_t c1, c2;
   
   printf("zz_subi...");

   /* test (a - b) - c = a - (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_subi(r1, a, c1);
      zz_subi(r1, r1, c2);

      zz_seti(r2, c1);
      zz_addi(r2, r2, c2);
      zz_sub(r2, a, r2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w, %w\n", c1, c2);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      c1 = (sword_t) randword(state);
      
      zz_addi(r1, a, c1);
      zz_subi(r1, r1, c1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      c1 = (sword_t) randword(state);
      
      zz_subi(r1, a, c1);
      zz_addi(r2, a, -c1);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a);
         bsdnt_printf("%w\n", c1);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_subi, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_muli(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t1, t2;
   len_t m1, m2;
   sword_t c1, c2;

   printf("zz_muli...");

   /* test a(b + c) = ab + ac */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &t1, &t2, NULL);
      c1 = (sword_t) randword(state);
      c2 = (sword_t) randword(state);

      zz_seti(t1, c1);
      zz_addi(t1, t1, c2);
      zz_mul(r1, a, t1);

      zz_muli(t1, a, c1);
      zz_muli(t2, a, c2);
      zz_add(r2, t1, t2);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      c1 = (sword_t) randword(state);
      
      test_zz_aliasing_12i(zz_muli, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_mul_2exp(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t;
   len_t m1, m2;
   bits_t c1, c2;

   printf("zz_mul_2exp...");

   /* test (a*2^b)*2^c = a*2^(b+c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, &r2, NULL);
      c1 = (bits_t) randint(200, state);
      c2 = (bits_t) randint(200, state);

      zz_mul_2exp(t, a, c1);
      zz_mul_2exp(r1, t, c2);

      zz_mul_2exp(r2, a, c1 + c2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      c1 = (bits_t) randint(200, state);
      
      test_zz_aliasing_12i(zz_mul_2exp, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_div_2exp(void)
{
   int result = 1;
   zz_t a, b, r1, r2, t;
   len_t m1, m2;
   bits_t c1, c2;

   printf("zz_div_2exp...");

   /* test (a/2^b)/2^c = a/2^(b+c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, &r2, NULL);
      c1 = (bits_t) randint(200, state);
      c2 = (bits_t) randint(200, state);

      zz_div_2exp(t, a, c1);
      zz_div_2exp(r1, t, c2);

      zz_div_2exp(r2, a, c1 + c2);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w, %w\n", c1, c2); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test (a*2^b)/2^b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(0, ANY, state, &t, &r1, NULL);
      c1 = (bits_t) randint(200, state);
      
      zz_mul_2exp(t, a, c1);
      zz_div_2exp(r1, t, c1);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); 
         bsdnt_printf("%w\n", c1); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;
   
   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      c1 = (bits_t) randint(200, state);
      
      test_zz_aliasing_12i(zz_div_2exp, a, b, c1);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_add(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   
   printf("zz_add...");

   /* test (a + b) + c = a + (b + c) */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_add(r1, a, b);
      zz_add(r1, r1, c);

      zz_add(r2, b, c);
      zz_add(r2, r2, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);
         zz_print_debug(c);
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_add, a, b, c);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_sub(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2;
   len_t m1, m2, m3;
   
   printf("zz_sub...");

   /* test (a - b) - c = (a - c) - b */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_sub(r1, a, b);
      zz_sub(r1, r1, c);

      zz_sub(r2, a, c);
      zz_sub(r2, r2, b);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test (a + b) - b = a */
   TEST_START(2, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, NULL);
      
      zz_add(r1, a, b);
      zz_sub(r1, r1, b);

      result = zz_equal(r1, a);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1);
      }

      gc_cleanup();
   } TEST_END;

   /* test a - b = a + (-b) */
   TEST_START(3, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &c, NULL);
      
      zz_sub(r1, a, b);
      zz_neg(c, b);
      zz_add(r2, a, c);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b);  zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_sub, a, b, c);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_mul(void)
{
   int result = 1;
   zz_t a, b, c, r1, r2, t1, t2;
   len_t m1, m2, m3;
   
   printf("zz_mul...");

   /* test ab = ba */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, NULL);
      
      zz_mul(r1, a, b);
      zz_mul(r2, b, a);
      
      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test a(b + c) = ac + bc */
   TEST_START(1, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, &m3, NULL);

      randoms_signed(m1, ANY, state, &a, NULL);
      randoms_signed(m2, ANY, state, &b, NULL);
      randoms_signed(m3, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &r1, &r2, &t1, &t2, NULL);
      
      zz_add(t1, b, c);
      zz_mul(r1, a, t1);

      zz_mul(t1, a, b);
      zz_mul(t2, a, c);
      zz_add(r2, t1, t2);

      result = zz_equal(r1, r2);

      if (!result) 
      {
         zz_print_debug(a); zz_print_debug(b); 
         zz_print_debug(c); 
         zz_print_debug(r1); zz_print_debug(r2);
      }

      gc_cleanup();
   } TEST_END;

   /* test aliasing */
   TEST_START(aliasing, ITER) 
   {
      randoms_upto(10, ANY, state, &m1, &m2, NULL);

      randoms_signed(m1, ANY, state, &b, NULL);
      randoms_signed(m2, ANY, state, &c, NULL);
      randoms_signed(0, ANY, state, &a, NULL);
      
      test_zz_aliasing_12(zz_mul, a, b, c);

      gc_cleanup();
   } TEST_END;

   return result;
}

int test_zz(void)
{
   long pass = 0;
   long fail = 0;
   
   RUN(test_gc);
   RUN(test_seti_equali);
   RUN(test_set_equal);
   RUN(test_neg);
   RUN(test_swap);
   RUN(test_is_zero);
   RUN(test_cmpi);
   RUN(test_cmp);
   RUN(test_cmpabs);
   RUN(test_addi);
   RUN(test_subi);
   RUN(test_muli);
   RUN(test_mul_2exp);
   RUN(test_div_2exp);
   RUN(test_add);
   RUN(test_sub);
   RUN(test_mul);
   
   printf("%ld of %ld tests pass.\n", pass, pass + fail);

   return (fail != 0);
}

int main(void)
{
   int ret = 0;
   
   printf("\nTesting zz functions:\n");
   
   randinit(&state);
   checkpoint_rand("First Random Word: ");

   ret = test_zz();
	
   randclear(state);

   return ret;
}
