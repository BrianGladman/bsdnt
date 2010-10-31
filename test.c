/* 
  Copyright (C) 2010, William Hart

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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "test.h"

node_t * garbage = NULL;

void talker(const char * str)
{
   fprintf(stderr, "Error: %s\n", str);
}

node_t * new_node(type_t type, void * ptr, len_t length, node_t * next)
{
   node_t * node = malloc(sizeof(node_t));
   node->type = type;
   
   if (type == NN)
      node->ptr = ptr;

   node->length = length;
   
   node->next = next;

   return node;
}

nn_t alloc_redzoned_nn(len_t n)
{
   nn_t a = nn_init(n + 2*REDZONE_WORDS);
   char * redzone1 = (char *) a;
   char * redzone2 = (char *) (a + REDZONE_WORDS + n);
   long i;

   for (i = 0; i < REDZONE_WORDS*sizeof(word_t); i++)
   {
      redzone1[i] = REDZONE_BYTE;
      redzone2[i] = REDZONE_BYTE;
   }
  
   return a + REDZONE_WORDS;
}

void free_redzoned_nn(nn_t a, len_t n)
{
   char * redzone1 = (char *) (a - REDZONE_WORDS);
   char * redzone2 = (char *) (a + n);
   long i;

   for (i = 0; i < REDZONE_WORDS*sizeof(word_t); i++)
   {
      if (redzone1[i] != REDZONE_BYTE)
      {
         fprintf(stderr, "Underrun detected in nn_t at %p of "
         "length %ld at byte %ld\n", (void *) a, n, i);
         abort();
      }
      if (redzone2[i] != REDZONE_BYTE)
      {
         fprintf(stderr, "Overrun detected in nn_t at %p of "
         "length %ld at byte %ld\n", (void *) a, n, i);
         abort();
      }
   }

   free(a - REDZONE_WORDS);
}

void randoms(flag_t flag, rand_t state, ...)
{
   va_list ap;
   word_t * w;

   va_start(ap, state);
   
   while ((w = va_arg(ap, word_t *)) != NULL) 
   {
      (*w) = randword(state);

      switch (flag)
      {
      case ANY: break;

      case ODD: 
         (*w) |= 1;
         break;

      case NONZERO: 
         while ((*w) == 0)
            (*w) = randword(state); 
         break;

      default: talker("Unknown flag in randoms.");
      }
   }

   va_end(ap);
}

void randoms_upto(word_t limit, flag_t flag, rand_t state, ...)
{
   va_list ap;
   word_t * w;

   va_start(ap, state);
   
   if (limit == 0)
      talker("Limit too low in randoms_upto");
      
   while ((w = va_arg(ap, word_t *)) != NULL) 
   {
      (*w) = randint(limit, state);

      switch (flag)
      {
      case ANY: break;

      case ODD: 
         if (limit == 1)
            talker("Limit too low in randoms_upto");
         (*w) |= 1;
         while ((*w) >= limit)
            (*w) = randint(limit, state) & 1; 
         break;

      case NONZERO: 
         if (limit == 1)
            talker("Limit too low in randoms_upto");
         while ((*w) == 0)
            (*w) = randint(limit, state); 
         break;

      default: talker("Unknown flag in randoms_upto.");
      }
   }

   va_end(ap);
}

void randoms_of_len(len_t n, flag_t flag, rand_t state, ...)
{
   va_list ap;
   nn_t * obj;

   va_start(ap, state);
   
   while ((obj = va_arg(ap, nn_t *)) != NULL) 
   {
      (*obj) = alloc_redzoned_nn(n);
      nn_test_random(*obj, state, n);

      switch (flag)
      {
      case ANY: break;
      case FULL: while (nn_normalise(*obj, n) != n)
                    nn_test_random(*obj, state, n);
                 break;
      case ODD: (*obj)[0] |= (word_t) 1; break;
      default: talker("Unknown flag in randoms_of_len");
         abort();
      }

      garbage = new_node(NN, (void *) (*obj), n, garbage);
   } 

   va_end(ap);
}

void free_obj(node_t * obj)
{
   if (obj->type == NN)
      free_redzoned_nn(obj->ptr, obj->length);
}

void gc_cleanup(void)
{
   node_t * g = garbage;
   node_t * temp;

   while (g != NULL)
   {
      free_obj(g);

      temp = g;
      g = g->next;
      free(temp);
   }

   garbage = NULL;
}