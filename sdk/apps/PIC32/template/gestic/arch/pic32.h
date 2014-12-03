/******************************************************************************
 *
 * Copyright (C) 2014 Microchip Technology Inc. and its
 *                    subsidiaries (“Microchip”).
 *
 * All rights reserved.
 *
 * You are permitted to use the Aurea software, GestIC API, and other
 * accompanying software with Microchip products.  Refer to the license
 * agreement accompanying this software, if any, for additional info regarding
 * your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP, SMSC, OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH
 * OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY FOR ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR OTHER SIMILAR COSTS.
 *
 ******************************************************************************/
#ifndef GESTIC_ARCH_PIC32_H
#define GESTIC_ARCH_PIC32_H

#include "gestic_api.h"

/* ======== Packing / Unpacking ======== */

/* Defines: Data Packing Macros
 *
 * Macros for operation on values in messages.
 *
 * GET_S8  - Loads 8-bit signed integer
 * GET_S16 - Loads 16-bit signed integer
 * GET_S32 - Loads 32-bit signed integer
 * GET_U8  - Loads 8-bit unsigned integer
 * GET_U16 - Loads 16-bit unsigned integer
 * GET_U32 - Loads 32-bit unsigned integer
 * GET_F32 - Loads 32-bit float number
 * SET_S8  - Stores 8-bit signed integer
 * SET_S16 - Stores 16-bit signed integer
 * SET_S32 - Stores 32-bit signed integer
 * SET_U8  - Stores 8-bit unsigned integer
 * SET_U16 - Stores 16-bit unsigned integer
 * SET_U32 - Stores 32-bit unsigned integer
 * SET_F32 - Stores 32-bit float number
 *
 * The values are stored in messages as a sequence of unsigned chars
 * in little-endian order.
 *
 * Note:
 *    In contrast to earlier versions P might be stored in any location and
 *    is *not* aligned to the size of the contained variable.
 * Note:
 *    Those macros have to be replaced for big endian platforms.
 */
#ifndef GET_U32
extern inline unsigned short __attribute__((always_inline)) get_u16(const unsigned char *p)
{
    return p[0] | (p[1]<<8);
}
extern inline unsigned int __attribute__((always_inline)) get_u32(const unsigned char *p)
{
    return p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
}
extern inline float __attribute__((always_inline)) get_f32(const unsigned char *p)
{
    unsigned int r = p[0] | (p[1]<<8) | (p[2]<<16) | (p[3]<<24);
    return *(float*)&r;
}
extern inline void __attribute__((always_inline)) set_u16(unsigned char *p, unsigned short v)
{
    p[0] = v & 0xFF;
    p[1] = (v & 0xFF00) >> 8;
}
extern inline void __attribute__((always_inline)) set_u32(unsigned char *p, unsigned int v)
{
    p[0] = v & 0xFF;
    p[1] = (v & 0xFF00) >> 8;
    p[2] = (v & 0xFF0000) >> 16;
    p[3] = (v & 0xFF000000) >> 24;
}
extern inline void __attribute__((always_inline)) set_f32(unsigned char *p, float v)
{
    unsigned int r = *(float*)&v;
    p[0] = r & 0xFF;
    p[1] = (r & 0xFF00) >> 8;
    p[2] = (r & 0xFF0000) >> 16;
    p[3] = (r & 0xFF000000) >> 24;
}
#   define GET_S8(P) (*(char*)(P))
#   define GET_S16(P) ((short)get_u16(P))
#   define GET_S32(P) ((int)get_u32(P))
#   define GET_U8(P) (*(unsigned char*)(P))
#   define GET_U16(P) (get_u16(P))
#   define GET_U32(P) (get_u32(P))
#   define GET_F32(P) (get_f32(P))
#   define SET_S8(P, X) (*(char*)(P) = (char)(X))
#   define SET_S16(P, X) (set_u16(P, (X)))
#   define SET_S32(P, X) (set_u32(P, (X)))
#   define SET_U8(P, X) (*(unsigned char*)(P) = (unsigned char)(X))
#   define SET_U16(P, X) (set_u16(P, (X)))
#   define SET_U32(P, X) (set_u32(P, (X)))
#   define SET_F32(P, X) (set_f32(P, (X)))
#endif

/* ======== Assertion ======== */

#ifndef GESTIC_ASSERT
#   include <assert.h>
#   define GESTIC_ASSERT(X) assert(X)
#endif

/* ======== Memory Operations ======== */

#ifdef GESTIC_HAS_DYNAMIC
#   ifndef GESTIC_NO_ALLOCATION
#       include <stdlib.h>
#       ifndef GESTIC_MALLOC
#           define GESTIC_MALLOC malloc
#       endif
#       ifndef GESTIC_FREE
#           define GESTIC_FREE free
#       endif
#   endif
#endif

#if !defined(GESTIC_MEMSET) || !defined(GESTIC_MEMCPY)
#   include <string.h>
#   ifndef GESTIC_MEMSET
#       define GESTIC_MEMSET memset
#   endif
#   ifndef GESTIC_MEMCPY
#       define GESTIC_MEMCPY memcpy
#   endif
#endif

/* ======== Synchronisation etc. ======== */

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
#   if !defined(GESTIC_SYNC_INIT) || !defined(GESTIC_SYNC_LOCK) || !defined(GESTIC_SYNC_UNLOCK) || !defined(GESTIC_SYNC_RELEASE)
#       error "Missing definition of locking mechanisms"
#   endif
#endif

/* ======== Logging (not implemented by default). ======== */

#ifndef GESTIC_BAD_DATA
#   define GESTIC_BAD_DATA(FUNC, MSG, VALUE) ((void)0)
#endif

#endif /* GESTIC_ARCH_PIC32_H */
