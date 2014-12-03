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
#ifndef GESTIC_IMPL_H
#define GESTIC_IMPL_H


#include "gestic_api.h"

#include "arch/arch.h"

#define GESTIC_UNUSED(x) (void)x;

/* ======== Section: Message Processing ======== */

/* Function: gestic_handle_system_status
 *
 * The actual handler for System Status (0x15) messages.
 *
 * data - The raw data of the message
 * size - The size of the message
 *
 * This function evaluates the response and signals to <wait_response> that
 * the instruction was processed.
 *
 * See also:
 *    <wait_response>, <gestic_message_handle>
 */
void gestic_handle_system_status(gestic_t *gestic,
                                 const unsigned char *data,
                                 int size);

/* Function: gestic_handle_version_info
 *
 * The actual handler for Version Info (0x83) messages.
 *
 * data - The raw data of the message
 * size - The size of the message
 *
 * See also:
 *    <gestic_message_handle>
 */
void gestic_handle_version_info(gestic_t *gestic,
                                const unsigned char *data,
                                int size);

/* Function: gestic_handle_data_output
 *
 * The actual handler for Sensor Data Output (0x91) messages.
 *
 * data - The raw data of the message
 *
 * See alos:
 *    <gestic_message_handle>
 */
void gestic_handle_data_output(gestic_t *gestic,
                               const unsigned char *data);

/* Function: gestic_handle_runtime_parameter
 *
 * The actual handler for Set Runtime Parameter (0xA2) messages.
 *
 * data - The raw data of the message
 *
 * See alos:
 *    <gestic_message_handle>
 */
void gestic_handle_runtime_parameter(gestic_t *gestic,
                                     const unsigned char *data);

/* ======== Section: Instruction Processing ======== */


/* Function: gestic_request_message
 *
 * Requests the message with msg_id from the device.
 *
 * msg_id  - The id of the requested message
 * param   - Optional parameter that specifies the content of the returned
 *           message for some msg_ids.
 * timeout - Timeout in milliseconds to wait for a response
 *
 * This function returns 0 when the device acknowledged that it sent the
 * message or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 */
int gestic_request_message(gestic_t *gestic,
                           unsigned char msgId,
                           unsigned int param,
                           int timeout);

#endif /* GESTIC_IMPL_H */
