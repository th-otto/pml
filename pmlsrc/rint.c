#if !defined (__M68881__) && !defined (sfp004)

/*
 * PDC I/O Library Copyright (C) 1987 by J.A. Lydiatt.
 * Modifications for PDC release 3.3 Copyright (C) 1988 Lionel D. Hummel.
 * PDC Software Distribution Copyright (C) 1988 Lionel Hummel and Paul Petersen.
 *
 * This code is freely redistributable upon the conditions that this notice
 * remains intact and that modified versions of this file not be included
 * as part of the PDC Software Distribution without the express consent of
 * the copyright holders.
 */

#include <math.h>

/*    round.c - performs rounding
 */

/* Returns an integer rounded from a double.  In case of fraction exactly
 * midway, round chooses nearest even value.
 */

double rint(num)
double num;
{
    double ipart;
    int    ival;
    double frac;

    frac = modf(num, &ipart);

    ival = ipart;
    if ( (frac > 0.5) || ((frac == 0.5) && (ival%2)) )
        ival++;

    return((double) ival);
}
#endif	/* __M68881__, sfp004	*/


#ifdef __M68881__

double rint(double x)
{
  double value;
#if 0
  int rounding_mode, round_nearest;

  __asm volatile ("fmove%.l fpcr,%0"
		  : "=dm" (rounding_mode)
		  : /* no inputs */ );
  round_nearest = rounding_mode & ~0x30;
  __asm volatile ("fmove%.l %0,fpcr"
		  : /* no outputs */
		  : "dmi" (round_nearest));
#endif
  __asm volatile ("fint%.x %1,%0"
		  : "=f" (value)
		  : "f" (x));
#if 0
  __asm volatile ("fmove%.l %0,fpcr"
		  : /* no outputs */
		  : "dmi" (rounding_mode));
#endif
  return value;
}
#endif /* __M68881__ */
#ifdef	sfp004
__asm(
"| pml compatible lib for the atari sfp004\t\n"
"|\t\n"
"| Michael Ritzert, Oktober 1990\t\n"
"| ritzert@dfg.dbp.de\t\n"
"|\t\n"
"| FUNCTION:	DOUBLE RINT( DOUBLE X )\t\n"
"|\t\n"
"| base =	0xfffa50\t\n"
"| the fpu addresses are taken relativ to 'base':\t\n"
"\t\n"
"comm =	 -6\t\n"
"resp =	-16\t\n"
"zahl =	  0\t\n"
"\t\n"
".text\t\n"
"	.globl _rint\t\n"
".even\t\n"
"_rint:\t\n"
"	lea	0xfffa50,a0\t\n"
"|	movew	#0x5403,a0@(comm)	| fintrz to fp0\t\n"
"	movew	#0x5401,a0@(comm)	| fint   to fp0\t\n"
"	cmpiw	#0x8900,a0@(resp)	| check\t\n"
"	movel	a7@(4),a0@		| load arg_hi\t\n"
"	movel	a7@(8),a0@		| load arg_low\t\n"
"	movew	#0x7400,a0@(comm)	| result to d0\t\n"
"| wait\t\n"
"	.long	0x0c688900, 0xfff067f8\t\n"
"	movel	a0@,d0\t\n"
"	movel	a0@,d1\t\n"
" 	rts\t\n"
"\t\n"
);
#endif /* sfp004 */
