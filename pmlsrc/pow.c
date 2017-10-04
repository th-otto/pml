/*
	computes x^y.
	uses log and exp
*/

#include	<errno.h>
#include	<math.h>

double 
pow(x,y)
double x, y;
{
	double temp;
	long l;

	if (y == 0.) {
		if (x == 0.) goto domain;
		return 1.0;
	}
	else if (x == 0.) {
		if (y > 0.) return 0.;
		else goto domain;
	}

	if(x < 0.)
	{
		l = y;			/* is y an integer? */
		if(l != y)
			goto domain;
		temp = exp(y * log(-x));
		if(l & 1)
			temp = -temp;
		return(temp);
	}

	return(exp(y * log(x)));

domain:
	errno = EDOM;
	return(HUGE_VAL);
}
