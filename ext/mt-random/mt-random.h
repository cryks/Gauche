/*
 * mt-random.h - implements MT19937 random number generation algorithm
 * This code is based on Makoto Matsumoto & Takuji Nishimura's mt18837ar.c
 * See mt-random.c for details.
 * $Id: mt-random.h,v 1.5 2002-05-27 22:12:15 shirok Exp $
 * The original copyright notice follows.
 */
/*
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

#include "gauche.h"
#include "gauche/uvector.h"
#include "gauche/extend.h"

#define N 624

typedef struct ScmMersenneTwisterRec {
    SCM_HEADER;
    unsigned long mt[N]; /* the array for the state vector  */
    int mti;             /* mti==N+1 means mt[N] is not initialized */
} ScmMersenneTwister;

SCM_CLASS_DECL(Scm_MersenneTwisterClass);
#define SCM_MERSENNE_TWISTER(obj)   ((ScmMersenneTwister*)obj)
#define SCM_MERSENNE_TWISTER_P(obj) SCM_XTYPEP(obj, &Scm_MersenneTwisterClass)

extern void Scm_MTInitByUI(ScmMersenneTwister *mt, unsigned long s);
extern void Scm_MTInitByArray(ScmMersenneTwister *mt,
                              SCM_UVECTOR_INT32 init_key[],
                              unsigned long key_length);

extern unsigned long Scm_MTGenrandU32(ScmMersenneTwister *);
extern float         Scm_MTGenrandF32(ScmMersenneTwister *, int);
extern double        Scm_MTGenrandF64(ScmMersenneTwister *, int);
extern ScmObj        Scm_MTGenrandInt(ScmMersenneTwister *mt, ScmObj n);
