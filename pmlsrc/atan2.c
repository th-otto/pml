/************************************************************************
 *									*
 *   Replacement atan2() function for PML library. Original function    *
 *   from Fred Fish does not conform to Standard C.  It has arguments   *
 *   in a reverse order.  It also does not complain if both arguments   *
 *   are 0.0                                                            *
 *									*
 *   Michal Jaegermann - December 1990
 *									*
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
 *	atan2   double precision arc tangent of two arguments
 *
 *  KEY WORDS
 *
 *	atan2
 *	machine independent routines
 *	trigonometric functions
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Returns double precision arc tangent of two
 *	double precision floating point arguments ( atan(Y/X) ).
 *
 *  USAGE
 *
 *	double atan2(y, x)
 *	double y;
 *	double x;
 *
 *  (This order of arguments really makes sense if you will notice
 *   that this function is mainly used during a conversion from
 *   rectangular to polar coordinates)
 *
 *  REFERENCES
 *
 *	Fortran 77 user's guide, Digital Equipment Corp. pp B-4.
 *
 *  RESTRICTIONS
 *
 *	For precision information refer to documentation of the
 *	other floating point library routines called.
 *	
 *  PROGRAMMER
 *
 *	Fred Fish
 *	Modified by Michal Jaegermann
 *
 *  INTERNALS
 *
 *	Computes atan2(y, x) from:
 *
 *		1.	If x = 0 and y != 0 then
 *			atan2(y, x) = PI/2 * (sign(y))
 *                      otherwise error signaled and
 *                      atan2(y ,x) is 0.
 *
 *		2.	If x > 0 then
 *			atan2(y, x) = atan(y/x)
 *
 *		3.	If x < 0 then atan2(y, x) =
 *			PI*(sign(y)) + atan(y/x)
 *
 */

#include <stdio.h>
#include <math.h>
#include "pml.h"
#include "symbols.h"

#if !defined (__M68881__) && !defined (sfp004)

static char funcname[] = "atan2";

    struct exception xcpt;

double atan2 (y, x)
double y;
double x;
{
    double result;

    if (x == 0.0) {
	if (y == 0.0) {
	    result = HUGE_VAL;	/* mjr: for now, should be NAN	*/
	    xcpt.type = DOMAIN;
	    xcpt.name = funcname;
	    xcpt.arg1 = x;
	    xcpt.retval = result;
	    if (!matherr(&xcpt)) {
		errno = EDOM;
	    }
	} else {
	    result = copysign (HALFPI, y);
	}
    } else {
	result = atan (y/x);
	if (x < 0.0) {
	    result += copysign (PI, y);
	}
    }
    return (result);
}

#endif /* !defined (__M68881__) && !defined (sfp004) */
#ifdef	__M68881__
__asm(
".text\t\n"
".even\t\n"
"_funcname:\t\n"
"	.ascii	\"atan2\\0\"\t\n"
"	.even\t\n"
"\t\n"
".globl	" C_SYMBOL_NAME(atan2) "\t\n"
C_SYMBOL_NAME(atan2) ":\t\n"
"| denormalized numbers are treated as 0\t\n"
"	tstl	%sp@(12)\t\n"
"	beq	5f		| x == 0!\t\n"
"	blt	1f		| x < 0!\t\n"
"				| x > 0: return atan(y/x)\t\n"
"\t\n"
"	fmoved	%sp@(4),%fp0	| get y\t\n"
"	fdivd	%sp@(12),%fp0	| y/x\t\n"
"	fatanx	%fp0,%fp0		| atan(y/x)\t\n"
"	bra 3f			| return\t\n"
"1:				| x < 0\t\n"
"\t\n"
"	fmovecr	#0,%fp1		| get pi\t\n"
"	fmoved	%sp@(4),%fp0	| get y\t\n"
"	fdivd	%sp@(12),%fp0	| y/x\t\n"
"	fatanx	%fp0,%fp0		| atan(y/x)\t\n"
"	btst	#31,%sp@(4)	| sign(y)\t\n"
"	beq	2f		| positive!\t\n"
"\t\n"
"	fnegx	%fp1,%fp1		| transfer sign\t\n"
"2:	faddx	%fp1,%fp0		| sign(y)*pi + atan(y/x)\t\n"
"|	bra 3f			| return\t\n"
"3:\t\n"
"	fmoved	%fp0,%sp@-	| return result\t\n"
"	moveml	%sp@+,%d0/%d1\t\n"
"4:	\t\n"
"	rts			| sigh.\t\n"
"5:				| x == 0\t\n"
"	movel	#1073291771,%d0	| pi/2\t\n"
"	movel	#1413754136,%d1	|\t\n"
"\t\n"
"	tstl	%sp@(4)		| \t\n"
"	beq	6f		| NaN\t\n"
"	bge	4b		| exit\t\n"
"	bset	#31,%d0		| x < 0 : return -pi/2\t\n"
"	bra	4b\t\n"
"6:	movel	#-1,%d0		| NaN\t\n"
"	movel	#-1,%d1		|\t\n"
"	bra	4b\t\n"
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
_funcname:
"	.ascii	\"atan2\\0\"\t\n"
"	.even\t\n"
".text\t\n"
".even\t\n"
".globl	_atan2\t\n"
"_atan2:\t\n"
"| denormalized numbers are treated as 0\t\n"
"	lea	0xfffa50,%a0\t\n"
"	moveml	%a7@(12),%d0-%d1	|  x\t\n"
"	tstl	%d0\t\n"
"	beq	5f		| x == 0!\t\n"
"	blt	1f		| x < 0!\t\n"
"				| x > 0: return atan(y/x)\t\n"
"\t\n"
"|	fmoved	%sp@(4),%fp0	| get y\t\n"
"	movew	#0x5400,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%sp@(4),%a0@\t\n"
"	movel	%sp@(8),%a0@\t\n"
"\t\n"
"|	fdivd	%sp@(12),%fp0	| y/x\t\n"
"	movew	#0x5420,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%d0,%a0@\t\n"
"	movel	%d1,%a0@\t\n"
"\t\n"
"|	fatanx	%fp0,%fp0		| atan(y/x)\t\n"
"	movew	#0x000a,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa\t\n"
"\t\n"
"	bra 3f			| return\t\n"
"1:				| x < 0\t\n"
"\t\n"
"|	fmovecr	#0,%fp1		| get pi\t\n"
"	movew	#0x5c80,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"\t\n"
"|	fmoved	%sp@(4),%fp0	| get y\t\n"
"	movew	#0x5400,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%sp@(4),%a0@\t\n"
"	movel	%sp@(8),%a0@\t\n"
"\t\n"
"|	fdivd	%sp@(12),%fp0	| y/x\t\n"
"	movew	#0x5420,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%d0,%a0@\t\n"
"	movel	%d1,%a0@\t\n"
"\t\n"
"|	fatanx	%fp0,%fp0		| atan(y/x)\t\n"
"	movew	#0x000a,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa\t\n"
"\t\n"
"	btst	#31,%sp@(4)	| sign(y)\t\n"
"	beq	2f		| positive!\t\n"
"\t\n"
"|	fnegx	%fp1,%fp1		| transfer sign\t\n"
"	movew	#0x049a,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa\t\n"
"\t\n"
"2:|	faddx	%fp1,%fp0		| sign(y)*pi + atan(y/x)\t\n"
"	movew	#0x0422,%a0@(comm)\t\n"
"	.word	0x4a68,0xfff0,0x6bfa\t\n"
"\t\n"
"|	bra 3f			| return\t\n"
"3:\t\n"
"|	fmoved	%fp0,%d0-%d1	| return result\t\n"
"	movew	#0x7400,%a0@(comm)\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	%a0@,%d0\t\n"
"	movel	%a0@,%d1\t\n"
"\t\n"
"4:	\t\n"
"	rts			| sigh.\t\n"
"5:				| x == 0\t\n"
"	movel	#1073291771,%d0	| pi/2\t\n"
"	movel	#1413754136,%d1	|\t\n"
"\t\n"
"	tstl	%sp@(4)		| \t\n"
"	beq	6f		| NaN\t\n"
"	bge	4b		| exit\t\n"
"	bset	#31,%d0		| x < 0 : return -pi/2\t\n"
"	bra	4b\t\n"
"6:	movel	#-1,%d0		| NaN\t\n"
"	movel	#-1,%d1		|\t\n"
"	bra	4b\t\n"
);	/* end asm	*/
#endif /* sfp004 */
