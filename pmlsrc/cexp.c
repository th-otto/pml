/************************************************************************
 *									*
 *				N O T I C E				*
 *									*
 *			Copyright Abandoned, 1987, Fred Fish		*
 *									*
 *	This previously copyrighted work has been placed into the	*
 *	public domain by the author (Fred Fish) and may be freely used	*
 *	for any purpose, private or commercial.  I would appreciate	*
 *	it, as a courtesy, if this notice is left in all copies and	*
 *	derivative works.  Thank you, and enjoy...			*
 *									*
 *	The author makes no warranty of any kind with respect to this	*
 *	product and explicitly disclaims any implied warranties of	*
 *	merchantability or fitness for any particular purpose.		*
 *									*
 ************************************************************************
 */


/*
 *  FUNCTION
 *
 *	cexp   complex double precision exponential
 *
 *  KEY WORDS
 *
 *	cexp
 *	complex functions
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Computes double precision complex exponential of
 *	a double precision complex argument.
 *
 *  USAGE
 *
 *	COMPLEX cexp (z)
 *	COMPLEX z;
 *
 *  REFERENCES
 *
 *	Fortran 77 user's guide, Digital Equipment Corp. pp B-13
 *
 *  PROGRAMMER
 *
 *	Fred Fish
 *	Tempe, Az 85281
 *	(602) 966-8871
 *
 *  INTERNALS
 *
 *	Computes complex exponential of z = x + j y from:
 *
 *		1.	r_cexp = exp(x) * cos(y)
 *
 *		2.	i_cexp = exp(x) * sin(y)
 *
 *		3.	cexp(z) = r_cexp + j i_cexp
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)


COMPLEX cexp (z)
COMPLEX z;
{
    COMPLEX result;
    double expx;

    expx = exp (z.real);
    result.real = expx * cos (z.imag);
    result.imag = expx * sin (z.imag);
    return (result);
}

#else	!defined (__M68881__) && !defined (sfp004)
#ifdef	ERROR_CHECK
__asm(
".text\t\n"
".even\t\n"
"_funcname:\t\n"
"	.ascii \"cexp\\0\"\t\n"
"	.even\t\n"
);
#endif /* ERROR_CHECK */
#endif	!defined (__M68881__) && !defined (sfp004)

#ifdef	__M68881__

__asm(
"	.text\t\n"
"	.even\t\n"
"	.globl " C_SYMBOL_NAME(cexp) "\t\n"
C_SYMBOL_NAME(cexp) ":\t\n"
"	fmovex	%fp2,%sp@-	| 12 Bytes\t\n"
"	movel	%a1,%d0		| save a1 as return value\t\n"
"	fetoxd	%a7@(16),%fp0	| exp( z.real )\t\n"
"	fmoved	%a7@(24),%fp2\t\n"
"	fcosx	%fp2,%fp1\t\n"
"	fsinx	%fp2,%fp2\t\n"
"	fmulx	%fp0,%fp1		|\t\n"
"	fmulx	%fp0,%fp2		|\t\n"
"	fmoved	%fp1,%a1@		| fetch result.real\t\n"
"	fmoved	%fp2,%a1@(8)	| fetch result.imag\t\n"
"	fmovex	%sp@+,%fp2\t\n"
);	/* end asm	*/
#endif /* __M68881__ */

#ifdef	sfp004
__asm(
"| double precision floating point stuff for Atari-gcc using the SFP004\t\n"
"| developed with gas\t\n"
"|\t\n"
"| double precision complex exponential function\t\n"
"|\t\n"
"| M. Ritzert (mjr at dmzrzu71)\t\n"
"|\t\n"
"| 12.10.1990\t\n"
"|\t\n"
"| addresses of the 68881 data port. This choice is fastest when much data is\t\n"
"| transferred between the two processors.\t\n"
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"
"\t\n"
"| waiting loop ...\t\n"
"|\t\n"
"| wait:\t\n"
"| ww:	cmpiw	#0x8900,%a1@(resp)\t\n"
"| 	beq	ww\t\n"
"| is coded directly by\t\n"
"|	.long	0x0c688900, 0xfff067f8\t\n"
"| and\t\n"
"| www:	tst.b	%a1@(resp)\t\n"
"|	bmi.b	www\t\n"
"| is coded by\t\n"
"|	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"\t\n"
"	.text; .even\t\n"
"	.globl " C_SYMBOL_NAME(cexp) "\t\n"
C_SYMBOL_NAME(cexp) ":\t\n"
"	movel	%a1,%d0				| save a1 as return value\t\n"
"	lea	0xfffa50,%a0			| fpu address\t\n"
"	movew	#0x5410,%a0@(comm)		| exp()\t\n"
"	cmpiw	#0x8900,%a0@(resp)		| check\t\n"
"	movel	%a7@(4),%a0@			| load arg_hi\t\n"
"	movel	%a7@(8),%a0@			| load arg_low\t\n"
"\t\n"
"|	movew	#%0101 0101 0011 0001,%a0@(comm)	| sincos: sin -> fp2\t\n"
"	movew	#0x5531,%a0@(comm)		| sincos: sin -> fp2\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(12),%a0@			| load arg_hi\t\n"
"	movel	%a7@(16),%a0@			| load arg_low\t\n"
"\t\n"
"|	movew	#%0000 0000 1010 0011,%a0@(comm)	| mul fp0 -> fp1\t\n"
"	movew	#0x00a3,%a0@(comm)		| mul fp0 -> fp1\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"\t\n"
"|	movew	#%0000 0001 0010 0011,%a0@(comm)	| mul fp0 -> fp2\t\n"
"	movew	#0x0123,%a0@(comm)		| mul fp0 -> fp2\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"\t\n"
"|	movew	#%0111 0100 1000 0000,%a0@(comm)	| fetch fp1\t\n"
"	movew	#0x7480,%a0@(comm)		|\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@(zahl),%a1@\t\n"
"	movel	%a0@(zahl),%a1@(4)\t\n"
"\t\n"
"|	movew	#%0111 0101 0000 0000,%a0@(comm)	| fetch fp2\t\n"
"	movew	#0x7500,%a0@(comm)		| \t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@(zahl),%a1@(8)\t\n"
"	movel	%a0@(zahl),%a1@(12)\t\n"
);	/* end asm	*/
#endif /* sfp004 */


#if defined (__M68881__) || defined (sfp004)
# ifdef ERROR_CHECK	/* no error checking for now	*/
__asm(
"	pea	_funcname\t\n"
"	jmp	c_err_check\t\n"
);	/* end asm	*/
# else /* ERROR_CHECK */

__asm("rts");

# endif /* ERROR_CHECK */
#endif defined (__M68881__) || defined (sfp004)
