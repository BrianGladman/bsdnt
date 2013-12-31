/* Copyright 2010 (C) Gonzalo Tornaria, Antony Vennard, Brian Gladman, William Hart */

#ifdef __cplusplus
 extern "C" {
#endif

#ifndef HAVE_ARCH_nn_add_mc
#define HAVE_ARCH_nn_add_mc

word_t nn_add_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "jrcxz 3f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    bt  $0, %%rax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    adc (%%rdx,%%rcx,8), %%r10; \
    mov %%r10, (%%rdi,%%rcx,8); \
    leaq 1(%%rcx), %%rcx; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc %%al; \
3:;"

   : "=a" (ci)
   : "a" (ci), "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_sub_mc
#define HAVE_ARCH_nn_sub_mc

word_t nn_sub_mc(nn_t a, nn_src_t b, nn_src_t c, len_t m, word_t ci)
{
   __asm__ __volatile__ (

   "jrcxz 3f; \
    leaq (%%rdi,%%rcx,8), %%rdi; \
    leaq (%%rsi,%%rcx,8), %%rsi; \
    leaq (%%rdx,%%rcx,8), %%rdx; \
    neg %%rcx; \
    bt  $0, %%rax; \
1:; \
    movq (%%rsi,%%rcx,8), %%r10; \
    sbb (%%rdx,%%rcx,8), %%r10; \
    movq %%r10, (%%rdi,%%rcx,8); \
    lea 1(%%rcx), %%rcx; \
    jrcxz 2f; \
    jmp 1b; \
2:; \
    setc %%al; \
3:;"

   : "=a" (ci)
   : "a" (ci), "c" (m), "d" (c), "S" (b), "D" (a)
   );

   return ci;
}

#endif

#ifndef HAVE_ARCH_nn_mul1_c
#define HAVE_ARCH_nn_mul1_c

word_t nn_mul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        movq  %%rax, (%%rdi,%%r11,8); \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_addmul1_c
#define HAVE_ARCH_nn_addmul1_c

word_t nn_addmul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        addq  %%rax, (%%rdi,%%r11,8); \
        adcq  $0, %%rdx; \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifndef HAVE_ARCH_nn_submul1_c
#define HAVE_ARCH_nn_submul1_c

word_t nn_submul1_c(nn_t a, nn_src_t b, len_t m, word_t c, word_t ci)
{
   register word_t r8 asm("r8") = ci;

   __asm__ __volatile__ (

       "testq %%rdx, %%rdx; \
        lea   (%%rsi,%%rdx,8), %%rsi; \
        lea   (%%rdi,%%rdx,8), %%rdi; \
        jle   2f; \
        neg   %%rdx; \
        movq  %%rdx, %%r11; \
1:; \
        movq  (%%rsi,%%r11,8), %%rax; \
        mulq  %%rcx; \
        addq  %%r8, %%rax; \
        adcq  $0, %%rdx; \
        subq  %%rax, (%%rdi,%%r11,8); \
        adcq  $0, %%rdx; \
        movq  %%rdx, %%r8; \
        addq  $1, %%r11; \
        jnz   1b; \
2:; \
        movq %%r8, %%rax;"

   : "=r" (r8)
   : "r" (r8), "d" (m), "S" (b), "D" (a)
   );

   return r8;
}

#endif

#ifdef __cplusplus
 }
#endif
