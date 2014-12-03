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

void gestic_handle_runtime_parameter(gestic_t *gestic,
                                     const unsigned char *data)
{
    gestic_param_request_t *request;
    int param;

#ifdef GESTIC_SYNC_THREADING
    /* Synchronize against interaction with the param_request
     * from the Application-Layer
     */
    GESTIC_SYNC_LOCK(gestic->io_sync);
#endif

    request = gestic->param_request;
    param = GET_U16(data + 4);
    if(request && request->param == param) {
        if(request->arg0)
            *request->arg0 = GET_U32(data + 8);
        if(request->arg1)
            *request->arg1 = GET_U32(data + 12);
        request->param = 0;
    }

#ifdef GESTIC_SYNC_THREADING
    /* Release synchronization against Application-Layer */
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#endif
}

int gestic_set_param(gestic_t *gestic, unsigned short param, unsigned int arg0, unsigned int arg1, int timeout) {
    unsigned char msg[16];
    GESTIC_MEMSET(msg, 0, sizeof(msg));
    SET_U8(msg, sizeof(msg));
    SET_U8(msg + 3, gestic_msg_Set_Runtime_Parameter);
    SET_U16(msg + 4, param);
    SET_U32(msg + 8, arg0);
    SET_U32(msg + 12, arg1);
    return gestic_send_message(gestic, msg, sizeof(msg), timeout);
}

int gestic_get_param(gestic_t *gestic, unsigned short param, unsigned int *arg0, unsigned int *arg1, int timeout)
{
    gestic_param_request_t request;
    int error;

    /* Enable receiving of parameters */
    request.param = param;
    request.arg0 = arg0;
    request.arg1 = arg1;
    gestic->param_request = &request;

    /* Do actual receiving */
    error = gestic_request_message(gestic, gestic_msg_Set_Runtime_Parameter, param, timeout);

    /* Disable receiving of parameters */
#ifdef GESTIC_SYNC_THREADING
    GESTIC_SYNC_LOCK(gestic->io_sync);
    gestic->param_request = 0;
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#else
    gestic->param_request = 0;
#endif

    /* Check wheter parameter was received */
    if(error == GESTIC_NO_ERROR && request.param)
        error = GESTIC_MSG_MISSING_ERROR;
    return error;
}

int gestic_trigger_action(gestic_t *gestic, unsigned short action, int timeout) {
    return gestic_set_param(gestic, gestic_param_trigger, action, 0, timeout);
}

#ifndef GESTIC_NO_DATA_RETRIEVAL

int gestic_set_output_enable_mask(gestic_t *gestic, gestic_data_mask_t flags,
                                  gestic_data_mask_t lock,
                                  gestic_data_mask_t mask, int timeout)
{
    int error;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_set_param(gestic, gestic_param_dataOutputLockMask,
                              flags, lock, timeout);

    if(!error)
        error = gestic_set_param(gestic, gestic_param_dataOutputEnableMask,
                                flags, mask, timeout);

    return error;
}

int gestic_get_output_enable_mask(gestic_t *gestic, gestic_data_mask_t *flags,
                                  gestic_data_mask_t *locked, int timeout)
{
    int error;
    unsigned int value;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    if(flags) {
        error = gestic_get_param(gestic, gestic_param_dataOutputEnableMask,
                                  &value, 0, timeout);

        if(!error)
            *flags = value & gestic_data_mask_all;
    }
    if(!error && locked) {
        error = gestic_get_param(gestic, gestic_param_dataOutputLockMask,
                                  &value, 0, timeout);
        if(!error)
            *locked = value & gestic_data_mask_all;
    }
    return error;
}

#endif

#ifndef GESTIC_NO_RTC

int gestic_set_auto_calibration(gestic_t *gestic, int enabled, int timeout) {
    int mode = enabled ? 0x00 : 0x3F;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_param(gestic, gestic_param_dspCalOpMode, mode, 0x3F, timeout);
}

int gestic_get_auto_calibration(gestic_t *gestic, int *enabled, int timeout) {
    int error;
    unsigned int value;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_get_param(gestic, gestic_param_dspCalOpMode, &value, 0, timeout);

    if(!error && enabled)
        *enabled = value & 0x3F;
    return error;
}

int gestic_force_calibration(gestic_t *gestic, int timeout) {
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_trigger_action(gestic, gestic_trigger_calibration, timeout);
}

int gestic_select_frequencies(gestic_t *gestic, unsigned int frequencies, int timeout) {
    int error = GESTIC_BAD_PARAM_ERROR;
    int count = 0;
    int list = 0xFFFFF;
    int i;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    for(i = 0; i < 5; ++i) {
        if(frequencies & (1 << i)) {
            list = (list << 4) | i;
            ++count;
        }
    }

    list &= 0x000FFFFF;

    if(count)
        error = gestic_set_param(gestic, gestic_param_transFreqSelect, count, list, timeout);

    return error;

}

int gestic_set_approach_detection(gestic_t *gestic, int enabled, int timeout) {
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_param(gestic, gestic_param_dspApproachDetectionMode, enabled ? 0x01 : 0x00, 0x01, timeout);
}

int gestic_get_approach_detection(gestic_t *gestic, int *enabled, int timeout) {
    int error;
    unsigned int value;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_get_param(gestic, gestic_param_dspApproachDetectionMode, &value, 0, timeout);

    if(!error && enabled)
        *enabled = value & 0x01;
    return error;
}

int gestic_set_enabled_gestures(gestic_t *gestic, int gestures, int timeout) {
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_param(gestic, gestic_param_dspGestureMask, gestures & 0x7F, 0x7F, timeout);
}

int gestic_get_enabled_gestures(gestic_t *gestic, int *gestures, int timeout) {
    int error;
    unsigned int value;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_get_param(gestic, gestic_param_dspGestureMask, &value, 0, timeout);

    if(!error && gestures)
        *gestures = value & 0x7F;
    return error;
}

int gestic_set_touch_detection(gestic_t *gestic, int enabled, int timeout) {
    unsigned int touchFlag = 0;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    if(enabled)
        touchFlag = 0x08;
    return gestic_set_param(gestic, gestic_param_dspTouchConfig, touchFlag, 0x08, timeout);
}

int gestic_get_touch_detection(gestic_t *gestic, int *enabled, int timeout) {
    int error;
    unsigned int value;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_get_param(gestic, gestic_param_dspTouchConfig, &value, 0, timeout);

    if(!error && enabled)
        *enabled = value & 0x80;
    return error;
}

int gestic_set_air_wheel_enabled(gestic_t *gestic, int enabled, int timeout) {
    unsigned int airWheelFlag = 0;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    if(enabled)
        airWheelFlag = 0x20;
    return gestic_set_param(gestic, gestic_param_dspAirWheelConfig, airWheelFlag, 0x20, timeout);
}

int gestic_get_air_wheel_enabled(gestic_t *gestic, int *enabled, int timeout) {
    int error;
    unsigned int value;
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    error = gestic_get_param(gestic, gestic_param_dspAirWheelConfig, &value, 0, timeout);

    if(!error && enabled)
        *enabled = value & 0x20;
    return error;
}

int gestic_make_persistent(gestic_t *gestic,
                           gestic_param_category category,
                           int timeout)
{
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    return gestic_set_param(gestic, gestic_param_makePersistent, category, 0, timeout);
}


#endif
