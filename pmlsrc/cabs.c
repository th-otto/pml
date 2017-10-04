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
 *	cabs   double precision complex absolute value
 *
 *  KEY WORDS
 *
 *	cabs
 *	complex functions
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Computes double precision absolute value of a double
 *	precision complex argument, where "absolute value"
 *	is taken to mean magnitude.  The result replaces the
 *	argument.
 *
 *  USAGE
 *
 *	double cabs (z)
 *	COMPLEX z;
 *
 *  PROGRAMMER
 *
 *	Fred Fish
 *	Tempe, Az
 *
 *  INTERNALS
 *
 *	Computes cabs (z) where z = (x) + j(y) from:
 *
 *		cabs (z) = sqrt (x*x + y*y)
 *
 */

#if !defined (__M68881__) && !defined (sfp004)

#include <stdio.h>
#include <math.h>
#include "pml.h"


double cabs (z)
COMPLEX z;
{
    return( sqrt ((z.real * z.real) + (z.imag * z.imag)) );
}

#else /* defined (__M68881__) || defined (sfp004) */
    __asm(".text; .even");

# ifdef	ERROR_CHECK

    __asm(
"\t\n"
".even\t\n"
"| m.ritzert 7.12.1991\t\n"
"| ritzert@dfg.dbp.de\t\n"
"|\t\n"
"|    /* NAN  = {7fffffff,ffffffff}		*/\t\n"
"|    /* +Inf = {7ff00000,00000000}		*/\t\n"
"|    /* -Inf = {fff00000,00000000}		*/\t\n"
"|    /* MAX_D= {7fee42d1,30773b76}		*/\t\n"
"|    /* MIN_D= {ffee42d1,30773b76}		*/\t\n"
"\t\n"
".even\t\n"
"double_max:\t\n"
"	.long	0x7fee42d1\t\n"
"	.long	0x30273b76\t\n"
"double_min:\t\n"
"	.long	0xffee42d1\t\n"
"	.long	0x30273b76\t\n"
"NaN:\t\n"
"	.long	0x7fffffff\t\n"
"	.long	0xffffffff\t\n"
"p_Inf:\t\n"
"	.long	0x7ff00000\t\n"
"	.long	0x00000000\t\n"
"m_Inf:\t\n"
"	.long	0xfff00000\t\n"
"	.long	0x00000000\t\n"
);
# endif	/* ERROR_CHECK */
#endif	/* __M68881, sfp004	*/

#ifdef	__M68881__
__asm(
".even\t\n"
".globl _cabs\t\n"
"_cabs:\t\n"
"	fmoved	%a7@(4),%fp0	|\t\n"
"	fmulx	%fp0,%fp0		| x**2\t\n"
"	fmoved	%a7@(12),%fp1	|\t\n"
"	fmulx	%fp1,%fp1		| y**2\t\n"
"	faddx	%fp1,%fp0		|\t\n"
"	fsqrtx	%fp0,%fp0		| sqrt( x**2 + y**2 )\t\n"
"	fmoved	%fp0,%a7@-	|\t\n"
"	moveml	%a7@+,%d0-%d1	| return arg\t\n"
);
#endif /* __M68881__ */

#ifdef	sfp004
__asm(
"| pml compatible lib for the atari sfp004\t\n"
"|\t\n"
"| Michael Ritzert, Oktober 1990\t\n"
"| ritzert@dfg.dbp.de\t\n"
"|\t\n"
"| FUNCTION:	CABS(COMPLEX X)\t\n"
"|\t\n"
"| base =	0xfffa50\t\n"
"|      the fpu addresses are taken relativ to 'base':\t\n"
"|\t\n"
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
"|	word	0x4a68,0xfff0,0x6bfa		| test\t\n"
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"
"\t\n"
".globl _cabs\t\n"
".text\t\n"
".even\t\n"
"_cabs:\t\n"
"	lea	0xfffa50,%a0\t\n"
"\t\n"
"	movew	#0x5400,%a0@(comm)	| load %fp0\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(4),%a0@		| load arg_hi\t\n"
"	movel	%a7@(8),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x5480,%a0@(comm)	| load %fp1\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(12),%a0@		| load arg_hi\t\n"
"	movel	%a7@(16),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x0023,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"	movew	#0x04a3,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"	movew	#0x0422,%a0@(comm)	| fp0 = fp0 + fp1	\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"	movew	#0x0004,%a0@(comm)	| sqrt(fp0)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa	| test\t\n"
"\t\n"
"	movew	#0x7400,%a0@(comm)	| result to d0/d1\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@(zahl),%d0\t\n"
"	movel	%a0@(zahl),%d1\t\n"
);	/* end asm	*/
#endif /* sfp004 */	

#if defined (__M68881__) || defined (sfp004)
# ifdef ERROR_CHECK
__asm(
"	lea double_max,%a0	|\t\n"
"	swap	%d0		| exponent into lower word\t\n"
"	cmpw	%a0@(16),%d0	| == NaN ?\t\n"
"	beq	error_nan	|\t\n"
"	cmpw	%a0@(24),%d0	| == + Infinity ?\t\n"
"	beq	error_plus	|\t\n"
"	swap	%d0		| result ok,\t\n"
"	rts			| restore d0\t\n"
);
#ifndef	__MSHORT__
__asm(
"error_plus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#63,_errno	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	%a0@(24),%d0-%d1	| result = +inf\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#62,_errno	| NAN => errno = EDOM\t\n"
);
#else	/* __MSHORT__ */
__asm(
"error_plus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movew	#63,_errno	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	%a0@(24),%d0-%d1	| result = +inf\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movew	#62,_errno	| NAN => errno = EDOM\t\n"
);
#endif	/* __MSHORT__ */
__asm(
"error_exit:\t\n"
"	moveml	%a7@+,%d0-%d1\t\n"
"	rts\t\n"
);	/* end asm	*/
# else	/* ERROR_CHECK */
__asm("rts");
# endif	/* ERROR_CHECK */
#endif /* defined (__M68881__) || defined (sfp004) */
