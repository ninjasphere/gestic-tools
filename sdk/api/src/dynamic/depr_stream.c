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
#include "dynamic.h"

#if defined(GESTIC_API_DYNAMIC) || defined(GESTIC_HAS_DYNAMIC)

#ifndef GESTIC_NO_DATA_RETRIEVAL

int gestic_data_stream(gestic_t *gestic, gestic_data_value_t flags, int timeout) {
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_stream_mask(gestic, flags, timeout);
}

int gestic_update_stream_mask(gestic_t *gestic, gestic_data_value_t flags,
                              gestic_data_value_t mask, int timeout)
{
    /* Mapping of deprecated API-flags to gestic_data_mask_t
     * DSPInfo and AirWheel are always enabled.
     */
    static const unsigned short conv_flags[] = {
        0x0009, 0x0809, 0x0009, 0x0809, 0x1009, 0x1809, 0x1009, 0x1809,
        0x0019, 0x0819, 0x0019, 0x0819, 0x1019, 0x1819, 0x1019, 0x1819,
        0x000B, 0x080B, 0x000B, 0x080B, 0x100B, 0x180B, 0x100B, 0x180B,
        0x001B, 0x081B, 0x001B, 0x081B, 0x101B, 0x181B, 0x101B, 0x181B,
        0x000D, 0x080D, 0x000D, 0x080D, 0x100D, 0x180D, 0x100D, 0x180D,
        0x001D, 0x081D, 0x001D, 0x081D, 0x101D, 0x181D, 0x101D, 0x181D,
        0x000F, 0x080F, 0x000F, 0x080F, 0x100F, 0x180F, 0x100F, 0x180F,
        0x001F, 0x081F, 0x001F, 0x081F, 0x101F, 0x181F, 0x101F, 0x181F,
    };

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_output_enable_mask(gestic, conv_flags[flags & 0x3F], 0,
                                         conv_flags[mask & 0x3F], timeout);
}

int gestic_set_stream_mask(gestic_t *gestic, gestic_data_value_t flags, int timeout) {
    return gestic_update_stream_mask(gestic, flags, 0x3F, timeout);
}

#endif

#endif
