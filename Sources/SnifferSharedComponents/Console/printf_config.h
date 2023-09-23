///////////////////////////////////////////////////////////////////////////////
// Config file for mpaland printf
///////////////////////////////////////////////////////////////////////////////

#ifndef _PRINTF_CONFIG_H
#define _PRINTF_CONFIG_H

// 'ntoa' conversion buffer size, this must be big enough to hold one converted
// numeric number including padded zeros (dynamically created on stack)
// default: 32 byte
//#define PRINTF_NTOA_BUFFER_SIZE    32U

// 'ftoa' conversion buffer size, this must be big enough to hold one converted
// float number including padded zeros (dynamically created on stack)
// default: 32 byte
//#define PRINTF_FTOA_BUFFER_SIZE    32U

// support for the floating point type (%f)
// default: activated
// define PRINTF_DISABLE_SUPPORT_FLOAT to disable
#define PRINTF_DISABLE_SUPPORT_FLOAT    1

// support for exponential floating point notation (%e/%g)
// default: activated
// define PRINTF_DISABLE_SUPPORT_EXPONENTIAL to disable
#define PRINTF_DISABLE_SUPPORT_EXPONENTIAL  1

// define the default floating point precision
// default: 6 digits
//#define PRINTF_DEFAULT_FLOAT_PRECISION  6U

// define the largest float suitable to print with %f
// default: 1e9
//#define PRINTF_MAX_FLOAT  1e9

// support for the long long types (%llu or %p)
// default: activated
// define PRINTF_DISABLE_SUPPORT_LONG_LONG to disable
//#define PRINTF_DISABLE_SUPPORT_LONG_LONG  1

// support for the ptrdiff_t type (%t)
// ptrdiff_t is normally defined in <stddef.h> as long or long long type
// default: activated
#define PRINTF_DISABLE_SUPPORT_PTRDIFF_T  1


// Implementation of _putchar for printf
void HAL_Console_Tx_Byte( uint8_t byte );
#define _putchar        HAL_Console_Tx_Byte

#endif  //_PRINTF_CONFIG_H