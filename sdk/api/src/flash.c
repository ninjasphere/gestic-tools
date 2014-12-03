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

#ifndef GESTIC_NO_FLASH

#define CRC_P_32 0xEDB88320L

static unsigned int crc(gestic_t *gestic, const unsigned char *msg, int size)
{
    unsigned int crc = 0xFFFFFFFF;
    int i;
    for(i = 0; i < size; ++i)
        crc = (crc >> 8) ^ gestic->flash.crc_table[(crc ^ msg[i]) & 0xFF];
    return crc ^ 0xFFFFFFFF;
}

static void init_crc(gestic_t *gestic)
{
    int i, j;
    unsigned long crc;

    for (i = 0; i < 256; ++i) {
        crc = i;

        for (j = 0; j < 8; ++j) {
            if ( crc & 0x00000001L )
                crc = (crc >> 1) ^ CRC_P_32;
            else
                crc = crc >> 1;
        }

        gestic->flash.crc_table[i] = crc;
    }

    gestic->flash.crc_intialized = 1;
}

int gestic_wait_for_version_info(gestic_t *gestic, int timeout)
{
    int error = GESTIC_NO_ERROR;

    for(;;) {
        /* Receive and handle one message */
        error = gestic_message_receive(gestic, &timeout);
        if(error != GESTIC_NO_ERROR) {
            if(error == GESTIC_NO_DATA)
                error = GESTIC_NO_RESPONSE_ERROR;
            break;
        }

        /* Check whether we received version info */
        if(gestic->version_request->received) {
            error = GESTIC_NO_ERROR;
            break;
        }
    }

    return error;
}

int gestic_flash_begin(gestic_t *gestic, unsigned int session_id, void *iv,
                       gestic_UpdateFunction_t mode, int timeout)
{
    int error = GESTIC_NO_ERROR;
    unsigned char msg[28];
    gestic_version_request_t v_request;

    /* Init table for CRC32-checksum */
    if(!gestic->flash.crc_intialized)
        init_crc(gestic);

    /* Prepare flash start message */
    GESTIC_MEMSET(msg, 0, sizeof(msg));
    SET_U8(msg, sizeof(msg));
    SET_U8(msg + 3, gestic_msg_Fw_Update_Start);
    SET_U32(msg+8, session_id);
    GESTIC_MEMCPY(msg+12, iv, 14);
    SET_U8(msg+26, mode);

    GESTIC_MEMSET(&v_request, 0, sizeof(v_request));

    SET_U32(msg+4, crc(gestic, msg+8, 20));

    gestic->flash.session_id = session_id;

    /* Reset device and wait for the firmware-version */
    gestic->version_request = &v_request;
    error = gestic_reset(gestic);

    if(!error)
        error = gestic_wait_for_version_info(gestic, timeout);
    gestic->version_request = 0;

    /* Send message to start flash-mode */
    if(!error)
        error = gestic_send_message(gestic, msg, sizeof(msg), timeout);

    return error;
}

int gestic_flash_write(gestic_t *gestic, unsigned short address,
                       unsigned char length, unsigned char *record,
                       gestic_UpdateFunction_t mode, int timeout)
{
    unsigned char msg[140];

    /* Assert that only verification is used for verification sessions */
    GESTIC_ASSERT(gestic->flash.session_mode != gestic_UpdateFunction_VerifyOnly ||
            mode == gestic_UpdateFunction_VerifyOnly);

    GESTIC_MEMSET(msg, 0, sizeof(msg));
    SET_U8(msg, sizeof(msg));
    SET_U8(msg + 3, gestic_msg_Fw_Update_Block);
    SET_U16(msg + 8, address);
    SET_U8(msg + 10, length);
    SET_U8(msg + 11, mode);
    GESTIC_MEMCPY(msg + 12, record, 128);

    SET_U32(msg + 4, crc(gestic, msg + 8, 132));

    return gestic_send_message(gestic, msg, sizeof(msg), timeout);
}

int gestic_flash_end(gestic_t *gestic, unsigned char *version, int timeout)
{
    int error = GESTIC_NO_ERROR;
    unsigned char msg[136];
    GESTIC_MEMSET(msg, 0, sizeof(msg));

    /* Finish by writing the version information */

    SET_U8(msg, sizeof(msg));
    SET_U8(msg + 3, gestic_msg_Fw_Update_Completed);
    SET_U32(msg + 8, gestic->flash.session_id);
    SET_U8(msg + 12, gestic->flash.session_mode);
    GESTIC_MEMCPY(msg + 13, version, 120);

    SET_U32(msg + 4, crc(gestic, msg + 8, 128));

    error = gestic_send_message(gestic, msg, sizeof(msg), timeout);

    /* Finally restart device */

    if(!error) {
        SET_U8(msg + 12, gestic_UpdateFunction_Restart);
        GESTIC_MEMSET(msg + 13, 0, 120);

        SET_U32(msg + 4, crc(gestic, msg + 8, 128));

        error = gestic_send_message(gestic, msg, sizeof(msg), timeout);
    }

    return error;
}

int gestic_flash_image(gestic_t *gestic,
                       unsigned int session_id,
                       gestic_flash_image_t *image,
                       gestic_UpdateFunction_t mode,
                       int timeout)
{
    int error = GESTIC_NO_ERROR;
    int i;
    gestic_flash_record_t *record;

    error = gestic_flash_begin(gestic, session_id, image->iv, mode, timeout);

    for(i = 0; !error && i < image->record_count; ++i) {
        record = image->data + i;
        error = gestic_flash_write(gestic, record->address, record->length,
                                   record->data, mode, timeout);
    }

    if(!error)
        error = gestic_flash_end(gestic, image->fw_version, timeout);

    return error;
}

int gestic_flash_wait_loader_updated(gestic_t *gestic,
                                     int timeout)
{
    int error = GESTIC_NO_ERROR;

    /* 0xFF is an invalid value for this field in fw-version-info */
    gestic->fw_valid = 0xFF;

    for(;;) {
        /* Receive and handle one message */
        error = gestic_message_receive(gestic, &timeout);
        if(error != GESTIC_NO_ERROR) {
            if(error == GESTIC_NO_DATA)
                error = GESTIC_NO_RESPONSE_ERROR;
            break;
        }

        /* Check whether we got a response */
        if(gestic->fw_valid == 0) {
            error = GESTIC_NO_ERROR;
            break;
        }
    }

    return error;
}

#endif
