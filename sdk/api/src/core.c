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
#include "impl.h"

#ifdef GESTIC_USE_MSG_EXTRACT
/* Forward declaration of gestic_init_msg_extract */
void gestic_init_msg_extract(gestic_t *gestic);
#endif

void gestic_initialize(gestic_t *gestic) {
    GESTIC_ASSERT(gestic);

    GESTIC_MEMSET(gestic, 0, sizeof(gestic_t));

#ifdef GESTIC_USE_MSG_EXTRACT
    /* Initialize extraction of messages from serial stream */
    gestic_init_msg_extract(gestic);
#endif

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
    GESTIC_SYNC_INIT(gestic->io_sync);
#endif
}

void gestic_cleanup(gestic_t *gestic) {
    GESTIC_ASSERT(gestic);

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
    GESTIC_SYNC_RELEASE(gestic->io_sync);
#endif
}

void gestic_message_handle(gestic_t *gestic, const void *msg, int size) {
    /* NOTE messages are ensured to have a size of at least 4 bytes  */
    const unsigned char *data = (const unsigned char*)msg;

    switch(GET_U8(data+3)) {
    case gestic_msg_System_Status:
        gestic_handle_system_status(gestic, data, size);
        break;
#if !defined(GESTIC_NO_FW_VERSION) || !defined(GESTIC_NO_FLASH)
    case gestic_msg_Fw_Version_Info:
        gestic_handle_version_info(gestic, data, size);
        break;
#endif
#ifndef GESTIC_NO_DATA_RETRIEVAL
    case gestic_msg_Sensor_Data_Output:
        gestic_handle_data_output(gestic, data);
        break;
#endif
#ifndef GESTIC_NO_RTC
    case gestic_msg_Set_Runtime_Parameter:
        gestic_handle_runtime_parameter(gestic, data);
        break;
#endif
    default:
        break;
    }
}

void gestic_handle_system_status(gestic_t *gestic,
                                 const unsigned char *data,
                                 int size)
{
    if(size == 16) {
        int msg_id = GET_U8(data + 4);
        int error_code = GET_U16(data + 6);
        if(msg_id == gestic->resp_msg_id ||
                error_code == gestic_system_WakeupHappened)
        {
            gestic->resp_msg_id = 0;
            gestic->resp_error_code = error_code;
        }
    } else {
        GESTIC_BAD_DATA("gestic_handle_system_status",
                        "Expected message size of 16 bytes",
                        size);
    }
}

static int wait_response(gestic_t *gestic, int msg_id, int timeout) {
    int error = GESTIC_NO_ERROR;

    gestic->resp_error_code = -1;
    gestic->resp_msg_id = msg_id;

    for(;;) {
        /* Receive and handle message */
        error = gestic_message_receive(gestic, &timeout);
        if(error != GESTIC_NO_ERROR) {
            if(error == GESTIC_NO_DATA)
                error = GESTIC_NO_RESPONSE_ERROR;
            break;
        }

        /* Check whether we got a response */
        if(gestic->resp_error_code >= 0) {
            if(gestic->resp_error_code != 0)
                error = GESTIC_SYSTEM_ERROR;
            break;
        }
    }

    return error;
}

int gestic_send_message(gestic_t *gestic, void *msg, int size, int timeout) {
    int retries;
    int last_error = GESTIC_NO_ERROR;
    int msg_id = ((unsigned char *)msg)[3];

    /* Retry 2 times before accepting a failure */
    for(retries = 3; retries > 0; --retries) {
        last_error = gestic_message_write(gestic, msg, size);
        if(last_error)
            continue;

        last_error = wait_response(gestic, msg_id, timeout);
        if(!last_error)
            break;
    }
    return last_error;
}

int gestic_request_message(gestic_t *gestic, unsigned char msgId, unsigned int param, int timeout) {
    unsigned char msg[12];
    GESTIC_MEMSET(msg, 0, sizeof(msg));
    SET_U8(msg, sizeof(msg));
    SET_U8(msg + 3, gestic_msg_Request_Message);
    SET_U8(msg + 4, msgId);
    SET_U32(msg + 8, param);
    return gestic_send_message(gestic, msg, sizeof(msg), timeout);
}

#ifndef GESTIC_NO_LOGGING

int gestic_log(gestic_t *gestic, const char *fmt, ...)
{
    va_list vlist;
    int result = 0;

    if(gestic->logging.logger) {
        va_start(vlist, fmt);
        result = gestic->logging.logger(gestic->logging.opaque, fmt, vlist);
        va_end(vlist);
    }

    return result;
}

int gestic_set_logger(gestic_t *gestic,
                      gestic_logger_t logger,
                      void *opaque)
{
    gestic->logging.logger = logger;
    gestic->logging.opaque = opaque;
    return GESTIC_NO_ERROR;
}

#endif
