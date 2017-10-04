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
 *	asin   double precision arc sine
 *
 *  KEY WORDS
 *
 *	asin
 *	machine independent routines
 *	trigonometric functions
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision arc sine of double precision
 *	floating point argument.
 *
 *	If argument is less than -1.0 or greater than +1.0, calls
 *	matherr with a DOMAIN error.  If matherr does not handle
 *	the error then prints error message and returns 0.
 *
 *  USAGE
 *
 *	double asin (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	Fortran IV-plus user's guide, Digital Equipment Corp. pp B-2.
 *
 *  RESTRICTIONS
 *
 *	For precision information refer to documentation of the floating
 *	point library primatives called.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *
 *  INTERNALS
 *
 *	Computes arcsine(x) from:
 *
 *		(1)	If x < -1.0 or x > +1.0 then
 *			call matherr and return 0.0 by default.
 *
 *		(2)	If x = 0.0 then asin(x) = 0.0
 *
 *		(3)	If x = 1.0 then asin(x) = PI/2.
 *
 *		(4)	If x = -1.0 then asin(x) = -PI/2
 *
 *		(5)	If -1.0 < x < 1.0 then
 *			asin(x) = atan(y)
 *			y = x / sqrt[1-(x**2)]
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)	/* mjr++	*/

static char funcname[] = "asin";

    struct exception xcpt;

double asin (x)
double x;
{

    if ( x > 1.0 || x < -1.0) {
	xcpt.type = DOMAIN;
	xcpt.name = funcname;
	xcpt.arg1 = x;
	if (!matherr (&xcpt)) {
	    errno = EDOM;
	    xcpt.retval = HUGE_VAL;
	}
    } else if (x == 0.0) {
	xcpt.retval = 0.0;
    } else if (x == 1.0) {
	xcpt.retval = HALFPI;
    } else if (x == -1.0) {
	xcpt.retval = -HALFPI;
    } else {
	xcpt.retval = atan ( x / sqrt (1.0 - (x * x)) );
    }
    return (xcpt.retval);
}
#endif /* !defined (__M68881__) && !defined (sfp004) */
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

# ifdef ERROR_CHECK

    __asm(
"\t\n"
".even\t\n"
"\t\n"
"| pml compatible asingent\t\n"
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
"\t\n"
);	/* END ASM	*/
# endif	/* ERROR_CHECK */
__asm(
".even\t\n"
"	.globl " C_SYMBOL_NAME(asin) "\t\n"
C_SYMBOL_NAME(asin) ":\t\n"
"\t\n"
);	/* end asm	*/
#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	fasind	%a7@(4), %fp0	| asin\t\n"
"	fmoved	%fp0,%a7@-	| push result\t\n"
"	moveml	%a7@+,%d0-%d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,%a0\t\n"
"	movew	#0x540c,%a0@(comm)	| specify function\t\n"
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
"error_minus:"
"	swap	%d0"
"	moveml	%d0-%d1,%a7@-"
"	movel	#63," C_SYMBOL_NAME(errno) "	| errno = ERANGE"
"	bra	error_exit	|"
"error_plus:"
"	swap	%d0"
"	moveml	%d0-%d1,%a7@-"
"	movel	#63," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM"
"	bra	error_exit	|"
"error_nan:"
"	moveml	%a0@(24),%d0-%d1	| result = +inf"
"	moveml	%d0-%d1,%a7@-"
"	movel	#62," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM"
);
#else	/* __MSHORT__ */
__asm(
"error_minus:"
"	swap	%d0"
"	moveml	%d0-%d1,%a7@-"
"	movew	#63," C_SYMBOL_NAME(errno) "	| errno = ERANGE"
"	bra	error_exit	|"
"error_plus:"
"	swap	%d0"
"	moveml	%d0-%d1,%a7@-"
"	movew	#63," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM"
"	bra	error_exit	|"
"error_nan:"
"	moveml	%a0@(24),%d0-%d1	| result = +inf"
"	moveml	%d0-%d1,%a7@-"
"	movew	#62," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM"
);
#endif	/* __MSHORT__ */
__asm(
"error_exit:\t\n"
"	moveml	%a7@+,%d0-%d1\t\n"
"	rts\t\n"
    );
# else	/* ERROR_CHECK */
__asm("rts");
# endif	/* ERROR_CHECK */

#endif /* __M68881__ || sfp004	*/
