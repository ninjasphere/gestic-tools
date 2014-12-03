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

const char *gestic_version_str(void) {
    return GESTIC_VERSION_STRING;
}

gestic_t *gestic_create(void) {
    return (gestic_t *)GESTIC_MALLOC(sizeof(gestic_t));
}

void gestic_free(gestic_t *gestic) {
    GESTIC_ASSERT(gestic);

    GESTIC_FREE(gestic);
}

gestic_system_error_t gestic_get_system_error(gestic_t *gestic)
{
    GESTIC_ASSERT(gestic);

    return gestic->resp_error_code;
}

#ifndef GESTIC_NO_DATA_RETRIEVAL

gestic_signal_t *gestic_get_cic(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.cic;
}

gestic_signal_t *gestic_get_sd(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.sd;
}

gestic_position_t *gestic_get_position(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.pos;
}

gestic_gesture_t *gestic_get_gesture(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.gesture;
}

gestic_touch_t *gestic_get_touch(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.touch;
}

gestic_air_wheel_t *gestic_get_air_wheel(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.air_wheel;
}

gestic_calib_t *gestic_get_calibration(gestic_t *gestic, int reserved) {
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(reserved == 0);
    GESTIC_UNUSED(reserved);

    return &gestic->result.calib;
}

gestic_freq_t *gestic_get_frequency(gestic_t *gestic) {
    GESTIC_ASSERT(gestic);
    return &gestic->result.frequency;
}

gestic_noise_power_t *gestic_get_noise_power(gestic_t *gestic) {
    GESTIC_ASSERT(gestic);
    return &gestic->result.noise_power;
}

#endif

#endif
