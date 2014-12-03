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
#ifndef GESTIC_STATIC_H
#define GESTIC_STATIC_H

/* This file is included by gestic_api.h and should not be included directly.
 *
 * As most IDEs don't take definitions from the file that is including the
 * header into consideration when parsing a header this file in turn
 * includes gestic_api.h to help the parser find the correct definitions.
 */
#ifndef GESTIC_API_H
#include "gestic_api.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ======== Logger ======== */

#ifndef GESTIC_NO_LOGGING

typedef struct {
    void *opaque;
    gestic_logger_t logger;
} gestic_logging_t;

#endif

/* ======== Parameter Request ======== */

typedef struct {
    int param;
    unsigned int *arg0;
    unsigned int *arg1;
} gestic_param_request_t;

typedef struct {
    char *version;
    int size;
    int received;
} gestic_version_request_t;

/* ======== Structure containing retrieved GestIC data ======== */

#ifndef GESTIC_NO_DATA_RETRIEVAL

typedef struct {
    gestic_signal_t cic;
    gestic_signal_t sd;
    gestic_position_t pos;
    gestic_gesture_t gesture;
    gestic_calib_t calib;
    gestic_touch_t touch;
    gestic_air_wheel_t air_wheel;
    gestic_freq_t frequency;
    gestic_noise_power_t noise_power;
    int frame_counter;
} gestic_input_data_t;

#endif

/* ======== Message Extraction State ======== */

#ifdef GESTIC_USE_MSG_EXTRACT
/* Message size is limited to what could be expressed with one byte */
#define GESTIC_MAX_MESSAGE_SIZE 255

/* The maximum of data that is read from the device at once */
#define GESTIC_INPUT_CAPACITY 1024

typedef struct {
    int state;
    int buffer_cursor;
    int buffer_size;
    unsigned char buffer[GESTIC_INPUT_CAPACITY];
    unsigned char msg[GESTIC_MAX_MESSAGE_SIZE];
} gestic_msg_extract_t;
#endif

/* ======== Flashing Libraries ======== */

#ifndef GESTIC_NO_FLASH

typedef struct {
    unsigned int session_id;
    gestic_UpdateFunction_t session_mode;
    int crc_intialized;
    unsigned int crc_table[256];
} gestic_flash_t;

#endif

/* ======== The gestic_io_t - Structure ======== */

#if GESTIC_IO != GESTIC_IO_CUSTOM
typedef struct {
#ifdef GESTIC_USE_IO_CDC_SERIAL
    void *cdc_serial;
#endif
#ifdef GESTIC_USE_MSG_EXTRACT
    gestic_msg_extract_t msg_extract;
#endif
} gestic_io_t;
#endif

/* ========Section: IO Assertion helpers ======== */

/* Macro: GESTIC_CONNECTED
 *
 * Returns whether the device was connected via <gestic_open>.
 *
 * This macro is used for debugging purpose to detect usage of functions that
 * require a connection even though it wasn't established.
 */
#ifndef GESTIC_CONNECTED
#   ifdef GESTIC_USE_IO_CDC_SERIAL
#       define GESTIC_CONNECTED(GESTIC) ((GESTIC)->io.cdc_serial)
#   else
#       error "Macro GESTIC_CONNECTED not defined"
#   endif
#endif

/* ======== Section: The gestic_t - Structure ======== */

struct gestic_struct {
    volatile int resp_msg_id;
    volatile int resp_error_code;
    gestic_param_request_t * volatile param_request;
    gestic_version_request_t * volatile version_request;

#ifndef GESTIC_NO_DATA_RETRIEVAL
    /* Buffer for the result as fetched via <gestic_data_stream_update> */
    gestic_input_data_t result;
    /* Buffer that contains the state after the last received data-frame */
    gestic_input_data_t internal;
    unsigned char last_time_stamp;
#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
    /* Pointer to data required for synchronization (e.g. a mutex) */
    void *io_sync;
#endif
#endif

#ifndef GESTIC_NO_LOGGING
    gestic_logging_t logging;
#endif
    gestic_io_t io;
#ifndef GESTIC_NO_FLASH
    gestic_flash_t flash;
    unsigned char fw_valid;
#endif
};

/* ======== Section: Connection Handling ======== */

/* Function: gestic_message_receive
 *
 * Called when incoming messages should be received.
 *
 * timeout - Optional pointer to an integer variable containing the timeout.
 *           This function sets timeout to the remaining time when returning.
 *
 * This function returns a negative error code if an error occured.
 *
 * If timeout is not NULL this function might return at any time with
 * <GESTIC_NO_ERROR> even if no message was processed but has to
 * return <GESTIC_NO_DATA> once when the timeout expired.
 *
 * If timeout is NULL this function *might* return <GESTIC_NO_DATA> if
 * *definitely* no message was received and will return <GESTIC_NO_ERROR>
 * otherwise.
 *
 * gestic_message_receive is a customizable function (see <Custom IO Implementation>).
 * Therefore it actual behaviour depends on the IO architecture.
 *
 * Typical implementations will try to read one message and forward it to
 * <gestic_message_handle>. However in case of asynchronous message handling
 * this function might do nothing except returning after a short sleep with
 * <GESTIC_NO_ERROR> when timeout has a nonzero value.
 *
 * Note:
 *    Custom IO implementations have to provide their own implementation
 *    of this function.
 *
 * See also:
 *    <gestic_message_write>, <gestic_message_handle>
 */
int gestic_message_receive(gestic_t *gestic, int *timeout);

/* Function: gestic_message_write
 *
 * Writes a message to the device.
 *
 * msg   - The message to write to the device
 * size  - The size of the message in bytes
 *
 * Returns 0 on success.
 *
 * Note:
 *    Custom IO implementations have to provide their own implementation of
 *    this function.
 *
 * See also:
 *    <gestic_message_receive>
 */
int gestic_message_write(gestic_t *gestic, void *msg, int size);

/* ======== Section: Message Processing ======== */

/* Function: gestic_message_handle
 *
 * Does the evaluation of received messages.
 *
 * msg  - Reference to the received message
 * size - The size of the received message
 *
 * This function is the heart of processing of incoming messages.
 * It decodes the messages and calls the appropriate handlers.
 *
 * See also:
 *    <gestic_message_receive>, <gestic_handle_system_status>,
 *    <gestic_handle_version_info>, <gestic_handle_data_output>,
 *    <gestic_handle_runtime_parameter>
 */
void gestic_message_handle(gestic_t *gestic, const void *msg, int size);

/* ======== Section: Version information ======== */

#define GESTIC_VERSION_MAJOR 1
#define GESTIC_VERSION_MINOR 1
#define GESTIC_VERSION_REVISION 8
#define GESTIC_VERSION_STRING "1.1.8"

#ifdef __cplusplus
}
#endif

#endif /* GESTIC_STATIC_H */
