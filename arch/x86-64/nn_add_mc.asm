; Copyright:
; * Antony Vennard 2010. All rights reserved.
; * William Hart 2010. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without 
; modification, are permitted provided that the following conditions are 
; met:
;
;    1. Redistributions of source code must retain the above copyright 
;       notice, this list of conditions and the following disclaimer.
; 
;    2. Redistributions in binary form must reproduce the above copyright 
;       notice, this list of conditions and the following disclaimer in the 
;       documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY Antony Vennard ``AS IS'' AND ANY EXPRESS OR 
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN 
; NO EVENT SHALL Antony Vennard OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
; OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
; DAMAGE.
;
; The views and conclusions contained in the software and documentation are 
; those of the authors and should not be interpreted as representing official 
; policies, either expressed or implied, of Antony Vennard.
;
; =============================================================================

; About: This code is an assembly implementation of
; Bill's nn_add_mc function for x64 processors, influenced
; by his blog and code in nn.c.

; data segment. 
SECTION .data

    WORDSIZE dw 8

; code segment. 
SECTION .code


; global symbols
GLOBAL _nn_add_mc


; function code.

_nn_add_mc:

    ; Stack frame for this function

    push    rbp
    mov     rbp, rsp

    ; arguments to functions. 
    ; arguments are in:
    ; * rdi (a)
    ; * rsi (b)
    ; * rdx (c)
    ; * r8  (m)
    ; * r9  (ci)
    
    xor     r10, r10
    mov     r10, r9
    mov     rcx, 0


_nn_add_mc_loop:

    ; t = 0

    ; t = (dword_t) b[i] + (dword_t) c[i] + (dword_t) ci;

    adc     r10, [rsi + rcx*8]
    adc     r10, [rdx + rcx*8]
    
    ; a[i] = (word_t) t; => a[i] = LOWORD
    ; shl     r10, 8
    mov     [rdi+rcx*8], r10;

    ; ci = (t >> WORD_BITS); a[i] = HIWORD

    ; loop control
    inc     rcx
    cmp     r15, rcx
    jl      _nn_add_mc_loop 

_nn_add_mc_exit:

    ; set return value (rax)
    shr     r10, 8
    mov     rax, r10             ; return carry

    ; destroy stack frame
    pop     rbp

    ; return
    ret



