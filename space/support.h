#ifndef HARNESS_SPACE_SUPPORT_H
#define HARNESS_SPACE_SUPPORT_H

#include "vex.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * Header-only cold support. Constructor counting supports 0..8; unsupported
 * forms deliberately name an undeclared Class_N and fail client compilation.
 * Projections always terminate when cap > 0; partial output is flagged.
 * All input spans must be live; no arbitrary-address validation is promised.
 */

#define SPACE_JOIN_(a, b) a##b
#define SPACE_JOIN(a, b) SPACE_JOIN_(a, b)
#define SPACE_COUNT_(_0, _1, _2, _3, _4, _5, _6, _7, _8, N, ...) N
#define SPACE_COUNT(...) SPACE_COUNT_(0 __VA_OPT__(,) __VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1, 0)
#define SPACE_CONSTRUCT(Class, ...) SPACE_JOIN(Class##_, SPACE_COUNT(__VA_ARGS__))(__VA_ARGS__)

/**
 * Handle storage uses the ecosystem's 23+1 name extent in this draft.
 */
#define MODEL_USER_NAME_CAP 24u

/**
 * Validates bounded ASCII handles, without '@' in the stored identity.
 */
static inline bool Space_nameValid(string name) {
    if (name == nullptr || name[0] == '\0')
        return false;
    for (bytes i = 0; i < MODEL_USER_NAME_CAP; ++i) {
        u8 c = (u8) name[i];
        if (c == 0)
            return true;
        if (!((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '-'))
            return false;
    }
    return false;
}

/**
 * Formats cold metadata, reporting one diagnostic on rejection/truncation.
 */
static inline bool Space_format(utf8 *dest, bytes cap, bool *outTruncated, string format, ...) {
    if (outTruncated != nullptr)
        *outTruncated = false;
    if (dest == nullptr || cap == 0) {
        if (outTruncated != nullptr)
            *outTruncated = true;
        THROW("space projection: missing destination");
        return false;
    }
    va_list args;
    va_start(args, format);
    i32 n = vsnprintf(dest, cap, format, args);
    va_end(args);
    if (n < 0 || (bytes) n >= cap) {
        if (outTruncated != nullptr)
            *outTruncated = true;
        THROW("space projection: truncated");
        return false;
    }
    return true;
}

/**
 * Appends escaped bytes to an already formatted prefix without allocations.
 * Control/non-ASCII bytes become \xHH for an unambiguous byte-span projection.
 * Internal callers supply a valid terminated prefix with available capacity.
 */
static inline bool Space_quote(string text, bytes length, utf8 *dest, bytes cap, bool *outTruncated) {
    static const utf8 HEX[] = "0123456789abcdef";
    bytes at = strlen(dest);
    bool cut = false;
    for (bytes i = 0; i < length; ++i) {
        u8 c = (u8) text[i];
        utf8 escaped[4];
        bytes n = 1;
        escaped[0] = (utf8) c;
        if (c == '\n' || c == '\t' || c == '"' || c == '\\') {
            escaped[0] = '\\';
            escaped[1] = c == '\n' ? 'n' : c == '\t' ? 't' : (utf8) c;
            n = 2;
        } else if (c < ' ' || c >= 127) {
            escaped[0] = '\\';
            escaped[1] = 'x';
            escaped[2] = HEX[c >> 4];
            escaped[3] = HEX[c & 15];
            n = 4;
        }
        if (n >= cap - at) {
            cut = true;
            break;
        }
        memcpy(dest + at, escaped, n);
        at += n;
    }
    if (!cut && cap - at > 2) {
        dest[at++] = '"';
        dest[at++] = '}';
    } else
        cut = true;
    dest[at] = '\0';
    if (outTruncated != nullptr)
        *outTruncated = cut;
    if (cut)
        THROW("space projection: truncated");
    return !cut;
}

#endif
