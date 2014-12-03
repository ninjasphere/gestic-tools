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
#ifndef GESTIC_DYNAMIC_H
#define GESTIC_DYNAMIC_H

#include "../impl.h"

#if defined(GESTIC_API_DYNAMIC) || defined(GESTIC_HAS_DYNAMIC)

#ifndef GESTIC_NO_DATA_RETRIEVAL

/* Function: gestic_update_stream_mask DEPRECATED
 *
 * Sets the data to be streamed from the device.
 *
 * flags   - Which data is to be included in the stream as a combination of
 *           <gestic_data_value_t DEPRECATED>-values
 * mask    - Bitmask of what bits of flags are to be updated. The others are
 *           left as they are
 * timeout - Timeout in milliseconds to wait for a response
 *
 * This function translates the stable flags and masks of the API to the flags
 * and masks of the firmware and sets the appropriate runtime-parameter.
 *
 * Note:
 *    Not all combinations of flags and masks are valid as the different data
 *    is contained in one message.
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * Note:
 *    This function is deprecated and only available for backward-compatibility.
 *    Please use <gestic_set_output_enable_mask> instead.
 *
 * See also:
 *    <gestic_data_stream DEPRECATED>, <gestic_set_stream_mask DEPRECATED>,
 *    <gestic_set_param>
 */
int gestic_update_stream_mask(gestic_t *gestic, gestic_data_value_t flags,
                              gestic_data_value_t mask, int timeout);

/* Function: gestic_set_stream_mask DEPRECATED
 *
 * Sets the data to be streamed from the device.
 *
 * flags   - Which data is to be included in the stream as a combination of
 *           <gestic_data_value_t DEPRECATED>-values
 * timeout - Timeout in milliseconds to wait fro a response
 *
 * This is the internal function behind <gestic_data_stream DEPRECATED> that is
 * independant of the actual streaming implementation. It in turn calls
 * <gestic_update_stream_mask DEPRECATED>.
 *
 * If flags is 0 the streaming is stopped. Otherwise streaming gets started or
 * the data that is included in the stream changed.
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * Note:
 *    This function is deprecated and only available for backward-compatibility.
 *    Please use <gestic_set_output_enable_mask> instead.
 *
 * See also:
 *    <gestic_data_stream DEPRECATED>, <gestic_update_stream_mask DEPRECATED>
 */
int gestic_set_stream_mask(gestic_t *gestic, gestic_data_value_t flags,
                           int timeout);

#endif

#endif

#endif /* GESTIC_DYNAMIC_H */
