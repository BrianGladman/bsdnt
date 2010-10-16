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

#ifndef HELPER_ARCH_H
#define HELPER_ARCH_H

#include <stdint.h>
#include <limits.h>
#include <assert.h>
#include "config.h"

#define HAVE_ARCH_INTRINSICS

#include <crtdbg.h>
#include <intrin.h>

#pragma intrinsic(_BitScanReverse)
__inline uint32_t high_zero_bits(word_t x)
{
	uint32_t pos;
	_ASSERT(x != 0);
	_BitScanReverse(&pos, x);
	return WORD_BITS - 1 - pos;
}

#pragma intrinsic(_BitScanForward)
__inline uint32_t low_zero_bits(word_t x)
{
	uint32_t pos;
	_ASSERT(x != 0);
	_BitScanForward(&pos, x);
	return pos;
}

#endif
