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

#if !defined(GESTIC_NO_FW_VERSION) || !defined(GESTIC_NO_FLASH)
void gestic_handle_version_info(gestic_t *gestic,
                                const unsigned char *data,
                                int size)
{
    gestic_version_request_t *request;
    int v_size;
    if(size != 132) {
        GESTIC_BAD_DATA("gestic_handle_version_info",
                        "Expected message size of 132 bytes",
                        size);
        return;
    }

#ifdef GESTIC_SYNC_THREADING
    /* Synchronize against interaction with the version-request
     * from the Application-Layer
     */
    GESTIC_SYNC_LOCK(gestic->io_sync);
#endif

    gestic->fw_valid = GET_U8(data + 4);
    request = gestic->version_request;
    if(request) {
        v_size = request->size > 120 ? 120 : request->size;
        GESTIC_MEMCPY(request->version, data + 12, v_size);
        request->received = 1;
    }

#ifdef GESTIC_SYNC_THREADING
    /* Release synchronization against Application-Layer */
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#endif
}
#endif

#ifndef GESTIC_NO_FW_VERSION
int gestic_query_fw_version(gestic_t *gestic, char *version,
                            int v_length, int timeout)
{
    gestic_version_request_t request;
    int error;

    request.version = version;
    request.size = v_length;
    request.received = 0;
    gestic->version_request = &request;
    error = gestic_request_message(gestic, gestic_msg_Fw_Version_Info, 0, timeout);

#ifdef GESTIC_SYNC_THREADING
    GESTIC_SYNC_LOCK(gestic->io_sync);
    gestic->version_request = 0;
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#else
    gestic->version_request = 0;
#endif

    if(!error && !request.received)
        error = GESTIC_MSG_MISSING_ERROR;
    return error;
}
#endif
