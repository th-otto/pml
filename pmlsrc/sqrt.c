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
 *	sqrt   double precision square root
 *
 *  KEY WORDS
 *
 *	sqrt
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision square root of double precision
 *	floating point argument.
 *
 *  USAGE
 *
 *	double sqrt (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	Fortran IV-PLUS user's guide, Digital Equipment Corp. pp B-7.
 *
 *	Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *	1968, pp. 89-96.
 *
 *  RESTRICTIONS
 *
 *	The relative error is 10**(-30.1) after three applications
 *	of Heron's iteration for the square root.
 *
 *	However, this assumes exact arithmetic in the iterations
 *	and initial approximation.  Additional errors may occur
 *	due to truncation, rounding, or machine precision limits.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *
 *  INTERNALS
 *
 *	Computes square root by:
 *
 *	  (1)	Range reduction of argument to [0.5,1.0]
 *		by application of identity:
 *
 *		sqrt(x)  =  2**(k/2) * sqrt(x * 2**(-k))
 *
 *		k is the exponent when x is written as
 *		a mantissa times a power of 2 (m * 2**k).
 *		It is assumed that the mantissa is
 *		already normalized (0.5 =< m < 1.0).
 *
 *	  (2)	An approximation to sqrt(m) is obtained
 *		from:
 *
 *		u = sqrt(m) = (P0 + P1*m) / (Q0 + Q1*m)
 *
 *		P0 = 0.594604482
 *		P1 = 2.54164041
 *		Q0 = 2.13725758
 *		Q1 = 1.0
 *
 *		(coefficients from HART table #350 pg 193)
 *
 *	  (3)	Three applications of Heron's iteration are
 *		performed using:
 *
 *		y[n+1] = 0.5 * (y[n] + (m/y[n]))
 *
 *		where y[0] = u = approx sqrt(m)
 *
 *	  (4)	If the value of k was odd then y is either
 *		multiplied by the square root of two or
 *		divided by the square root of two for k positive
 *		or negative respectively.  This rescales y
 *		by multiplying by 2**frac(k/2).
 *
 *	  (5)	Finally, y is rescaled by int(k/2) which
 *		is equivalent to multiplication by 2**int(k/2).
 *
 *		The result of steps 4 and 5 is that the value
 *		of y between 0.5 and 1.0 has been rescaled by
 *		2**(k/2) which removes the original rescaling
 *		done prior to finding the mantissa square root.
 *
 *  NOTES
 *
 *	The Convergent Technologies compiler optimizes division
 *	by powers of two to "arithmetic shift right" instructions.
 *	This is ok for positive integers but gives different
 *	results than other C compilers for negative integers.
 *	For example, (-1)/2 is -1 on the CT box, 0 on every other
 *	machine I tried.
 *
 */

/*
 *  MODIFICATIONS
 *
 *	Function sqrt(x) is initially approximated on an
 *      interval [0.5..2.0] by a quadratic polynomial with
 *	the following coefficients
 *
 *		 P0  0.3608580479718948e+00
 *		 P1  0.7477707028388739e+00
 *		 P2 -0.1105464728191369e+00
 *
 *	These coefficients were computed with a help of Maple
 *	symbolic algebra system.  Longer approximation interval
 *	is used in order to avoid multiplication by SQRT2
 *	constant in case of odd exponents (this idea comes from
 *	Olaf Flebbe, flebbe@tat.physik.uni-tuebingen.de).  With
 *	64 bit IEEE representation three Heron's iterations are
 *	good enough to satisfy essential requirements of Paranoia
 *	test suite. An absolute error after three iterations is
 *	below 2e-19 over the whole range (below 5e-10 after two).
 *	Multiplications by powers of 2 are performed by explicit
 *	calls to ldexp.
 *
 *	Michal Jaegermann, 21 October 1992
 */

#if !defined (__M68881__) && !defined (sfp004)

#include <stdio.h>
#include <math.h>
#include "pml.h"

#define P0  0.3608580479718948e+00
#define P1  0.7477707028388739e+00
#define P2 -0.1105464728191369e+00

static char funcname[] = "sqrt";

double sqrt (x)
double x;
{
    int k;
    double m;
    double u;
    struct exception xcpt;
    register double (*dfunc)(double, int) = ldexp;

    if (x == 0.0) {
	xcpt.retval = 0.0;
    } else if (x < 0.0) {
	xcpt.type = DOMAIN;
	xcpt.name = funcname;
	xcpt.arg1 = x;
	if (!matherr (&xcpt)) {
	    errno = EDOM;
	    xcpt.retval = 0.0;
	}
    } else {
	m = frexp (x, &k);
	if (k & 1) {
	    m = dfunc(m, 1);
	    /*
	     * multiply by 2 if our exponent was odd;
	     * odd k is now too big by 1 but (k >> 1) will take
	     * care of that
	     */
	}
	u = (P2 * m + P1) * m + P0;	/* the first guess   */

	u = dfunc((u + (m / u)), -1);	/* Heron's iteration */
#ifndef __SOZOBON__
	/*
	 * with current floating point representation used
	 * by Sozobon C we are already below achievable precision
	 */
 	u = dfunc((u + (m / u)), -1);   /* one more          */
#endif /* __SOZOBON__ */
	/*
	 * here we rely on the fact that -3/2 and (-3 >> 1)
	 * do give different results;
	 * the last iteration and adjust final exponent properly
	 */
	xcpt.retval = dfunc (u + (m / u), (k >> 1) - 1);
    }
    return (xcpt.retval);
}

double hypot(double x, double y)
{
    return sqrt(x*x + y*y);
}
#endif	/*  __M68881__, sfp004	*/

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

    __asm(".even\t\n"
".globl _hypot\t\n"
".globl _sqrt\t\n"
"_sqrt:\t\n"
    );	/* end asm	*/

#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	fsqrtd	%a7@(4), %fp0	| sqrt\t\n"
"	fmoved	%fp0,%a7@-	| push result\t\n"
"	moveml	%a7@+,%d0-%d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,%a0\t\n"
"	movew	#0x5404,%a0@(comm)	| specify function\t\n"
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
"err:\t\n"
"	lea	double_max,%a0	|\t\n"
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
#else	__MSHORT__
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
);
# endif	/* ERROR_CHECK */
__asm(
"	rts\t\n"
"\t\n"
".even\t\n"
"_hypot:\t\n"
    );
#endif /* __M68881__ || sfp004	*/
#ifdef __M68881__
__asm(
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
"	lea	0xfffa50,%a0\t\n"
"\t\n"
"	movew	#0x5400,%a0@(comm)	| load fp0\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a7@(4),%a0@		| load arg_hi\t\n"
"	movel	%a7@(8),%a0@		| load arg_low\t\n"
"\t\n"
"	movew	#0x5480,%a0@(comm)	| load fp1\t\n"
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
);
#endif /* sfp004 */
#if ( defined (__M68881__) || defined (sfp004) ) && defined (ERROR_CHECK)

__asm("bra err");

#else /* ERROR_CHECK */

__asm("rts");

#endif /* ERROR_CHECK */
