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
 *	cmult   double precision complex multiplication
 *
 *  KEY WORDS
 *
 *	cmult
 *	complex functions
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Computes double precision complex result of multiplication of
 *	first double precision complex argument by second double
 *	precision complex argument.
 *
 *  USAGE
 *
 *	COMPLEX cmult (z1, z2)
 *	COMPLEX z1;
 *	COMPLEX z2;
 *
 *  PROGRAMMER
 *
 *	Fred Fish
 *	Tempe, Az 85281
 *	(602) 966-8871
 *
 *  INTERNALS
 *
 *	Computes cmult(z1,z2) from:
 *
 *		1.	Let z1 = a + j b
 *			Let z2 = c + j d
 *
 *		2.	r_cmult = (a*c - b*d)
 *			i_cmult = (a*d + c*b)
 *
 *		3.	Then cmult(z1,z2) = r_cmult + j i_cmult
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)

COMPLEX cmult (z1, z2)
COMPLEX z1;
COMPLEX z2;
{
    COMPLEX result;

    result.real = (z1.real * z2.real) - (z1.imag * z2.imag);
    result.imag = (z1.real * z2.imag) + (z2.real * z1.imag);

    return (result);
}
#endif /* !defined (__M68881__) && !defined (sfp004) */
#ifdef	__M68881__
__asm(
".text\t\n"
".even\t\n"
"_funcname:\t\n"
"	.ascii	\"cmult\\0\"\t\n"
"	.even\t\n"
"\t\n"
".globl	" C_SYMBOL_NAME(cmult) "\t\n"
C_SYMBOL_NAME(cmult) ":\t\n"
"	fmoved	%sp@(4),%fp0\t\n"
"	fmoved	%sp@(12),%fp1\t\n"
"	fmoved	%sp@(20),%fp2\t\n"
"	fmoved	%sp@(28),%fp3\t\n"
"	fmovex	%fp0,%fp4\t\n"
"	movel	%a1,%d0		| pointer to result\t\n"
"\t\n"
"	fmulx	%fp2,%fp4\t\n"
"	fmulx	%fp3,%fp0\t\n"
"	fmulx	%fp1,%fp2\t\n"
"	fmulx	%fp1,%fp3\t\n"
"\t\n"
"	fsubx	%fp3,%fp4\t\n"
"	faddx	%fp0,%fp2\t\n"
"\t\n"
"	fmoved	%fp4,%a1@\t\n"
"	fmoved	%fp2,%a1@(8)\t\n"
);	/* end asm	*/
#endif /* __M68881__ */

#ifdef	sfp004
__asm(
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"
"\t\n"
".even\t\n"
".text\t\n"
".even\t\n"
"_funcname:\t\n"
"	.ascii	\"cmult\\0\"\t\n"
"	.even\t\n"
".text\t\n"
".even\t\n"
".globl	" C_SYMBOL_NAME(cmult) "\t\n"
C_SYMBOL_NAME(cmult) ":\t\n"
"\t\n"
"	lea	0xfffa50,%a0\t\n"
"\t\n"
"	movew	#0x5400,%a0@(comm)	| z1.real -> fp0\t\n"
"	movel	%a1,%d0		| pointer to result\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(4),%a0@		| load arg_hi\t\n"
"	movel	%a7@(8),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x5480,%a0@(comm)	| z1.imag -> fp1\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(12),%a0@		| load arg_hi\t\n"
"	movel	%a7@(16),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x5500,%a0@(comm)	| z2.real -> fp2\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(20),%a0@		| load arg_hi\t\n"
"	movel	%a7@(24),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x5580,%a0@(comm)	| z2.imag -> fp3\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(28),%a0@		| load arg_hi\t\n"
"	movel	%a7@(32),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x0200,%a0@(comm)	| copy fp0 to fp4\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"|	fmulx	%fp2,%fp4\t\n"
"	movew	#0x0a23,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"|	fmulx	%fp3,%fp0\t\n"
"	movew	#0x0c23,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"|	fmulx	%fp1,%fp2\t\n"
"	movew	#0x0523,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"|	fmulx	%fp1,%fp3\t\n"
"	movew	#0x05a3,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"|	fsubx	%fp3,%fp4\t\n"
"	movew	#0x0e28,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"|	faddx	%fp0,%fp2\t\n"
"	movew	#0x0122,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"|	fmoved	%fp4,%a1@\t\n"
"	movew	#0x7600,%a0@(comm)		| \t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@,%a1@\t\n"
"	movel	%a0@,%a1@(4)\t\n"
"\t\n"
"|	fmoved	%fp2,%d1\t\n"
"	movew	#0x7500,%a0@(comm)		| \t\n"
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
