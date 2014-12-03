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
#ifndef GESTIC_IO_H
#define GESTIC_IO_H

#include "../impl.h"

/* ======== Section: Serial IO ======== */

#ifdef GESTIC_HAS_SERIAL_IO

/* Function: gestic_serial_read
 *
 * Tries to read at max maxsize data from the device to buffer.
 * Returns the amount of read data.
 */
int gestic_serial_read(gestic_t *gestic, void *buffer, int maxsize);

/* Function: gestic_serial_write
 *
 * Writes size bytes from buffer to the device.
 * Returns size on success or a negative error code on failure.
 */
int gestic_serial_write(gestic_t *gestic, void *buffer, int size);

#endif /* GESTIC_HAS_SERIAL_IO */

/* ======== Section: Message-IO ========*/

/* ======== Section: Message Extraction ======== */

#ifdef GESTIC_USE_MSG_EXTRACT
/* Function: gestic_init_msg_extract
 *
 * Initializes the state-machine of <message_extract>.
 *
 * This function is called by <gestic_initialize>.
 */
void gestic_init_msg_extract(gestic_t *gestic);
#endif

#endif /* GESTIC_IO_H */
