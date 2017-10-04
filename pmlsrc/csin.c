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
 *	csin   complex double precision sine
 *
 *  KEY WORDS
 *
 *	csin
 *	complex functions
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Computes double precision complex sine of a double
 *	precision complex argument.
 *
 *  USAGE
 *
 *	COMPLEX csin (z)
 *	COMPLEX z;
 *
 *  REFERENCES
 *
 *	Fortran 77 user's guide, Digital Equipment Corp. pp B-12
 *
 *  PROGRAMMER
 *
 *	Fred Fish
 *	Tempe, Az 85281
 *	(602) 966-8871
 *
 *  INTERNALS
 *
 *	Computes complex sine of z = x + j y from:
 *
 *		1.	r_csin = sin(x) * cosh(y)
 *
 *		2.	i_csin = cos(x) * sinh(y)
 *
 *		3.	csin(z) = r_csin + j i_csin
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)

COMPLEX csin (z)
COMPLEX z;
{
    COMPLEX result;

    result.real = sin (z.real) * cosh (z.imag);
    result.imag = cos (z.real) * sinh (z.imag);
    return (result);
}
#endif /* !defined (__M68881__) && !defined (sfp004) */

#if defined (__M68881__) || defined (sfp004)
# ifdef ERROR_CHECK	/* no error checking for now	*/
__asm(
".text\t\n"
"_funcname:\t\n"
"	.ascii	\"csin\\0\"\t\n"
"	.even\t\n"
);	/* end asm	*/
# endif /* ERROR_CHECK */
#endif /* defined (__M68881__) || defined (sfp004) */

#ifdef	__M68881__
__asm(
".text\t\n"
"	.globl " C_SYMBOL_NAME(csin) "\t\n"
C_SYMBOL_NAME(csin) ":\t\n"
"	movel	%a1,%d0		| save a1 as return value\t\n"
"	fmoved	%sp@(4),%fp0	| z.real\t\n"
"	fcosx	%fp0,%fp1		| cos(z.real)\t\n"
"	fsinx	%fp0,%fp0		| sin(z.real)\t\n"
"\t\n"
"	fmoved	%sp@(12),%fp3	| z.imag\t\n"
"	fcoshx	%fp3,%fp2		| cosh(z.imag)\t\n"
"	fsinhx	%fp3,%fp3		| sinh(z.imag)\t\n"
"	\t\n"
"	fmulx	%fp2,%fp0		| result.real\t\n"
"	fmoved	%fp0,%a1@		|\t\n"
"	fmulx	%fp3,%fp1		| result.imag\t\n"
"	fmoved	%fp1,%a1@(8)	|\t\n"
);	/* end asm	*/
#endif /* __M68881__ */

#ifdef	sfp004
__asm(
"| double precision floating point stuff for Atari-gcc using the SFP004\t\n"
"| developed with gas\t\n"
"|\t\n"
"| double precision complex sin\t\n"
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
"	.globl " C_SYMBOL_NAME(csin) "\t\n"
C_SYMBOL_NAME(csin) ":\t\n"
"	movel	%a1,%d0				| save a1 as return value\t\n"
"	lea	0xfffa50,%a0			| fpu address\t\n"
"\t\n"
"	movew	#0x5431,%a0@(comm)		| sincos: sin -> fp0 \t\n"
"	.long	0x0c688900, 0xfff067f8		|	  cos -> fp1\t\n"
"	movel	%sp@(4), %a0@			| load z.real\t\n"
"	movel	%sp@(8), %a0@			| load z.real\t\n"
"\t\n"
"|	fmoved	%sp@(12),%fp3			| z.imag to fp3\t\n"
"	movew	#0x5580,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%sp@(12),%a0@\t\n"
"	movel	%sp@(16),%a0@\t\n"
"\t\n"
"|	fcoshx	%fp3,%fp2				| cosh(z.imag)\t\n"
"	movew	#0x0d19,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"|	fsinhx	%fp3,%fp3				| sinh(z.imag)\t\n"
"	movew	#0x0d82,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"|	fmulx	%fp2,%fp0				| result.real\t\n"
"	movew	#0x0823,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"|	fmoved	%fp0,%a1@				|\t\n"
"	movew	#0x7400,%a0@(comm)		| \t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@,%a1@\t\n"
"	movel	%a0@,%a1@(4)\t\n"
"|	fmulx	%fp3,%fp1				| result.imag\t\n"
"	movew	#0x0ca3,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"|	fmoves	%fp1,%d1				|\t\n"
"	movew	#0x7480,%a0@(comm)		|\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@,%a1@(8)\t\n"
"	movel	%a0@,%a1@(12)\t\n"
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
