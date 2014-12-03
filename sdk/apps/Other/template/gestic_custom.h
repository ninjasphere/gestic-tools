/******************************************************************************
 *
 * Copyright (C) 2014 Microchip Technology Inc. and its
 *                    subsidiaries ("Microchip").
 *
 * All rights reserved.
 *
 * You are permitted to use the Aurea software, GestIC API, and other
 * accompanying software with Microchip products.  Refer to the license
 * agreement accompanying this software, if any, for additional info regarding
 * your rights and obligations.
 *
 * SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
 * MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
 * PURPOSE. IN NO EVENT SHALL MICROCHIP, SMSC, OR ITS LICENSORS BE LIABLE OR
 * OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH
 * OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY FOR ANY DIRECT OR INDIRECT
 * DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES, OR OTHER SIMILAR COSTS.
 *
 ******************************************************************************/
#ifndef GESTIC_CUSTOM_H
#define GESTIC_CUSTOM_H

/* This file is included by gestic_api.h and should not be included directly.
 *
 * As most IDEs don't take definitions from the file that is including the
 * header into consideration when parsing a header this file in turn
 * includes gestic_api.h to help the parser find the correct definitions.
 */
#ifndef GESTIC_API_H
#include "gestic_api.h"
#endif

/* Disable features that are not needed by this application */

//#define GESTIC_NO_FW_VERSION
//#define GESTIC_NO_FLASH
//#define GESTIC_NO_DATA_RETRIEVAL
//#define GESTIC_NO_RTC
//#define GESTIC_NO_LOGGING

/* Notify GestIC-API that we provide a custom IO-implementation */

#ifdef GESTIC_IO
#undef GESTIC_IO
#endif
#define GESTIC_IO GESTIC_IO_CUSTOM

/* Specify the message handling synchronization mode if
 * gestic_message_handle is to be called outside of
 * gestic_message_receive:
 */
//#define GESTIC_SYNC_INTERRUPT
//#define GESTIC_SYNC_THREADING

/* Definition of custom IO-implementation */

typedef struct {
    /* TODO Add fields required by your implementation */
    int connected;
} gestic_io_t;

/* TODO Adapt this macro */
#define GESTIC_CONNECTED(gestic) ((gestic)->io.connected)

/* TODO Implement architecture specific macros */
#error "Architecture specific macros not implemented"
//#define GET_S8(P)
//#define GET_S16(P) 
//#define GET_S32(P) 
//#define GET_U8(P) 
//#define GET_U16(P) 
//#define GET_U32(P) 
//#define GET_F32(P) 
//#define SET_S8(P, X) 
//#define SET_S16(P, X) 
//#define SET_S32(P, X) 
//#define SET_U8(P, X) 
//#define SET_U16(P, X) 
//#define SET_U32(P, X) 
//#define SET_F32(P, X) 
#include <string.h>
#define GESTIC_MEMSET memset
#define GESTIC_MEMCPY memcpy

#endif /* GESTIC_CUSTOM_H */
