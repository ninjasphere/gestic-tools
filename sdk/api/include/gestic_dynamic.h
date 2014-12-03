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
#ifndef GESTIC_API_DYNAMIC_H
#define GESTIC_API_DYNAMIC_H

/* This file is included by gestic_api.h and should not be included directly.
 *
 * As most IDEs don't take definitions from the file that is including the
 * header into consideration when parsing a header this file in turn
 * includes gestic_api.h to help the parser find the correct definitions.
 */
#ifndef GESTIC_API_H
#include "gestic_api.h"
#endif

/* ======== Section: Version ======== */

/* Function: gestic_version_str
 *
 * Returns the GestIC API version as a string.
 */
GESTIC_API const char * CDECL gestic_version_str(void);

/* ======== Section: Allocation ======== */

/* Function: gestic_create
 *
 * Returns a newly allocated <gestic_t>-instance that has to be released with
 * <gestic_free> after use or a null pointer if not enough memory was available.
 *
 * Before the instance could be used it has to be initialized with
 * <gestic_initialize>.
 *
 * See also:
 *    <gestic_free>, <gestic_initialize>, <gestic_cleanup>
 */
GESTIC_API gestic_t * CDECL gestic_create(void);

/* Function: gestic_free
 *
 * Releases gestic that was aquired with <gestic_create>.
 *
 * Instances that were initialized with <gestic_initialize> have to be
 * cleaned with <gestic_cleanup> before.
 *
 * See also:
 *    <gestic_create>, <gestic_initialize>, <gestic_cleanup>
 */
GESTIC_API void CDECL gestic_free(gestic_t *gestic);

/* Function: gestic_get_system_error
 *
 * Returns the error code of the response to the last request.
 *
 * See also:
 *    <gestic_system_error_t>, <GESTIC_SYSTEM_ERROR>, <gestic_send_message>
 */
GESTIC_API gestic_system_error_t CDECL gestic_get_system_error(gestic_t *gestic);

/* ======== Section: Data Access ======== */

#ifndef GESTIC_NO_DATA_RETRIEVAL

/* Function: gestic_get_cic
 *
 * Returns the pointer to the CIC-signals.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_signal_t * CDECL gestic_get_cic(gestic_t *gestic, int reserved);

/* Function: gestic_get_sd
 *
 * Returns the pointer to the SD-signals.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_signal_t * CDECL gestic_get_sd(gestic_t *gestic, int reserved);

/* Function: gestic_get_position
 *
 * Returns the pointer to the position-data.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_position_t * CDECL gestic_get_position(gestic_t *gestic, int reserved);

/* Function: gestic_get_gesture
 *
 * Returns the pointer to the gesture-data.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_gesture_t * CDECL gestic_get_gesture(gestic_t *gestic, int reserved);

/* Function: gestic_get_touch
 *
 * Returns the pointer to the touch-related data of gestic.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_touch_t * CDECL gestic_get_touch(gestic_t *gestic, int reserved);

/* Function: gestic_get_air_wheel
 *
 * Return the pointer to the AirWheel-related data of gestic.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 */
GESTIC_API gestic_air_wheel_t * CDECL gestic_get_air_wheel(gestic_t *gestic, int reserved);

/* Function: gestic_get_calibration
 *
 * Returns the pointer to the calibration-data.
 *
 * reserved - Reserved argument for future use. Has to be 0.
 *
 * Important:
 *    Calibration data is only communicated when <gestic_data_mask_sd> was selected
 *    via <gestic_set_output_enable_mask>. Otherwise calibrations would
 *    not be detected.
 */
GESTIC_API gestic_calib_t * CDECL gestic_get_calibration(gestic_t *gestic, int reserved);

/* Function: gestic_get_frequency
 *
 * Returns the pointer to the frequency-data of gestic.
 */
GESTIC_API gestic_freq_t * CDECL gestic_get_frequency(gestic_t *gestic);

/* Function: gestic_get_noise_power
 *
 * Returns the pointer to the noise-power-data of gestic.
 */
GESTIC_API gestic_noise_power_t * CDECL gestic_get_noise_power(gestic_t *gestic);

/* ======== Section: Deprecated Functions ======== */

/* Enumeration: gestic_data_value_t DEPRECATED
 *
 * Bitmask of data that could be requested from the device with
 * <gestic_data_stream DEPRECATED>.
 *
 * gestic_data_cic      - CIC signals, accessed through <gestic_get_cic>
 * gestic_data_sd       - Signal deviation and calibration info, accessed
 *                        through <gestic_get_sd> and <gestic_get_calibration>
 * gestic_data_position - Position, accessed through <gestic_get_position>
 * gestic_data_gesture  - Gestures, accessed through <gestic_get_gesture>
 *
 * Note:
 *    This enumeration is deprecated and only available for
 *    backward-compatibility.
 *    Please use <gestic_data_mask_t> instead.
 *
 * See also:
 *    <gestic_data_stream DEPRECATED>
 */
typedef enum {
    gestic_data_cic = 0x01,
    gestic_data_sd = 0x04,
    gestic_data_position = 0x08,
    gestic_data_gesture = 0x10,
    gestic_data_touch = 0x20
} gestic_data_value_t;

/* Function: gestic_data_stream DEPRECATED
 *
 * Starts, changes or stops data-streaming.
 *
 * flags   - Which data is to be included in the stream as a combination of
 *           <gestic_data_value_t DEPRECATED>-values
 * timeout - Timeout in milliseconds to wait for a response
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
 */
GESTIC_API int CDECL gestic_data_stream(gestic_t *gestic,
                                  gestic_data_value_t flags,
                                  int timeout);

#endif

#endif /* GESTIC_API_DYNAMIC_H */
