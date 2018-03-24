#include <math.h>
#include "symbols.h"

#if defined(LONG_DOUBLE_VERSION)
#define DOUBLE long double
#define ROUND roundl
#define FLOOR floorl
#define CEIL ceill
#define L_(x) x##L
#elif defined(FLOAT_VERSION)
#define DOUBLE float
#define ROUND roundf
#define FLOOR floorf
#define CEIL ceilf
#define L_(x) x ## f
#else
#define DOUBLE double
#define ROUND round
#define FLOOR floor
#define CEIL ceil
#define L_(x) x
#endif


DOUBLE ROUND(DOUBLE x)
{
	DOUBLE y;

	if (x >= L_(0.0))
	{
		y = FLOOR(x);

		if (x - y >= L_(0.5))
			y += L_(1.0);
	} else
	{
		y = CEIL(x);

		if (y - x >= L_(0.5))
			y -= L_(1.0);
	}
	return y;
}
