/* 
  George Marsaglia's KISS 64-bit Pseudo Random Number Generator

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

#include "../helper.h"

#if WORD_BITS == 64

#include <stdlib.h>
#include "internal_rand.h"

typedef struct kiss_ctx
{   word_t x, c, y, z;
} kiss_ctx;

#define CTX(x) ((kiss_ctx*)(x))

rand_t kiss_init(void)
{
	rand_t c = malloc(sizeof(kiss_ctx));

   CTX(c)->x = WORD(1234567890987654321);
	CTX(c)->c = WORD(123456123456123456); 
   CTX(c)->y = WORD(362436362436362436);
	CTX(c)->z = WORD(1066149217761810);
    
   return c;
}

void kiss_clear(rand_t c)
{
	free(c);
}

word_t kiss_word(rand_t c)
{   
   word_t t;

	t = (CTX(c)->x << 58) + CTX(c)->c;

	CTX(c)->c = CTX(c)->x >> 6;
	CTX(c)->x += t;
	CTX(c)->c += (CTX(c)->x < t);

	CTX(c)->y ^= (CTX(c)->y << 13);
	CTX(c)->y ^= (CTX(c)->y >> 17);
	CTX(c)->y ^= (CTX(c)->y << 43);

   CTX(c)->z = WORD(6906969069) * CTX(c)->z + WORD(1234567);

	return CTX(c)->x + CTX(c)->y + CTX(c)->z;
}

#endif