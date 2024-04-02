/** @file
 * @brief Helper macros header
 * @author 5cover
 */

#ifndef MACROS_H
#define MACROS_H

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

/// @brief Gets the necessary buffer size for a sprintf operation.
#define bufferSize(format, args) (vsnprintf(NULL, 0, (format), (args)) + 1) // safe byte for \0

/// @brief Gets the digit count of an unsigned integer @p n in base @p base.
#define digitCount(n, base) ((n) == 0 ? 1 : (int)(log(n) / log(base)) + 1)

#ifdef __GNUC__

#define max(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a > _b ? _a : _b;      \
    })

#define min(a, b)               \
    ({                          \
        __typeof__(a) _a = (a); \
        __typeof__(b) _b = (b); \
        _a < _b ? _a : _b;      \
    })

#else

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#endif // __GNUC__

#endif // MACROS_H
