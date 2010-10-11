/* 
  Copyright (C) 2010, William Hart
  Copyright (C) 2010, Brian Gladman

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

#include "nn.h"
#include "nn_linear_x64_asm.h"

#ifndef HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t carry)
{
	long i = 0;

	while(i < m)
    {   
		word_t sum = b[i] + c[i] + carry;
        carry = (sum < c[i] ? 1 : sum > c[i] ? 0 : carry);
        a[i++] = sum;
    }

	return carry;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t borrow)
{
	long i = 0;

	while(i < m)
    {   
        word_t diff = b[i] - c[i] - borrow;
        borrow = (diff > b[i] ? 1 : diff < b[i] ? 0 : borrow);
        a[i++] = diff;
    }

	return borrow;
}

#endif

#ifndef HAVE_ARCH_nn_shl_c

word_t nn_shl_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t carry)
{
	if(bits == 0)
	{
		if(a != b)
			memcpy(a, b, m * sizeof(word_t));
	}
	else
	{
		long i = -1;
		while(++i < m)
		{
			word_t t = b[i];
			a[i] = (t << bits) + carry;
			carry = t >> (WORD_BITS - bits);
		}
	}

	return carry;
}

#endif

#ifndef HAVE_ARCH_nn_shr_c

word_t nn_shr_c(nn_t a, nn_src_t b, len_t m, bits_t bits, word_t carry)
{
	if(bits == 0)
	{
		if(a != b)
			memcpy(a, b, m * sizeof(word_t));
	}
	else
	{
		long i = m;

		while(i--)
		{
			word_t t = b[i];
			a[i] = (t >> bits) + carry;
			carry = t << (WORD_BITS - bits); 
		}

	}

	return carry;
}

#endif

#ifndef HAVE_ARCH_nn_add1

word_t nn_add1(nn_t a, nn_src_t b, len_t m, word_t carry)
{
	if(m)
	{
		long i = 0;
		carry = (a[0] = b[0] + carry) < carry ? 1 : 0;
    
		if(a != b)
			while(++i < m)
				carry = (a[i] = b[i] + carry) ? 0 : carry;
		else
			while(++i < m && carry)
				carry = (a[i] = b[i] + carry) ? 0 : carry;
	}

	return carry;
}

#endif

#ifndef HAVE_ARCH_nn_sub1

word_t nn_sub1(nn_t a, nn_src_t b, len_t m, word_t borrow)
{
	if(m)
    {
		long i = 0;
		word_t t = b[0];
		borrow =  ((a[0] = t - borrow) > t ? 1 : 0);

        if(a != b)
            while(++i < m)
                borrow = ~(a[i] = b[i] - borrow) ? 0 : borrow;
        else
            while(++i < m && borrow)
                borrow = ~(a[i] = b[i] - borrow) ? 0 : borrow;
    }

	return borrow;
}

#endif

#ifndef HAVE_ARCH_nn_neg_c

word_t nn_neg_c(nn_t a, nn_src_t b, len_t m, word_t carry)
{
	if(m)
	{	
		long i = -1;	
		carry = 1 - carry;

		while(carry && ++i < m)
		{
			word_t t = ~b[i];
			carry = (a[i] = t + carry) < t ? 1 : 0;
		}

		while(++i < m)
			a[i] = ~b[i];

		carry = 1 - carry;
	}

   return carry;
}

#endif

#ifndef HAVE_ARCH_nn_mul1_c

word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
	long i;
	for( i = 0 ; i < m ; ++i )
	{
		word_t hi;
		a[i] = mul_64_by_64(b[i], c, &hi) + ci;
		ci = hi + (a[i] < ci ? 1 : 0);
	}
	return ci;
}

#endif

#ifndef HAVE_ARCH_nn_addmul1_c

word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
	long i;
	for( i = 0 ; i < m ; ++i )
	{
		word_t hi, lo;
		a[i] += (lo = mul_64_by_64(b[i], c, &hi) + ci);
		ci = hi + (lo < ci ? 1 : 0) + (a[i] < lo ? 1 : 0);
	}
	return ci;
}

#endif

#ifndef HAVE_ARCH_nn_muladd1_c

word_t nn_muladd1_c(nn_t r, nn_src_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   long i;
	for( i = 0 ; i < m ; ++i )
	{
		word_t hi, lo;
		r[i] = a[i] + (lo = mul_64_by_64(b[i], c, &hi) + ci);
		ci = hi + (lo < ci ? 1 : 0) + (r[i] < lo ? 1 : 0);
	}
   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_submul1_c

word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
	long i;

	for( i = 0 ; i < m ; ++i )
	{
		word_t hi, lo;
		lo = mul_64_by_64(b[i], c, &hi) + ci;
		ci = hi + (lo < ci ? 1 : 0) + (a[i] < lo ? 1 : 0);
		a[i] -= lo;
	}
	return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_simple_c

word_t nn_divrem1_simple_c(nn_t q, nn_src_t a, len_t m, word_t d, word_t ci)
{
	dword_t t;
	long i;

	for( i = m - 1 ; i >= 0; --i )
	{
		t.lo = a[i];
		t.hi = ci;
		q[i] = div_128_by_64(&t,  d, &ci);
	}
	return ci;
}

#endif

#ifndef HAVE_ARCH_nn_divrem1_preinv_c

word_t nn_divrem1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                            word_t d, preinv1_t inv, word_t ci)
{
	dword_t t;
	long i;
	word_t norm = inv.norm;
	word_t dinv = inv.dinv;
   
	d <<= norm;
	ci <<= norm;

	for( i = m - 1 ; i >= 0 ; --i )
	{
		t.lo = a[i] << norm;
		t.hi = (norm ? (a[i] >> (WORD_BITS - norm)) : 0) + ci;
		divrem21_preinv1(q[i], ci, t, d, dinv);
	}
	return (ci >> norm);
}

#endif

#ifndef HAVE_ARCH_nn_divrem_hensel1_preinv_c

word_t nn_divrem_hensel1_preinv_c(nn_t q, nn_src_t a, len_t m, 
                        word_t d, hensel_preinv1_t inv, word_t ci)
{
	long i;
	dword_t t, r;

	for( i = 0 ; i < m ; ++i )
	{
		t.lo = a[i] - ci;
		t.hi = (a[i] < ci ? (word_t)-1 : 0);
		q[i] = t.lo * inv;
		r.lo = mul_64_by_64(q[i], d, &r.hi);
		ci = r.hi - t.hi;
   }
   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mod1_preinv_c

word_t nn_mod1_preinv_c(nn_src_t a, len_t m, word_t d, 
                                     mod_preinv1_t inv, word_t ci)
{
	dword_t t, u;
	word_t a1, a0;
   
	if (m & 1) /* odd number of words plus carry word */
	{
		a1 = ci;
		a0 = a[--m];
	} 
	else /* even number of words plus carry word */
	{
		if (m == 0)
			return ci;
		m -= 2;
		u.lo = mul_64_by_64(ci, inv.b2, &u.hi) + a[m];
		u.hi += a[m + 1] + (u.lo < a[m] ? 1 : 0);
		if(u.hi < a[m + 1] || u.hi == a[m + 1] && u.lo < a[m]) 
		{
			u.lo += inv.b2;
			u.hi += (u.lo < inv.b2 ? 1 : 0);
		}
		a1 = u.hi;
		a0 = u.lo;
   }

   /* reduce to two words */
   while (m >= 2)
   {
		m -= 2;
		t.lo = mul_64_by_64(a0, inv.b2, &t.hi) + a[m];
		t.hi += a[m + 1] + (t.lo < a[m] ? 1 : 0);
		if(t.hi < a[m + 1] || t.hi == a[m + 1] && t.lo < a[m]) 
		{
			t.lo += inv.b2;
			t.hi += (t.lo < inv.b2 ? 1 : 0);
		}
		u.lo = mul_64_by_64(a1, inv.b3, &u.hi) + t.lo;
		u.hi += t.hi + (u.lo < t.lo ? 1 : 0);
		if(u.hi < t.hi || u.hi == t.hi && u.lo < t.lo) 
		{
			u.lo += inv.b2;
			u.hi += (u.lo < inv.b2 ? 1 : 0);
		}
		a1 = u.hi;
		a0 = u.lo;
   }
   
   /* reduce top word mod d */
	u.lo = mul_64_by_64(a1, inv.b1, &u.hi) + a0;
	u.hi += (u.lo < a0 ? 1 : 0);
	div_128_by_64(&u, d, &a0);
	return a0;
}

#endif

