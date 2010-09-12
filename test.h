#ifndef BSDNT_GENERIC_H
#define BSDNT_GENERIC_H

#include "nn.h"

typedef enum
{
   NN 
} type_t;

typedef enum
{
   ANY, NONZERO, ODD, FULL
} flag_t;

typedef struct node_t
{
   type_t type;
   void * ptr;
   struct node_t * next;
} node_t;

/*
   Send the given error message to stderr.
*/
void talker(const char * str);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag.

   randoms(NONZERO, state, &w1, &w2, NULL);
*/
void randoms(flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to words, generate 
   random words which have the property specified by the flag and
   which are below the given limit.

   randoms_upto(100, ANY, state, &w1, &w2, NULL);
*/
void randoms_upto(word_t limit, flag_t flag, rand_t state, ...);

/*
   Given a NULL terminated list of *pointers* to nn_t's, generate 
   random integers which have the property specified by the flag and
   which have the specified number of limbs. These can only be
   cleaned up by calling gc_cleanup() (see below).

   randoms_upto(100, FULL, state, &a, &b, NULL);
*/
void randoms_of_len(len_t n, flag_t flag, rand_t state, ...);

/*
   Clean up any objects allocated by the random functions above.
   Note *all* objects are cleaned up. We have no way of knowing 
   which are still in use, so it is assumed none of them are still
   in use.
*/
void gc_cleanup(void);

#endif

