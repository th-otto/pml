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
 *	log10   double precision common log
 *
 *  KEY WORDS
 *
 *	log10
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision common log of double precision
 *	floating point argument.
 *
 *  USAGE
 *
 *	double log10 (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	PDP-11 Fortran IV-plus users guide, Digital Equip. Corp.,
 *	1975, pp. B-3.
 *
 *  RESTRICTIONS
 *
 *	For precision information refer to documentation of the
 *	floating point library routines called.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *
 *  INTERNALS
 *
 *	Computes log10(x) from:
 *
 *		log10(x) = log10(e) * log(x)
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)

#ifndef __GCC_HACK__	/* see log.c for __GCC_HACK__ */

double log10 (x)
double x;
{
    return ( LOG10E * log (x) );
}
#endif
#endif /* !__M68881__, !sfp004 */

#ifdef	sfp004

__asm(
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"

);	/* end asm	*/

#endif /* sfp004 */
#if defined (__M68881__) || defined (sfp004)

    __asm(".text; .even");

# ifdef	ERROR_CHECK

    __asm(
"\t\n"
".even\t\n"
"| pml compatible log\t\n"
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
    );	/* end asm	*/
# endif	/* ERROR_CHECK */

    __asm(
".even\t\n"
"	.globl " C_SYMBOL_NAME(log10) "\t\n"
C_SYMBOL_NAME(log10) ":\t\n"
    );	/* end asm	*/

#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	flog10d	%a7@(4), %fp0	| log\t\n"
"	fmoved	%fp0,%a7@-	| push result\t\n"
"	moveml	%a7@+,%d0-%d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,%a0\t\n"
"	movew	#0x5415,%a0@(comm)	| specify function\t\n"
"	cmpiw	#0x8900,%a0@(resp)	| check\t\n"
"	movel	%a7@(4),%a0@		| load arg_hi\t\n"
"	movel	%a7@(8),%a0@		| load arg_low\t\n"
"	movew	#0x7400,%a0@(comm)	| result to d0\t\n"
"	.long	0x0c688900, 0xfff067f8	| wait\t\n"
"	movel	%a0@,%d0\t\n"
"	movel	%a0@,%d1\t\n"
);	/* end asm	*/

#endif /* sfp004 */
#if defined (__M68881__) || defined (sfp004)
# ifdef	ERROR_CHECK
    __asm(
"	lea	double_max,%a0	|\t\n"
"	swap	%d0		| exponent into lower word\t\n"
"	cmpw	%a0@(16),%d0	| == NaN ?\t\n"
"	beq	error_nan	|\t\n"
"	cmpw	%a0@(24),%d0	| == + Infinity ?\t\n"
"	beq	error_plus	|\t\n"
"	cmpw	%a0@(32),%d0	| == - Infinity ?\t\n"
"	beq	error_minus	|\t\n"
"	swap	%d0		| result ok,\t\n"
"	rts			| restore d0\t\n"
);
#ifndef	__MSHORT__
__asm(
"error_minus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#63," C_SYMBOL_NAME(errno) "	| errno = ERANGE\t\n"
"	bra	error_exit	|\t\n"
"error_plus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#63," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	%a0@(24),%d0-%d1	| result = +inf\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#62," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM\t\n"
);
#else	/* __MSHORT__ */
__asm(
"error_minus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movew	#63," C_SYMBOL_NAME(errno) "	| errno = ERANGE\t\n"
"	bra	error_exit	|\t\n"
"error_plus:\t\n"
"	swap	%d0\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movew	#63," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	%a0@(24),%d0-%d1	| result = +inf\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movew	#62," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM\t\n"
);
#endif	__MSHORT__
__asm(
"error_exit:\t\n"
"	moveml	%a7@+,%d0-%d1\t\n"
"	rts\t\n"
    );
# else	/* ERROR_CHECK */

__asm("rts");

# endif	/* ERROR_CHECK */
#endif /* __M68881__ || sfp004	*/
