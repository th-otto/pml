/************************************************************************
 *                                                                      *
 * A replacement exp() routine for PML library.  An original algorithm  *
 * is not changed but a table of fractionals power of 2 was recalcul-   *
 * ated (believe it or not - this has an influence on a final result).  *
 * Also divisions by power of 2 were replaced by applications of ldexp  *
 * routine which is faster and better preserves precision               *
 *                                                                      *
 * Michal Jaegermann, December 1990                                     *
 *                                                                      *
 ************************************************************************
 *                                                                      *
 * 68881 support added by Michael Ritzert				*
 *                                                                      *
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
 *	exp   double precision exponential
 *
 *  KEY WORDS
 *
 *	exp
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision exponential of double precision
 *	floating point number.
 *
 *  USAGE
 *
 *	double exp (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	Fortran IV plus user's guide, Digital Equipment Corp. pp B-3
 *
 *	Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *	1968, pp. 96-104.
 *
 *  RESTRICTIONS
 *
 *	Inputs greater than log(MAXDOUBLE) result in overflow.
 *	Inputs less than log(MINDOUBLE) result in underflow.
 *
 *	The maximum relative error for the approximating polynomial
 *	is 10**(-16.4).  However, this assumes exact arithmetic
 *	in the polynomial evaluation.  Additional rounding and
 *	truncation errors may occur as the argument is reduced
 *	to the range over which the polynomial approximation
 *	is valid, and as the polynomial is evaluated using
 *	finite precision arithmetic.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *
 *  INTERNALS
 *
 *	Computes exponential from:
 *
 *		exp(x)	=	2**y  *  2**z  *  2**w
 *
 *	Where:
 *
 *		y	=	int ( x * log2(e) )
 *
 *		v	=	16 * frac ( x * log2(e))
 *
 *		z	=	(1/16) * int (v)
 *
 *		w	=	(1/16) * frac (v)
 *
 *	Note that:
 *
 *		0 =< v < 16
 *
 *		z = {0, 1/16, 2/16, ...15/16}
 *
 *		0 =< w < 1/16
 *
 *	Then:
 *
 *		2**z is looked up in a table of 2**0, 2**1/16, ...
 *
 *		2**w is computed from an approximation:
 *
 *			2**w	=  (A + B) / (A - B)
 *
 *			A	=  C + (D * w * w)
 *
 *			B	=  w * (E + (F * w * w))
 *
 *			C	=  20.8137711965230361973
 *
 *			D	=  1.0
 *
 *			E	=  7.2135034108448192083
 *
 *			F	=  0.057761135831801928
 *
 *		Coefficients are from HART, table #1121, pg 206.
 *
 *		Effective multiplication by 2**y is done by a
 *		floating point scale with y as scale argument.
 *
 */

#if !defined (__M68881__) && !defined (sfp004)	/* mjr++		*/

#include <stdio.h>
#include <math.h>
#include "pml.h"

static char funcname[] = "exp";

# define C  20.8137711965230361973	/* Polynomial approx coeff.	*/
/* # define D  1.0	*/              /* Polynomial approx coeff.	*/
# define E  7.2135034108448192083	/* Polynomial approx coeff.	*/
# define F  0.057761135831801928	/* Polynomial approx coeff.	*/


/************************************************************************
 *									*
 *	This table is fixed in size and reasonably hardware		*
 *	independent.  The given constants were computed on Atari ST     *
 *      using integer arithmetic and decimal values for 2**(1/2),       *
 *      2**(1/4) and 2**(1/8) taken from Appendix C of HART et al.      *
 *									*
 ************************************************************************
 */

static double fpof2tbl[] = {
    1.0/*0000000000000000000*/,		/*	2 ** 0/16	*/
    1.04427378242741384032,		/*	2 ** 1/16	*/
    1.09050773266525765920,		/*	2 ** 2/16	*/
    1.13878863475669165369,		/*	2 ** 3/16	*/
    1.18920711500272106671,		/*	2 ** 4/16	*/
    1.24185781207348404858,		/*	2 ** 5/16	*/
    1.29683955465100966592,		/*	2 ** 6/16	*/
    1.35425554693689272828,		/*	2 ** 7/16	*/
    1.41421356237309504880,		/*	2 ** 8/16	*/
    1.47682614593949931138,		/*	2 ** 9/16	*/
    1.54221082540794082360,		/*	2 ** 10/16	*/
    1.61049033194925430816,		/*	2 ** 11/16	*/
    1.68179283050742908605,		/*	2 ** 12/16	*/
    1.75625216037329948310,		/*	2 ** 13/16	*/
    1.83400808640934246346,		/*	2 ** 14/16	*/
    1.91520656139714729384		/*	2 ** 15/16	*/
};

    char * xcptstr;
    struct exception xcpt;

double exp (x)
double x;
{
    register int index;
    int y;
    double w;
    double a;
    double b;
    double temp;

    if (x > LOGE_MAXDOUBLE) {
	xcpt.type = OVERFLOW;
	xcpt.retval = MAXDOUBLE;
	xcptstr = "OVER";
	goto eset;
    }
    if (x <= LOGE_MINDOUBLE) {
	xcpt.type = UNDERFLOW;
	xcpt.retval = 0.0;
	xcptstr = "UNDER";
eset:
	xcpt.name = funcname;
	xcpt.arg1 = x;
	if (!matherr (&xcpt)) {
            errno = ERANGE;
	}
    } else {
	x *= LOG2E;
	w = ldexp(modf(x,&a), 4);
	y = a;
	w = ldexp(modf(w, &temp), -4);
	index = temp;
	
	b = w * w;
	a = C + b;
	b = w * (E + (F * b));
	xcpt.retval = ldexp (((a + b) / (a - b)) *
	    (index < 0 ? ldexp(fpof2tbl[16 + index], -1) : fpof2tbl[index]), y);
    }
    return (xcpt.retval);
}
#endif	/* !__M68881__ && !sfp004	*/
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
"\t\n"
".even\t\n"
"\t\n"
"| pml compatible expgent\t\n"
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
#endif /* ERROR_CHECK */

__asm(
".even\t\n"
"	.globl _exp\t\n"
"_exp:\t\n"
);	/* end asm	*/

#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	fetoxd	a7@(4), fp0	| exp\t\n"
"	fmoved	fp0,a7@-	| push result\t\n"
"	moveml	a7@+,d0-d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,a0\t\n"
"	movew	#0x5410,a0@(comm)	| specify function\t\n"
"	cmpiw	#0x8900,a0@(resp)	| check\t\n"
"	movel	a7@(4),a0@		| load arg_hi\t\n"
"	movel	a7@(8),a0@		| load arg_low\t\n"
"	movew	#0x7400,a0@(comm)	| result to d0\t\n"
"	.long	0x0c688900, 0xfff067f8	| wait\t\n"
"	movel	a0@,d0\t\n"
"	movel	a0@,d1\t\n"
    );	/* end asm	*/

#endif /* sfp004 */
#if defined (__M68881__) || defined (sfp004)
# ifdef	ERROR_CHECK
    __asm(
"	lea	double_max,a0	|\t\n"
"	swap	d0		| exponent into lower word\t\n"
"	cmpw	a0@(16),d0	| == NaN ?\t\n"
"	beq	error_nan	|\t\n"
"	cmpw	a0@(24),d0	| == + Infinity ?\t\n"
"	beq	error_plus	|\t\n"
"	swap	d0		| result ok,\t\n"
"	rts			| restore d0\t\n"
);
#ifndef	__MSHORT__
__asm(
"error_plus:\t\n"
"	swap	d0\t\n"
"	moveml	d0-d1,a7@-\t\n"
"	movel	#63,_errno	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	a0@(24),d0-d1	| result = +inf\t\n"
"	moveml	d0-d1,a7@-\t\n"
"	movel	#62,_errno	| NAN => errno = EDOM\t\n"
);
#else	__MSHORT__
__asm(
"error_plus:\t\n"
"	swap	d0\t\n"
"	moveml	d0-d1,a7@-\t\n"
"	movew	#63,_errno	| NAN => errno = EDOM\t\n"
"	bra	error_exit	|\t\n"
"error_nan:\t\n"
"	moveml	a0@(24),d0-d1	| result = +inf\t\n"
"	moveml	d0-d1,a7@-\t\n"
"	movew	#62,_errno	| NAN => errno = EDOM\t\n"
);
#endif	__MSHORT__
__asm(
"error_exit:\t\n"
"	moveml	a7@+,d0-d1\t\n"
"	rts\t\n"
    );
# else	/* ERROR_CHECK */

__asm("rts");

# endif	/* ERROR_CHECK */
#endif /* __M68881__ || sfp004	*/
