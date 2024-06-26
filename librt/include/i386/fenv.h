/*-
 * Copyright (c) 2004-2005 David Schultz <das@FreeBSD.ORG>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/msun/i387/fenv.h,v 1.8 2011/10/10 15:43:09 das Exp $
 */

#ifndef	_FENV_H_
#define	_FENV_H_

/* Includes */
#include <os/osdefs.h>

#ifndef	__fenv_static
#define	__fenv_static	static
#endif

#ifndef	__fenv_extern
#define	__fenv_extern	extern
#endif

/* Exception flags */
#define	FE_INVALID	0x01
#define	FE_DENORMAL	0x02
#define	FE_DIVBYZERO	0x04
#define	FE_OVERFLOW	0x08
#define	FE_UNDERFLOW	0x10
#define	FE_INEXACT	0x20
#define	FE_ALL_EXCEPT	(FE_DIVBYZERO | FE_DENORMAL | FE_INEXACT | \
			 FE_INVALID | FE_OVERFLOW | FE_UNDERFLOW)

/* Rounding modes */
#define	FE_TONEAREST	0x0000
#define	FE_DOWNWARD	0x0400
#define	FE_UPWARD	0x0800
#define	FE_TOWARDZERO	0x0c00
#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)

/* As compared to the x87 control word, the SSE unit's control word
 * has the rounding control bits offset by 3 and the exception mask
 * bits offset by 7. */
#define	_SSE_ROUND_SHIFT	3
#define	_SSE_EMASK_SHIFT	7

/* Inline helpers for both MSVC assembler, 
 * but also for GCC assembler, unfortunately
 * i don't know how to do the clober thing in msvc */
#if defined(_MSC_VER) && !defined(__clang__)
#define	__fldcw(__cw)		_asm fldcw dword ptr [__cw]
#define	__fldenv(__env)		_asm fldenv dword ptr [__env]
#define	__fldenvx(__env)	_asm fldenv dword ptr [__env]
#define	__fnclex()			_asm fnclex
#define	__fnstenv(__env)	_asm fnstenv dword ptr [__env]
#define	__fnstcw(__cw)		_asm fnstcw dword ptr [__cw]
#define	__fnstsw(__sw)		_asm fnstsw dword ptr [__sw]
#define	__fwait()			_asm fwait
#define	__ldmxcsr(__csr)	_asm ldmxcsr dword ptr [__csr]
#define	__stmxcsr(__csr)	_asm stmxcsr dword ptr [__csr]
#else
#define	__fldcw(__cw)		__asm __volatile("fldcw %0" : : "m" (__cw))
#define	__fldenv(__env)		__asm __volatile("fldenv %0" : : "m" (__env))
#define	__fldenvx(__env)	__asm __volatile("fldenv %0" : : "m" (__env)  \
				: "st", "st(1)", "st(2)", "st(3)", "st(4)",   \
				"st(5)", "st(6)", "st(7)")
#define	__fnclex()		__asm __volatile("fnclex")
#define	__fnstenv(__env)	__asm __volatile("fnstenv %0" : "=m" (*(&__env)))
#define	__fnstcw(__cw)		__asm __volatile("fnstcw %0" : "=m" (*(&__cw)))
#define	__fnstsw(__sw)		__asm __volatile("fnstsw %0" : "=am" (*(&__sw)))
#define	__fwait()		__asm __volatile("fwait")
#define	__ldmxcsr(__csr)	__asm __volatile("ldmxcsr %0" : : "m" (__csr))
#define	__stmxcsr(__csr)	__asm __volatile("stmxcsr %0" : "=m" (*(&__csr)))
#endif

/* For now, support only for the basic abstraction of flags that are
 * either set or clear. fexcept_t could be  structure that holds more
 * info about the fp environment. */
typedef unsigned short fexcept_t;

/* This 32-byte struct represents the entire floating point
 * environment as stored by fnstenv or fstenv, augmented by
 * the  contents of the MXCSR register, as stored by stmxcsr
 * (if CPU supports it). */
typedef struct {
	unsigned short __control_word;
	unsigned short __unused0;
	unsigned short __status_word;
	unsigned short __unused1;
	unsigned short __tag_word;
	unsigned short __unused2;
	unsigned int   __ip_offset;    /* instruction pointer offset */
	unsigned short __ip_selector;
	unsigned short __opcode;
	unsigned int   __data_offset;
	unsigned short __data_selector;
	unsigned short __unused3;

	/* contents of the MXCSR register 
	 * This is not seen by the fpu */
	unsigned int   __mxcsr;
} fenv_t;

/*The C99 standard (7.6.9) allows us to define 
 * implementation-specific macros for different fp environments */
_CRTIMP extern __CONST fenv_t __fe_dfl_env;
#define	FE_DFL_ENV	(&__fe_dfl_env)

/* The SSE status */
enum __sse_support { 
	__SSE_YES, 
	__SSE_NO, 
	__SSE_UNK 
};

/* After testing for SSE support once 
 * we cache the result in __has_sse. */
_CRTIMP enum __sse_support __has_sse;
_CRTIMP int __test_sse(void);
#ifdef __SSE__
#define	__HAS_SSE()	1
#else
#define	__HAS_SSE()	(__has_sse == __SSE_YES ||			\
			 (__has_sse == __SSE_UNK && __test_sse()))
#endif

/* Prototypes which we don't inline */
_CRTIMP int fesetexceptflag(__CONST fexcept_t *__flagp, int __excepts);
_CRTIMP int feraiseexcept(int __excepts);
_CRTIMP int fegetenv(fenv_t *__envp);
_CRTIMP int feholdexcept(fenv_t *__envp);
_CRTIMP int feupdateenv(__CONST fenv_t *__envp);
_CRTIMP int feenableexcept(int __mask);
_CRTIMP int fedisableexcept(int __mask);

__fenv_static __CRT_INLINE 
int feclearexcept(int __excepts)
{
	fenv_t __env;
	uint32_t __mxcsr;

	if (__excepts == FE_ALL_EXCEPT) {
		__fnclex();
	} else {
		__fnstenv(__env);
		__env.__status_word &= ~__excepts;
		__fldenv(__env);
	}
	if (__HAS_SSE()) {
		__stmxcsr(__mxcsr);
		__mxcsr &= ~__excepts;
		__ldmxcsr(__mxcsr);
	}
	return (0);
}

__fenv_static __CRT_INLINE 
int fegetexceptflag(fexcept_t *__flagp, int __excepts)
{
	uint32_t __mxcsr;
	uint16_t __status;

	__fnstsw(__status);
	if (__HAS_SSE())
		__stmxcsr(__mxcsr);
	else
		__mxcsr = 0;
	*__flagp = (fexcept_t)((__mxcsr | __status) & __excepts);
	return (0);
}

__fenv_static __CRT_INLINE 
int fetestexcept(int __excepts)
{
	uint32_t __mxcsr;
	uint16_t __status;
	__fnstsw(__status);
	if (__HAS_SSE())
		__stmxcsr(__mxcsr);
	else __mxcsr = 0;
	return ((__status | __mxcsr) & __excepts);
}

/* We assume that the x87 and the SSE unit agree on the
 * rounding mode.  Reading the control word on the x87 turns
 * out to be about 5 times faster than reading it on the SSE
 * unit on an Opteron 244. */
__fenv_static __CRT_INLINE 
int fegetround(void)
{
	uint16_t __control;
	__fnstcw(__control);
	return (__control & _ROUND_MASK);
}

__fenv_static __CRT_INLINE 
int fesetround(int __round)
{
	uint32_t __mxcsr;
	uint16_t __control;

	if (__round & ~_ROUND_MASK)
		return (-1);

	__fnstcw(__control);
	__control &= ~_ROUND_MASK;
	__control |= __round;
	__fldcw(__control);

	if (__HAS_SSE()) {
		__stmxcsr(__mxcsr);
		__mxcsr &= ~(_ROUND_MASK << _SSE_ROUND_SHIFT);
		__mxcsr |= __round << _SSE_ROUND_SHIFT;
		__ldmxcsr(__mxcsr);
	}

	return (0);
}

/* XXX Using fldenvx() instead of fldenv() tells the compiler that this
 * instruction clobbers the i387 register stack.  This happens because
 * we restore the tag word from the saved environment.  Normally, this
 * would happen anyway and we wouldn't care, because the ABI allows
 * function calls to clobber the i387 regs.  However, fesetenv() is
 * inlined, so we need to be more careful. */
__fenv_static __CRT_INLINE 
int fesetenv(__CONST fenv_t *__envp)
{
	fenv_t __env = *__envp;
	uint32_t __mxcsr = __env.__mxcsr;
	__env.__mxcsr = 0xFFFFFFFF;
	__fldenvx(__env);
	if (__HAS_SSE())
		__ldmxcsr(__mxcsr);
	return (0);
}

/* We currently provide no external definition of fegetexcept(). 
 * We assume that the masks for the x87 and the SSE unit are
 * the same. */
__fenv_static __CRT_INLINE 
int fegetexcept(void) 
{
	uint16_t __control = 0;
	__fnstcw(__control);
	return (~__control & FE_ALL_EXCEPT);
}

#endif	/* !_FENV_H_ */
