#APP

| mjr: complex error checking code
|    inputs:
|	a1:	pointer to double
|	%a7@(4):	pointer to funcname

#include "symbols.h"

.text
.even

.globl c_err_check
.even

c_err_check:
	movew	%a1@,%d1		| get z.real
	cmpiw	#0x7fff,%d1	| == NaN ?
	beq	error_nan	|
	cmpiw	#0x7ff0,%d1	| == + Infinity ?
	beq	error_plus	|
	cmpiw	#0xfff0,%d1	| == - Infinity ?
	beq	error_minus	|
continue:
	movew	%a1@(8),%d1	| get z.imag
	cmpiw	#0x7fff,%d1	| == NaN ?
	beq	error_nan_i	|
	cmpiw	#0x7ff0,%d1	| == + Infinity ?
	beq	error_plus_i	|
	cmpiw	#0xfff0,%d1	| == - Infinity ?
	beq	error_minus_i	|

	addqw	#4,%a7		| drop _funcname
	rts

#ifndef	__MSHORT__
error_minus:
	moveml	%d0/%a1,%a7@-
	movel	#63,C_SYMBOL_NAME(errno)	| errno = ERANGE
	bra	error_exit	|
error_plus:
	moveml	%d0/%a1,%a7@-
	movel	#63,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
	bra	error_exit	|
error_nan:
	moveml	%d0/%a1,%a7@-
	lea	C_SYMBOL_NAME(_infinitydf),%a0
	movel	%a0@,%a1@		| result = + Infinity (for now)
	movel	%a0@(4),%a1@(4)	| result = + Infinity (for now)
	movel	#62,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
#else	__MSHORT__
error_minus:
	moveml	%d0/%a1,%a7@-
	movew	#63,C_SYMBOL_NAME(errno)	| errno = ERANGE
	bra	error_exit	|
error_plus:
	moveml	%d0/%a1,%a7@-
	movew	#63,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
	bra	error_exit	|
error_nan:
	moveml	%d0/%a1,%a7@-
	lea	C_SYMBOL_NAME(_infinitydf),%a0
	movel	%a0@,%a1@		| result = + Infinity (for now)
	movel	%a0@(4),%a1@(4)	| result = + Infinity (for now)
	movew	#62,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
#endif	__MSHORT__
error_exit:
	moveml	%a7@+,%d0/%a1
	bra	continue

#ifndef __MSHORT__
error_minus_i:
	moveml	%d0/%a1,%a7@-
	movel	#63,C_SYMBOL_NAME(errno)	| errno = ERANGE
	bra	error_exit_i	|
error_plus_i:
	moveml	%d0/%a1,%a7@-
	movel	#63,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
	bra	error_exit_i	|
error_nan_i:
	moveml	%d0/%a1,%a7@-
	lea	C_SYMBOL_NAME(_infinitydf),%a0
	movel	%a0@,%a1@(8)	| result = + Infinity (for now)
	movel	%a0@(4),%a1@(12)	| result = + Infinity (for now)
	movel	#62,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
#else __MSHORT__
error_minus_i:
	moveml	%d0/%a1,%a7@-
	movew	#63,C_SYMBOL_NAME(errno)	| errno = ERANGE
	bra	error_exit_i	|
error_plus_i:
	moveml	%d0/%a1,%a7@-
	movew	#63,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
	bra	error_exit_i	|
error_nan_i:
	moveml	%d0/%a1,%a7@-
	lea	C_SYMBOL_NAME(_infinitydf),%a0
	movel	%a0@,%a1@(8)	| result = + Infinity (for now)
	movel	%a0@(4),%a1@(12)	| result = + Infinity (for now)
	movew	#62,C_SYMBOL_NAME(errno)	| NAN => errno = EDOM
#endif __MSHORT__

error_exit_i:
	moveml	%a7@+,%d0/%a1

	rts
