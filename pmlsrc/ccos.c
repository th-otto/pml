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
 *	ccos   complex double precision cosine
 *
 *  KEY WORDS
 *
 *	ccos
 *	complex functions
 *	machine independent routines
 *	math libraries
 *
 *  DESCRIPTION
 *
 *	Computes double precision complex cosine of a double
 *	precision complex argument.
 *
 *  USAGE
 *
 *	COMPLEX ccos (z)
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
 *	Computes complex cosine of z = x + j y from:
 *
 *		1.	r_ccos = cos(x) * cosh(y)
 *
 *		2.	i_ccos = -sin(x) * sinh(y)
 *
 *		3.	ccos(z) = r_ccos + j i_ccos
 *
 */

#if !defined (__M68881__) && !defined (sfp004)

#include <stdio.h>
#include <math.h>
#include "pml.h"


COMPLEX ccos (z)
COMPLEX z;
{
    COMPLEX result;

    result.real = cos(z.real) * cosh(z.imag);
    result.imag = -sin(z.real) * sinh(z.imag);
    return (result);
}
#endif /* !defined (__M68881__) #endif !defined (__M68881__) && !defined (sfp004)#endif !defined (__M68881__) && !defined (sfp004) !defined (sfp004) */

#if defined (__M68881__) || defined (sfp004)
# ifdef ERROR_CHECK	/* no error checking for now	*/
__asm(
".text\t\n"
"_funcname:\t\n"
"	.ascii	\"ccos\\0\"\t\n"
"	.even\t\n"
);	/* end asm	*/
# endif /* ERROR_CHECK */
#endif defined (__M68881__) || defined (sfp004)

#ifdef	__M68881__
__asm(
".text\t\n"
"	.globl _ccos\t\n"
"_ccos:\t\n"
"	movel	a1,d0		| save a1 as return value\t\n"
"	fmoved	sp@(4),fp0	| z.real\t\n"
"	fsinx	fp0,fp1		| cos(z.real)\t\n"
"	fcosx	fp0,fp0		| sin(z.real)\t\n"
"	fnegx	fp1,fp1		|-sin(z.real)\t\n"
"\t\n"
"	fmoved	sp@(12),fp3	| z.imag\t\n"
"	fcoshx	fp3,fp2		| cosh(z.imag)\t\n"
"	fsinhx	fp3,fp3		| sinh(z.imag)\t\n"
"	\t\n"
"	fmulx	fp2,fp0		| result.real\t\n"
"	fmoved	fp0,a1@		|\t\n"
"	fmulx	fp3,fp1		| result.imag\t\n"
"	fmoved	fp1,a1@(8)	|\t\n"
);	/* end asm	*/
#endif /* __M68881__ */

#ifdef	sfp004
__asm("
| double precision floating point stuff for Atari-gcc using the SFP004
| developed with gas
|
| double precision complex sin
|
| M. Ritzert (mjr at dmzrzu71)
|
| 12.10.1990
|
| addresses of the 68881 data port. This choice is fastest when much data is
| transferred between the two processors.

comm =	 -6
resp =	-16
zahl =	  0

| waiting loop ...
|
| wait:
| ww:	cmpiw	#0x8900,a1@(resp)
| 	beq	ww
| is coded directly by
|	.long	0x0c688900, 0xfff067f8
| and
| www:	tst.b	a1@(resp)
|	bmi.b	www
| is coded by
|	.word	0x4a68,0xfff0,0x6bfa		| test

	.text; .even
	.globl _ccos
_ccos:
	movel	a1,d0				| save a1 as return value
	lea	0xfffa50,a0			| fpu address

	movew	#0x54b0,a0@(comm)		| sincos: sin -> fp1
	.long	0x0c688900, 0xfff067f8		|	  cos -> fp0
	movel	sp@(4), a0@			| load z.real
	movel	sp@(8), a0@			| load z.real

|	fmoved	sp@(12),fp3			| z.imag to fp3
	movew	#0x5580,a0@(comm)
	.long	0x0c688900, 0xfff067f8
	movel	sp@(12),a0@
	movel	sp@(16),a0@

|	fcoshx	fp3,fp2				| cosh(z.imag)
	movew	#0x0d19,a0@(comm)
	.word	0x4a68,0xfff0,0x6bfa		| test
|	fsinhx	fp3,fp3				| sinh(z.imag)
	movew	#0x0d82,a0@(comm)
	.word	0x4a68,0xfff0,0x6bfa		| test
|	fmulx	fp2,fp0				| result.real
	movew	#0x0823,a0@(comm)
	.word	0x4a68,0xfff0,0x6bfa		| test
|	fmoved	fp0,a1@				|
	movew	#0x7400,a0@(comm)		| 
	.long	0x0c688900, 0xfff067f8
	movel	a0@,a1@
	movel	a0@,a1@(4)
|	fmulx	fp3,fp1				| result.imag
	movew	#0x0ca3,a0@(comm)
	.word	0x4a68,0xfff0,0x6bfa		| test
|	fnegx	fp1,fp1
	movew	#0x049a,a0@(comm)
	.word	0x4a68,0xfff0,0x6bfa		| test
|	fmoves	fp1,d1				|
	movew	#0x7480,a0@(comm)		|
	.long	0x0c688900, 0xfff067f8
	movel	a0@,a1@(8)
	movel	a0@,a1@(12)
");	/* end asm	*/
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
