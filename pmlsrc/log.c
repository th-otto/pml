/************************************************************************
 *                                                                      *
 * A replacement log() routine for PML library.  It really computes     *
 * base 2 logarithm which can be multiplied by a proper constant        *
 * to provide final answer.  A rational approximation of an original    *
 * is replaced by a polynomial one.  In practice, with a software       *
 * floating point, this gives a better precision.                       *
 *                                                                      *
 * Michal Jaegermann, December 1990                                     *
 *                                                                      *
 * If __GCC_HACK__ is defined then we are folding log and log10 routines*
 * by making in assembler an extra entry point.  Do not define that     *
 * for portable routine!!                                               *
 * Do not define that with gcc -O2 !!					*
 *                                                                      *
 * 68881 support added by Michael Ritzert, November 1991		*
 ************************************************************************
 */

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
 *	log   double precision natural log
 *
 *  KEY WORDS
 *
 *	log
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision natural log of double precision
 *	floating point argument.
 *
 *  USAGE
 *
 *	double log (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *	1968, pp. 105-111.
 *
 *  RESTRICTIONS
 *
 *	The absolute error in the approximating rational function is
 *	10**(-19.38).  Note that contrary to DEC's assertion
 *	in the F4P user's guide, the error is absolute and not
 *	relative.
 *      ** modified ** theoretical precision is only 10**(-16.5);
 *      it works better in practice.
 *
 *	This error bound assumes exact arithmetic
 *	in the polynomial evaluation.  Additional rounding and
 *	truncation errors may occur as the argument is reduced
 *	to the range over which the polynomial approximation
 *	is valid, and as the polynomial is evaluated using
 *	finite-precision arithmetic.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *      Modifications - Michal Jaegermann
 *
 *  INTERNALS
 *
 *	Computes log(X) from:
 *
 *	  (1)	If argument is zero then flag an error
 *		and return minus infinity (or rather its
 *		machine representation).
 *
 *	  (2)	If argument is negative then flag an
 *		error and return minus infinity.
 *
 *	  (3)	Given that x = m * 2**k then extract
 *		mantissa m and exponent k.
 *        (3a)  If m = 0.5 then we know what is the final
 *              result and calculations can be shortened.
 *
 *	  (4)	Transform m which is in range [0.5,1.0]
 *		to range [1/sqrt(2), sqrt(2)] by:
 *
 *			s = m * sqrt(2)
 *
 *	  (5)	Compute z = (s - 1) / (s + 1)
 *        (5a)  Modified - combine steps (4) and (5) computing
 *              z = (m - 1/sqrt(2)) / (m + 1/sqrt(2))
 *
 *	  (6)	Now use the approximation from HART
 *		page 111 to find log(s):
 *
 *		log(s) = z * ( P(z**2) / Q(z**2) )
 *
 *		Where:
 *
 *		P(z**2) =  SUM [ Pj * (z**(2*j)) ]
 *		over j = {0,1,2,3}
 *
 *		Q(z**2) =  SUM [ Qj * (z**(2*j)) ]
 *		over j = {0,1,2,3}
 *
 *		P0 =  -0.240139179559210509868484e2
 *		P1 =  0.30957292821537650062264e2
 *		P2 =  -0.96376909336868659324e1
 *		P3 =  0.4210873712179797145
 *		Q0 =  -0.120069589779605254717525e2
 *		Q1 =  0.19480966070088973051623e2
 *		Q2 =  -0.89111090279378312337e1
 *		Q3 =  1.0000
 *
 *		(coefficients from HART table #2705 pg 229)
 *	  (6a)	Modified - compute, as a polynomial of z, an
 *              approximation of log2(m * sqrt(2)). Coefficients
 *              for this polynomial are not given explicitely in HART
 *              but can be computed from table #2665, for example.
 *
 *	  (7)	Finally, compute log(x) from:
 *
 *		log(x) = (k * log(2)) - log(sqrt(2)) + log(s)
 *
 *        (7a)  log2(x) = k - 1/2 + log(m * sqrt(2)).  Multiply
 *              by a constant to adjust a logarithm base.
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"


#if !defined (__M68881__) && !defined (sfp004)


			/* mjr++				*/
			/* use a different routine instead	*/
			/* see end of listing			*/

# define HALFSQRT2		0.70710678118654752440

static double log_coeffs[] = {
	2.88539008177793058026,
	0.9617966939212138784,
	0.577078018095541030,
	0.4121983030496934,
	0.32062199711811,
	0.2612917312170,
	0.24451102108
};

#ifdef __GCC_HACK__
static double log_of2[] = {
	LN2, 0.30102999566398119521 /* log10(2) */
};
#endif

static char funcname[] = "log";

#ifdef __GCC_HACK__
/*
 * This fake function header may change even from version to a version
 * of gcc compiler.  Ensure that first four assembler instructions in
 * log and log10 are the same!
 */
__asm__(
"	.text\t\n"
"	.even\t\n"
"	.globl " C_SYMBOL_NAME(log10) "\t\n"
C_SYMBOL_NAME(log10) ":");
#ifdef __MSHORT__
__asm__(
"	link %a6,#-32\t\n"
"	moveml #0x3e30,%sp@-");
#else
__asm__(
"	link %a6,#-36\t\n"
"	moveml #0x3f30,%sp@-");
#endif  /* __MSHORT__ */
__asm__(
"	movel %a6@(8),%d4\t\n"
"	movel %a6@(12),%d5\t\n"
"	moveq #1,%d6\t\n"
"	jra   lgentry");
#endif  /* __GCC_HACK__ */

double log (x)
double x;
{
    int k;
    double s;
    struct exception xcpt;
    char * xcptstr;
#ifdef __GCC_HACK__
    int index;

    index = 0;
__asm__(
"lgentry:");
#endif

    if (x <= 0.0) {
	xcpt.name = funcname;
	xcpt.arg1 = x;
    	if (x == 0.0) {
	    xcpt.retval = -MAXDOUBLE;
	    xcpt.type = SING;
	    xcptstr = "SINGULARITY";
	}
	else {
	    xcpt.retval = MAXDOUBLE;
	    xcpt.type = DOMAIN;
	    xcptstr = "DOMAIN";
	}
	if (!matherr (&xcpt)) {
	    errno = EDOM;
	}
    }
    else {
	s = frexp (x, &k);
	if (0.5 == s ) {
		s = -1.0;
	}
	else {
	    /* range reduction with s multiplied by SQRT2 */
	    s = (s - HALFSQRT2) / (s + HALFSQRT2);
	    /* polynomial approximation */
	    s *= poly ((sizeof(log_coeffs)/sizeof(double)) - 1,
	               log_coeffs, s * s);
	    /* and subtract log2 of SQRT2 */
	    s -= 0.5;
	}
	/* add the original binary exponent */
	s += k;
	/* multiply to get a requested logarithm */
#ifdef __GCC_HACK__
	xcpt.retval = s * log_of2[index];
#else
	xcpt.retval = s * LN2;
#endif
    }
    return (xcpt.retval);
}
#endif /* !__M68881__ && ! sfp004	*/

/*****************************************************************/

#ifdef	sfp004

__asm(
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"
"\t\n"
);	/* end asm	*/

#endif /* sfp004 */
#if defined (__M68881__) || defined (sfp004)
    __asm(".text; .even");

# ifdef	ERROR_CHECK

    __asm(
"\t\n"
".even\t\n"
"\t\n"
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
);
# endif	/* ERROR_CHECK */

    __asm(
".even\t\n"
"	.globl " C_SYMBOL_NAME(log) "\t\n"
C_SYMBOL_NAME(log) ":\t\n"
    );	/* end asm	*/

#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	flognd	%a7@(4), %fp0	| log\t\n"
"	fmoved	%fp0,%a7@-	| push result\t\n"
"	moveml	%a7@+,%d0-%d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,%a0\t\n"
"	movew	#0x5414,%a0@(comm)	| specify function\t\n"
"	cmpiw	#0x8900,%a0@(resp)	| check\t\n"
"	movel	%a7@(4),%a0@		| load arg_hi\t\n"
"	movel	%a7@(8),%a0@		| load arg_low\t\n"
"	movew	#0x7400,%a0@(comm)	| result to %d0\t\n"
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
#else	__MSHORT__
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
