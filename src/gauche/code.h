/*
 * code.h - Virtual machine code
 *
 *   Copyright (c) 2005 Shiro Kawai, All rights reserved.
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  $Id: code.h,v 1.2 2005-04-12 01:42:28 shirok Exp $
 */

#ifndef GAUCHE_CODE_H
#define GAUCHE_CODE_H

#include <gauche/vm.h>

/*
 * Compiled code packet
 */

struct ScmCompiledCodeRec {
    SCM_HEADER;
    ScmWord *code;              /* Code vector.  this is allocated as atomic,
                                   to prevent GC from scanning it. */
    ScmObj *constants;          /* Constant vector.  this isn't used during
                                   execution, but kept here so that the
                                   constants in the code vector won't be
                                   GC-ed. */
    int codeSize;               /* size of code vector */
    int constantSize;           /* size of constant vector */
    int maxstack;               /* maximum runtime stack depth */
    u_short requiredArgs;       /* # of required args, if this code is the
                                   body of a closure.  Otherwise 0. */
    u_short optionalArgs;       /* 1 if this code is the body of a closure.
                                   that takes rest arg.  Otherwise 0. */
    ScmObj name;                /* If this is the body of a closure, holds
                                   its name.  Otherwise #f. */
    ScmObj info;                /* debug info.  alist of instruction offset
                                   and info. */
    ScmObj argInfo;             /* If this code is the body of the closure,
                                   keeps a list of args.  #f otherwise. */
    ScmObj parent;              /* ScmCompiledCode if this code is compiled
                                   within other code chunk.  #f otherwise. */
    ScmObj intermediateForm;    /* An intermediate form (the result of pass1
                                   of the compiler) of the body.  It is used
                                   to inline this procedure.  Only set if
                                   the procedure is defined with define-inline.
                                   #f otherwise. */
    void *builder;              /* An opaque data used during consturcting
                                   the code vector.  Usually NULL. */
};

SCM_CLASS_DECL(Scm_CompiledCodeClass);
#define SCM_CLASS_COMPILED_CODE   (&Scm_CompiledCodeClass)

#define SCM_COMPILED_CODE(obj)    ((ScmCompiledCode*)(obj))
#define SCM_COMPILED_CODE_P(obj)  SCM_XTYPEP(obj, SCM_CLASS_COMPILED_CODE)
#define SCM_COMPILED_CODE_ARG_INFO(obj) (SCM_COMPILED_CODE(obj)->argInfo)
#define SCM_COMPILED_CODE_REQUIRED_ARGS(obj) \
    (SCM_COMPILED_CODE(obj)->requiredArgs)
#define SCM_COMPILED_CODE_OPTIONAL_ARGS(obj) \
    (SCM_COMPILED_CODE(obj)->optionalArgs)

#define SCM_COMPILED_CODE_CONST_INITIALIZER(code, codesize, maxstack, reqargs, optargs, name, info, arginfo, parent, iform) \
    { { SCM_CLASS2TAG(SCM_CLASS_COMPILED_CODE) },        \
      (code), NULL, (codesize), 0, (maxstack),           \
      (reqargs), (optargs), (name), (info), (arginfo),   \
      (parent), (iform) }

SCM_EXTERN void   Scm_CompiledCodeDump(ScmCompiledCode *cc);
SCM_EXTERN ScmObj Scm_CompiledCodeToList(ScmCompiledCode *cc);
SCM_EXTERN ScmObj Scm_CompiledCodeFullName(ScmCompiledCode *cc);
SCM_EXTERN void   Scm_VMExecuteToplevels(ScmCompiledCode *cv[]);

/* Builder API */
SCM_EXTERN ScmObj Scm_MakeCompiledCodeBuilder(int reqargs, int optargs,
                                              ScmObj name, ScmObj parent,
                                              ScmObj intForm);
SCM_EXTERN ScmObj Scm_CompiledCodeCurrentInsn(ScmCompiledCode *cc);
SCM_EXTERN void   Scm_CompiledCodeReplaceInsn(ScmCompiledCode *cc,
                                              ScmObj insn,
                                              ScmObj operand,
                                              ScmObj info);
SCM_EXTERN void   Scm_CompiledCodeFlushInsn(ScmCompiledCode *cc);
SCM_EXTERN void   Scm_CompiledCodePutInsn(ScmCompiledCode *cc,
                                          ScmObj insn,
                                          ScmObj operand,
                                          ScmObj info);
SCM_EXTERN ScmObj Scm_CompiledCodeNewLabel(ScmCompiledCode *cc);
SCM_EXTERN void   Scm_CompiledCodeSetLabel(ScmCompiledCode *cc, ScmObj label);
SCM_EXTERN void   Scm_CompiledCodeFinishBuilder(ScmCompiledCode *cc,
                                                int maxstack);

/*
 * Compiler auxiliary API
 */
SCM_EXTERN ScmObj Scm_CompilerEnvLookup(ScmObj cenv, ScmObj name,
                                        ScmObj lookupAs);


/*
 * VM instructions
 */
#define SCM_VM_INSN_ARG_MAX          ((1L<<(32-13))-1)
#define SCM_VM_INSN_ARG_MIN          (-SCM_VM_INSN_ARG_MAX)
#define SCM_VM_INSN_ARG_FITS(k) \
    (((k)<=SCM_VM_INSN_ARG_MAX)&&((k)>=SCM_VM_INSN_ARG_MIN))

/* Macros for transition to the packed code vector of NVM.
   In the packed code vector, VM insns are stored untagged.
   It eliminates the shift in the dispatcher. */
#define SCM_VM_INSN_CODE(obj)       (SCM_WORD(obj)&0x0ff)
#define SCM_VM_INSN_ARG(obj)        ((signed long)SCM_WORD(obj) >> 8)
#define SCM_VM_INSN_ARG0(obj)       ((SCM_WORD(obj) >>  8) & 0x03ff)
#define SCM_VM_INSN_ARG1(obj)       ((SCM_WORD(obj) >> 18) & 0x03ff)

#define SCM_VM_INSN(code)           SCM_WORD(code)
#define SCM_VM_INSN1(code, arg)     SCM_WORD((long)((arg)<<8) | (code))
#define SCM_VM_INSN2(code, arg0, arg1)  \
    SCM_WORD((long)((arg1) << 18) | ((arg0) << 8) | (code))

/* Operand type */
enum {
    SCM_VM_OPERAND_NONE,        /* take no operand */
    SCM_VM_OPERAND_OBJ,         /* take ScmObj */
    SCM_VM_OPERAND_CODE,        /* take ScmCompiledCode */
    SCM_VM_OPERAND_CODES,       /* take a list of ScmCompiledCodes */
    SCM_VM_OPERAND_ADDR,        /* take address of next code */
    SCM_VM_OPERAND_OBJ_ADDR     /* take an object and address of next code */
};

SCM_EXTERN const char *Scm_VMInsnName(u_int code);
SCM_EXTERN int Scm_VMInsnNumParams(u_int code);
SCM_EXTERN int Scm_VMInsnOperandType(u_int code);
SCM_EXTERN int Scm_VMInsnNameToCode(ScmObj name);
SCM_EXTERN ScmWord Scm_VMInsnBuild(ScmObj insn);



#endif /* GAUCHE_CODE_H */