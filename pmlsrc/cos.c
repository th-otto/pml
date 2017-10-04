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
 *	cos   double precision cosine
 *
 *  KEY WORDS
 *
 *	cos
 *	machine independent routines
 *	trigonometric functions
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision cosine of double precision
 *	floating point argument.
 *
 *  USAGE
 *
 *	double cos (x)
 *	double x;
 *
 *  REFERENCES
 *
 *	Computer Approximations, J.F. Hart et al, John Wiley & Sons,
 *	1968, pp. 112-120.
 *
 *  RESTRICTIONS
 *
 *	The sin and cos routines are interactive in the sense that
 *	in the process of reducing the argument to the range -PI/4
 *	to PI/4, each may call the other.  Ultimately one or the
 *	other uses a polynomial approximation on the reduced
 *	argument.  The sin approximation has a maximum relative error
 *	of 10**(-17.59) and the cos approximation has a maximum
 *	relative error of 10**(-16.18).
 *
 *	These error bounds assume exact arithmetic
 *	in the polynomial evaluation.  Additional rounding and
 *	truncation errors may occur as the argument is reduced
 *	to the range over which the polynomial approximation
 *	is valid, and as the polynomial is evaluated using
 *	finite-precision arithmetic.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *
 *	68881 support included by Michael Ritzert
 *
 *  INTERNALS
 *
 *	Computes cos(x) from:
 *
 *		(1)	Reduce argument x to range -PI to PI.
 *
 *		(2)	If x > PI/2 then call cos recursively
 *			using relation cos(x) = -cos(x - PI).
 *
 *		(3)	If x < -PI/2 then call cos recursively
 *			using relation cos(x) = -cos(x + PI).
 *
 *		(4)	If x > PI/4 then call sin using
 *			relation cos(x) = sin(PI/2 - x).
 *
 *		(5)	If x < -PI/4 then call cos using
 *			relation cos(x) = sin(PI/2 + x).
 *
 *		(6)	If x would cause underflow in approx
 *			evaluation arithmetic then return
 *			sqrt(1.0 - x**2).
 *
 *		(7)	By now x has been reduced to range
 *			-PI/4 to PI/4 and the approximation
 *			from HART pg. 119 can be used:
 *
 *			cos(x) = ( p(y) / q(y) )
 *			Where:
 *
 *			y    =	x * (4/PI)
 *
 *			p(y) =  SUM [ Pj * (y**(2*j)) ]
 *			over j = {0,1,2,3}
 *
 *			q(y) =  SUM [ Qj * (y**(2*j)) ]
 *			over j = {0,1,2,3}
 *
 *			P0   =	0.12905394659037374438571854e+7
 *			P1   =	-0.3745670391572320471032359e+6
 *			P2   =	0.134323009865390842853673e+5
 *			P3   =	-0.112314508233409330923e+3
 *			Q0   =	0.12905394659037373590295914e+7
 *			Q1   =	0.234677731072458350524124e+5
 *			Q2   =	0.2096951819672630628621e+3
 *			Q3   =	1.0000...
 *			(coefficients from HART table #3843 pg 244)
 *
 *
 *	**** NOTE ****    The range reduction relations used in
 *	this routine depend on the final approximation being valid
 *	over the negative argument range in addition to the positive
 *	argument range.  The particular approximation chosen from
 *	HART satisfies this requirement, although not explicitly
 *	stated in the text.  This may not be true of other
 *	approximations given in the reference.
 *			
 */

# include <stdio.h>
# include <math.h>
# include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)	/* mjr++		*/

static char funcname[] = "cos";

static double cos_pcoeffs[] = {
    0.12905394659037374438e7,
   -0.37456703915723204710e6,
    0.13432300986539084285e5,
   -0.11231450823340933092e3
};

static double cos_qcoeffs[] = {
    0.12905394659037373590e7,
    0.23467773107245835052e5,
    0.20969518196726306286e3,
    1.0
};



    double y;
    double yt2;
    struct exception xcpt;

double cos (x)
double x;
{
    if (x < -PI || x > PI) {
	x = fmod (x, TWOPI);
        if (x > PI) {
	    x = x - TWOPI;
        } else if (x < -PI) {
	    x = x + TWOPI;
        }
    }
    if (x > HALFPI) {
	xcpt.retval = -(cos (x - PI));
    } else if (x < -HALFPI) {
	xcpt.retval = -(cos (x + PI));
    } else if (x > FOURTHPI) {
	xcpt.retval = sin (HALFPI - x);
    } else if (x < -FOURTHPI) {
	xcpt.retval = sin (HALFPI + x);
    } else if (x < X6_UNDERFLOWS && x > -X6_UNDERFLOWS) {
	xcpt.retval = sqrt (1.0 - (x * x));
    } else {
	y = x / FOURTHPI;
	yt2 = y * y;
	xcpt.retval = poly (3, cos_pcoeffs, yt2) / poly (3, cos_qcoeffs, yt2);
    }
    return (xcpt.retval);
}

#endif	/* !__M68881 && !sfp004	*/
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
".even\t\n"
"| pml compatible cosgent\t\n"
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
"	.globl " C_SYMBOL_NAME(cos) "\t\n"
C_SYMBOL_NAME(cos) ":\t\n"
);	/* end asm	*/

#endif	/* __M68881__ || sfp004	*/
#ifdef	__M68881__

    __asm(
"	fcosd	%a7@(4), %fp0	| cos\t\n"
"	fmoved	%fp0,%a7@-	| push result\t\n"
"	moveml	%a7@+,%d0-%d1	| return_value\t\n"
    );	/* end asm	*/

#endif /* __M68881__ */
#ifdef	sfp004
    __asm(
"	lea	0xfffa50,%a0\t\n"
"	movew	#0x541d,%a0@(comm)	| specify function\t\n"
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
"	swap	%d0		| result ok,\t\n"
"	rts			| restore d0\t\n"
);
#ifndef	__MSHORT__
__asm(
"error_nan:\t\n"
"	moveml	%a0@(24),%d0-%d1	| result = +inf\t\n"
"	moveml	%d0-%d1,%a7@-\t\n"
"	movel	#62," C_SYMBOL_NAME(errno) "	| NAN => errno = EDOM\t\n"
);
#else	__MSHORT__
__asm(
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
