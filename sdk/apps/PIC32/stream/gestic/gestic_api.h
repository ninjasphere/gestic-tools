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
#ifndef GESTIC_API_H
#define GESTIC_API_H


/* ======== IO-Types ======== */

/* Possible IO-types */
#define GESTIC_IO_CUSTOM 0
#define GESTIC_IO_CDC_SERIAL 1

/* ======== Declaration of gestic_t ======== */

#ifndef GESTIC_NO_GESTIC_TYPEDEF
typedef struct gestic_struct gestic_t;
#endif

/* gestic_custom.h allows to provide extra configuration and functionality
 * on a per project-basis.
 * This is mainly used by projects for embedded devices that provide their
 * IO-functionality and configuration this way.
 */
#ifdef GESTIC_CUSTOM
#include "gestic_custom.h"
#endif

#if defined(GESTIC_API_DYNAMIC) && defined(_WIN32)
#   ifdef GESTIC_API_EXPORT
#       define GESTIC_API __declspec(dllexport)
#   else
#       define GESTIC_API __declspec(dllimport)
#   endif
#else
#   define GESTIC_API
#endif

#ifndef CDECL
#   ifdef _WIN32
#       define CDECL __cdecl
#   else
#       define CDECL
#   endif
#endif

#ifndef GESTIC_UNDEFINED_VALUE
#   define GESTIC_UNDEFINED_VALUE 0
#endif

/* ======== IO-related configuration ======== */

/* Default IO-implementation */
#ifndef GESTIC_IO
#if defined(_WIN32) || defined(__linux__)
#   define GESTIC_IO GESTIC_IO_CDC_SERIAL
#else
#   error "No IO-implementation selected"
#endif
#endif

#if GESTIC_IO == GESTIC_IO_CUSTOM
/* Nothing to define here */
#elif GESTIC_IO == GESTIC_IO_CDC_SERIAL
#   define GESTIC_HAS_SERIAL_IO
#   define GESTIC_USE_IO_CDC_SERIAL
#   define GESTIC_USE_MSG_EXTRACT
#else
#   error "Unknown IO implementation selected"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ======== Section: Error Codes ======== */

/* Enum: gestic_error_t
 *
 * Error codes as returned by API functions of the GestIC SDK.
 *
 * GESTIC_NO_ERROR                - No Error Occured
 * GESTIC_NO_DATA                 - <gestic_data_stream_update> could not retrieve new data.
 *                                  This code does not necessarily mark an error.
 * GESTIC_SYSTEM_ERROR            - Last System_Status message contained an error state
 *                                  See <gestic_get_system_error> and <gestic_system_error_t> for details.
 * GESTIC_NO_RESPONSE_ERROR       - Could not receive an response for the last sent instruction
 * GESTIC_MSG_MISSING_ERROR       - Request for message was acknowledged but no message or requested type
 *                                  was received.
 * GESTIC_IO_ERROR                - There was some unspecified error during communication
 * GESTIC_IO_CTL_ERROR            - Configuration of the connection to the device failed with an error
 * GESTIC_IO_OPEN_ERROR           - Opening the connection to the device failed
 * GESTIC_IO_ENUM_ERROR           - Error during automatic device detection.
 *                                  This error is specific to systems that provide automatic
 *                                  device detection
 * GESTIC_BAD_PARAM_ERROR         - Parameter of a function call was invalid in that context
 * GESTIC_NO_IMPLEMENTATION_ERROR - The implementation of the called function is missing or incomplete
 */

typedef enum {
    GESTIC_NO_ERROR = 0,
    GESTIC_NO_DATA = -1,
    GESTIC_SYSTEM_ERROR = -8,
    GESTIC_NO_RESPONSE_ERROR = -9,
    GESTIC_MSG_MISSING_ERROR = -10,
    GESTIC_IO_ERROR = -16,
    GESTIC_IO_CTL_ERROR = -17,
    GESTIC_IO_OPEN_ERROR = -18,
    GESTIC_IO_ENUM_ERROR = -19,
    GESTIC_BAD_PARAM_ERROR = -32,
    GESTIC_NO_IMPLEMENTATION_ERROR = -48
} gestic_error_t;

/* Enum: gestic_system_error_t
 *
 * gestic_system_NoError              - No error occured
 * gestic_system_UnknownCommand       - Message ID is unknown
 * gestic_system_InvalidSessionid     - Session ID is invalid or does not match (<Firmware Version and Update>)
 * gestic_system_InvalidCrc           - CRC is invalid (<Firmware Version and Update>)
 * gestic_system_InvalidLength        - Length is invalid (<Firmware Version and Update>)
 * gestic_system_InvalidAddress       - Address is invalid (<Firmware Version and Update>)
 * gestic_system_InvalidFunction      - Function id is invalid (<Firmware Version and Update>)
 * gestic_system_ContentMismatch      - <gestic_UpdateFunction_VerifyOnly> found mismatch between
 *                                      sent data block and flash memory (<Firmware Version and Update>)
 * gestic_system_WrongParameterAddr   - Parameter start address does not match
                                        Library Loader assumptions (<Firmware Version and Update>)
 * gestic_system_WrongParameterValue  - Parameter Value is invalid
 * gestic_system_UnknownParameterID   - Parameter ID is invalid
 * gestic_system_WakeupHappened       - Device was sleeping and therefore could not
 *                                      process the request
 * gestic_system_LoaderUpdateStarted  - Library Loader Update has started (<Firmware Version and Update>)
 * gestic_system_LoaderUpdateFinished - Library Loader Update has finished (<Firmware Version and Update>)
 */
typedef enum {
    gestic_system_NoError = 0,
    gestic_system_UnknownCommand = 0x01,
    gestic_system_InvalidSessionid = 0x02,
    gestic_system_InvalidCrc = 0x03,
    gestic_system_InvalidLength = 0x04,
    gestic_system_InvalidAddress = 0x05,
    gestic_system_InvalidFunction = 0x06,
    gestic_system_ContentMismatch = 0x08,
    gestic_system_WrongParameterAddr = 0x0B,
    gestic_system_WrongParameterValue = 0x14,
    gestic_system_UnknownParameterID = 0x15,
    gestic_system_WakeupHappened = 0x1A,
    gestic_system_LoaderUpdateStarted = 0x80,
    gestic_system_LoaderUpdateFinished = 0x81
} gestic_system_error_t;


/* ======== Section: Common Functions ======== */

/* Function: gestic_initialize
 *
 * Initializes gestic for further use. Resources that were aquired with
 * <gestic_initialize> have to be released with <gestic_cleanup> once they are
 * no longer needed.
 *
 * gestic - The <gestic_t>-instance to be initialized.
 *          New instances could be aquired with <gestic_create>.
 *
 * See also:
 *    <gestic_create>, <gestic_free>, <gestic_cleanup>
 */
GESTIC_API void CDECL gestic_initialize(gestic_t *gestic);

/* Function: gestic_cleanup
 *
 * Releases resources that were aquired by <gestic_initialize>.
 * 
 * See also:
 *    <gestic_initialize>
 */
GESTIC_API void CDECL gestic_cleanup(gestic_t *gestic);

/* ======== Section: Logging ======== */

#ifndef GESTIC_NO_LOGGING

#include <stdarg.h>

/* Function: gestic_log
 *
 * Calls the logger set with <gestic_set_logger> to do printf like output.
 *
 * fmt - The format string
 * ... - The extra parameters that are forwarded via va_list
 *
 * If not logger is set this function returns 0. Otherwise it returns
 * the return-value of the call to the logger-function.
 *
 * See also:
 *    <gestic_set_logger>
 */
GESTIC_API int CDECL gestic_log(gestic_t *gestic, const char *fmt, ...);

/* Typedef: gestic_logger_t
 *
 * Definition of the signature for logger functions.
 *
 * opaque - <gestic_log> will pass the opaque pointer provided with
 *          <gestic_set_logger>
 * fmt    - The format string
 * vlist  - List of the variadic arguments
 *
 * See also:
 *    <gestic_set_logger>
 */
typedef int (CDECL* gestic_logger_t)(void *opaque,
                                     const char *fmt,
                                     va_list vlist);

/* Function: gestic_set_logger
 *
 * Sets the logger implementation that is called by <gestic_log>.
 *
 * logger - A logger function accepting the opaque-pointer, format string
 *          and the variadic argument list as parameters
 * opaque - An opaque pointer that is provided on calls of logger as the
 *          first argument
 *
 * This function always return 0.
 *
 * An example of valid functions for logger is vfprintf.
 *
 * See also:
 *    <gestic_log>, <gestic_logger_t>
 */
GESTIC_API int CDECL gestic_set_logger(gestic_t *gestic,
                                       gestic_logger_t logger,
                                       void *opaque);

#endif

/* ======== Section: Connection Handling ======== */

/* Function: gestic_open
 *
 * Opens a connection to the physical device and associates it with gestic
 * that was already initialized with <gestic_initialize>.
 *
 * Returns 0 on success or a negative value on error.
 *
 * The connection has to be closed with <gestic_close> after use.
 *
 * See also:
 *    <gestic_initialize>, <gestic_close>
 */
GESTIC_API int CDECL gestic_open(gestic_t *gestic);

/* Function: gestic_close
 *
 * Closes the connection to the device associated with gestic that was
 * established with <gestic_open>.
 */
GESTIC_API void CDECL gestic_close(gestic_t *gestic);

/* Function: gestic_reset
 *
 * Tries to reset the GestIC-chip if this is supported by the connection.
 *
 * Returns 0 on success or a negative value on error.
 *
 * Resetting the hardware is done by signaling the reset line of the chip.
 * This requires support by the hardware connection.
 */
GESTIC_API int CDECL gestic_reset(gestic_t *gestic);

/* ======== Section: Communication Related Enums ======== */


typedef enum {
    gestic_msg_System_Status = 0x15,
    gestic_msg_Request_Message = 0x06,
    gestic_msg_Fw_Update_Start = 0x80,
    gestic_msg_Fw_Update_Block = 0x81,
    gestic_msg_Fw_Update_Completed = 0x82,
    gestic_msg_Fw_Version_Info = 0x83,
    gestic_msg_Sensor_Data_Output = 0x91,
    gestic_msg_Set_Runtime_Parameter = 0xA2,
} gestic_message_id_t;

/* Enum: gestic_parameter_id_t
 *
 * Enumeration of possible parameter ids.
 * For the correct arguments refer to the MGC3130 GestIC Library
 * Interface Description User's Guids or use the
 * <Real time control (RTC)> functions.
 *
 * gestic_param_trigger                     - Trigger an action.
 *                                            See <gestic_trigger_action>
 * gestic_param_makePersistent              - Make changes persistent.
 * gestic_param_afeRxAtt_S                  - Signal Matching for South electrode
 * gestic_param_afeRxAtt_W                  - Signal Matching for West electrode
 * gestic_param_afeRxAtt_N                  - Signal Matching for North electrode
 * gestic_param_afeRxAtt_E                  - Signal Matching for East electrode
 * gestic_param_afeRxAtt_C                  - Signal Matching for Center electrode
 * gestic_param_channelmapping_S            - Physical channel for South electrode
 * gestic_param_channelmapping_W            - Physical channel for West electrode
 * gestic_param_channelmapping_N            - Physical channel for North electrode
 * gestic_param_channelmapping_E            - Physical channel for East electrode
 * gestic_param_channelmapping_C            - Physical channel for Center electrode
 * gestic_param_dspCalOpMode                - Calibration Operation Mode Flags.
 *                                            See <gestic_set_auto_calibration>
 * gestic_param_transFreqSelect             - Transmit Frequency Selection.
 *                                            See <gestic_select_frequencies>
 * gestic_param_dspGestureMask              - Mask for Gesture Processing.
 *                                            See <gestic_set_enabled_gestures>
 * gestic_param_dspAirWheelConfig           - Enables/Disables AirWheel.
 *                                            See <gestic_set_air_wheel_enabled>
 * gestic_param_dspTouchConfig              - Enables/Disables Touch Detection.
 *                                            See <gestic_set_touch_detection>
 * gestic_param_dspApproachDetectionMode    - Enables/Disables Approach Detection.
 *                                            See <gestic_set_approach_detection>
 * gestic_param_dataOutputEnableMask        - Determines the continuous data output of the device.
 *                                            See <gestic_set_output_enable_mask>
 * gestic_param_dataOutputLockMask          - Determines the continuous data output of the device.
 *                                            See <gestic_set_output_enable_mask>
 * gestic_param_dataOutputRequestMask       - Determines the data output for next message.
 * gestic_param_dataOutputGestureInProgress - Enables/Disables output of
 *                                            gestures in progress
 */
typedef enum {
    gestic_param_trigger = 0x1000,
    gestic_param_makePersistent = 0xFF00,
    gestic_param_afeRxAtt_S = 0x50,
    gestic_param_afeRxAtt_W = 0x51,
    gestic_param_afeRxAtt_N = 0x52,
    gestic_param_afeRxAtt_E = 0x53,
    gestic_param_afeRxAtt_C = 0x54,
    gestic_param_channelmapping_S = 0x65,
    gestic_param_channelmapping_W = 0x66,
    gestic_param_channelmapping_N = 0x67,
    gestic_param_channelmapping_E = 0x68,
    gestic_param_channelmapping_C = 0x69,
    gestic_param_dspCalOpMode = 0x80,
    gestic_param_transFreqSelect = 0x82,
    gestic_param_dspGestureMask = 0x85,
    gestic_param_dspAirWheelConfig = 0x90,
    gestic_param_dspTouchConfig = 0x97,
    gestic_param_dspApproachDetectionMode = 0x97,
    gestic_param_dataOutputEnableMask = 0xA0,
    gestic_param_dataOutputLockMask = 0xA1,
    gestic_param_dataOutputRequestMask = 0xA2,
    gestic_param_dataOutputGestureInProgress = 0xA3
} gestic_parameter_id_t;

typedef enum {
    gestic_trigger_calibration = 0x00,
    gestic_trigger_enterDeepSleep1 = 0x01,
    gestic_trigger_enterDeepSleep2 = 0x02
} gestic_trigger_id_t;

typedef enum {
    gestic_DataOutConfigMask_DSPStatus = 0x0001,
    gestic_DataOutConfigMask_GestureInfo = 0x0002,
    gestic_DataOutConfigMask_TouchInfo = 0x0004,
    gestic_DataOutConfigMask_AirWheelInfo = 0x0008,
    gestic_DataOutConfigMask_xyzPosition = 0x0010,
    gestic_DataOutConfigMask_NoisePower = 0x0020,
    gestic_DataOutConfigMask_ElectrodeConfiguration = 0x0700,
    gestic_DataOutConfigMask_CICData = 0x0800,
    gestic_DataOutConfigMask_SDData = 0x1000
} gestic_DataOutConfigMask_t;

typedef enum {
    gestic_SystemInfo_PositionValid = 0x01,
    gestic_SystemInfo_AirWheelValid = 0x02,
    gestic_SystemInfo_RawDataValid = 0x04,
    gestic_SystemInfo_NoisePowerValid = 0x08,
    gestic_SystemInfo_EnvironmentalNoise = 0x10,
    gestic_SystemInfo_Clipping = 0x20,
    gestic_SystemInfo_DSPRunning = 0x80
} gestic_SystemInfo_t;

/* Enum: gestic_param_category
 *
 * Enumeration of possible categories for use with <gestic_make_persistent>.
 *
 * gestic_afe_category    - AFE Category
 * gestic_dsp_category   - DSP Category
 * gestic_system_category - System Category
 */
typedef enum {
    gestic_afe_category = 0,
    gestic_dsp_category = 1,
    gestic_system_category = 2
} gestic_param_category;


/* ======== Section: Low Level Communication ======== */

/* Function: gestic_send_message
 *
 * Sends a message to the device and waits for a response.
 *
 * msg    - Pointer to the message to send
 * size   - The size of the message as set in msg
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * This function calls <wait_response> to wait for an response.
 * If <wait_response> returns an error this function retries up to three times
 * before it in turn returns an error to the caller.
 *
 * See also:
 *    <wait_response>, <gestic_set_param>, <gestic_trigger_action>
 */
GESTIC_API int CDECL gestic_send_message(gestic_t *gestic,
                                         void *msg,
                                         int size,
                                         int timeout);

/* Function: gestic_set_param
 *
 * Sends the instruction for updating a runtime-parameter to the device.
 *
 * param   - The code for the parameter to update
 * arg0    - First parameter specific argument
 * arg1    - Second parameter specific argument
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_send_message>, <gestic_update_stream_mask DEPRECATED>,
 *    <gestic_set_auto_calibration>, <gestic_select_frequencies>,
 *    <gestic_set_approach_detection>, <gestic_set_enabled_gestures>,
 *    <gestic_get_param>
 */
GESTIC_API int CDECL gestic_set_param(gestic_t *gestic,
                                      unsigned short param,
                                      unsigned int arg0,
                                      unsigned int arg1,
                                      int timeout);

/* Function: gestic_get_param
 *
 * Reads back a parameter from the device.
 *
 * param   - The code for the parameter to read
 * arg0    - Pointer where to store the first argument, could be NULL
 * arg1    - Pointer where to store the second argument, could be NULL
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * The read parameter arguments are stored in arg0 and arg1 if those pointers
 * are valid.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_param>
 */
GESTIC_API int CDECL gestic_get_param(gestic_t *gestic,
                                      unsigned short param,
                                      unsigned int *arg0,
                                      unsigned int *arg1,
                                      int timeout);

/* Function: gestic_trigger_action
 *
 * Sends the instruction for a specific action to the device.
 *
 * action  - The firmware-specific code for the action
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * The enumeration gestic_trigger_id_t provides the possible actions.
 *
 * See also:
 *    <gestic_send_message>, <gestic_force_calibration>
 */
GESTIC_API int CDECL gestic_trigger_action(gestic_t *gestic,
                                           unsigned short action,
                                           int timeout);


/* ======== Section: Data Access ======== */

/* Enumeration: gestic_data_mask_t
 *
 * Bitmask of retrievable data as requested by <gestic_set_output_enable_mask>.
 *
 * gestic_data_mask_dsp_info    - DSP Info like calibration and frequency,
 *                                accessible through <gestic_get_calibration>
 *                                and <gestic_get_frequency>
 * gestic_data_mask_gesture     - Gesture Info, accessible via
 *                                <gestic_get_gesture>
 * gestic_data_mask_touch       - Touch Info, accessible via <gestic_get_touch>
 * gestic_data_mask_airwheel    - AirWheel Info, not yet accessible
 * gestic_data_mask_position    - Position Info, accessible via
 *                                <gestic_get_position>
 * gestic_data_mask_noise_power - Noise Power, not yet accessible
 * gestic_data_mask_cic         - CIC Data, accessible via <gestic_get_cic>
 * gestic_data_mask_sd          - SD Data, accessible via <gestic_get_sd>
 *
 * See also:
 *    <gestic_set_output_enable_mask>
 */
typedef enum {
    gestic_data_mask_dsp_status = 0x0001,
    gestic_data_mask_gesture = 0x0002,
    gestic_data_mask_touch = 0x0004,
    gestic_data_mask_airwheel = 0x0008,
    gestic_data_mask_position = 0x0010,
    gestic_data_mask_noise_power = 0x0020,
    gestic_data_mask_cic = 0x0800,
    gestic_data_mask_sd = 0x1000,
    gestic_data_mask_all = 0x183F
} gestic_data_mask_t;

#ifndef GESTIC_NO_DATA_RETRIEVAL

/* Enumeration: gestic_gestures_t
 *
 * The gestures supported by the API
 *
 * gestic_gest_none  - No gesture detected
 * gestic_flick_w2e  - Flick from west to east detected
 * gestic_flick_e2w  - Flick from east to west was detected
 * gestic_flick_s2n  - Flick from south to north was detected
 * gestic_flick_n2s  - Flick from north to south was detected
 * gestic_circle_cw  - Clock-wise circle detected
 * gestic_circle_ccw - Counter-clock-wise circle detected
 */
typedef enum {
    gestic_gest_none    = 0x00,
    gestic_flick_w2e    = 0x01,
    gestic_flick_e2w    = 0x02,
    gestic_flick_s2n    = 0x03,
    gestic_flick_n2s    = 0x04,
    gestic_circle_cw    = 0x05,
    gestic_circle_ccw   = 0x06
} gestic_gestures_t;

/* Enumeration: gestic_gesture_flags_t
 *
 * Bitmask of gesture related flags:
 *
 * gestic_gesture_edge_flick  - The flick was actually an edge flick
 * gestic_gesture_in_progress - Gesture recognition is in progress.
 *                              Only set while recognizer is active and
 *                              gets reset once gesture got recognized.
 * gestic_gesture_flags_mask  - Mask of possible flags.
 */
typedef enum {
    gestic_gesture_edge_flick = 0x00010000,
    gestic_gesture_in_progress = 0x80000000,
    gestic_gesture_flags_mask = 0x80010000
} gestic_gesture_flags_t;

/* Enum: gestic_touch_flags_t
 *
 * Bitmask of gesture related flags
 *
 * gestic_touch_north  - Touch at north electrode detected
 * gestic_touch_east   - Touch at east electrode detected
 * gestic_touch_south  - Touch at south electrode detected
 * gestic_touch_west   - Touch at west electrode detected
 * gestic_touch_center - Touch at center electrode detected
 * gestic_touch_mask   - Mask for detected touchs
 */
typedef enum {
    gestic_touch_north = 0x04,
    gestic_touch_east = 0x08,
    gestic_touch_south = 0x01,
    gestic_touch_west = 0x02,
    gestic_touch_center = 0x10,
    gestic_touch_mask = 0x1F
} gestic_touch_flags_t;

typedef enum {
    gestic_tap_north = 0x0080,
    gestic_tap_east = 0x0100,
    gestic_tap_south = 0x0020,
    gestic_tap_west = 0x0040,
    gestic_tap_center = 0x0200,
    gestic_double_tap_north = 0x1000,
    gestic_double_tap_east = 0x2000,
    gestic_double_tap_south = 0x0400,
    gestic_double_tap_west = 0x0800,
    gestic_double_tap_center = 0x4000,
    gestic_single_tap_mask = 0x03E0,
    gestic_double_tap_mask = 0x7C00,
    gestic_tap_mask = 0x7FE0
} gestic_tap_flags_t;

/* Enumeration: gestic_calib_reason_t
 *
 * Bitmask of possible reasons for calibrations
 *
 * gestic_forced_calib     - Enforced with <gestic_force_calibration>
 * gestic_startup_calib    - Triggered during startup
 * gestic_gesture_calib    - Triggered after gesture
 * gestic_negative_calib   - Triggered to compensate negative values
 * gestic_idle_calib       - Triggered to compensate background noise
 * gestic_invalidity_calib - Triggered as signals exceeded their valid range
 * gestic_dsp_forced_calib - Internally triggered calibration
 */
typedef enum {
    gestic_forced_calib = 0x0002,
    gestic_startup_calib = 0x0004,
    gestic_gesture_calib = 0x0008,
    gestic_negative_calib = 0x0010,
    gestic_idle_calib = 0x0020,
    gestic_invalidity_calib = 0x0040,
    gestic_dsp_forced_calib = 0x0080,
} gestic_calib_reason_t;

/* Structure: gestic_signal_t 
 *
 * Contains data for all channels of one type of signal.
 *
 * channel - The indvidual channels
 *
 * The channels are typically in the order
 * (south, west, north, east, center).
 *
 * See also:
 *    <gestic_get_cic>, <gestic_get_sd>
 */
typedef struct {
    float channel[5];
} gestic_signal_t;

/* Structure: gestic_position_t
 *
 * Contains position data that could be accessed with <gestic_get_position>.
 *
 * x - Ranges from 0 (west) to 65535 (east)
 * y - Ranges from 0 (south) to 65535 (north)
 * z - Ranges from 0 (touching) to 65535 (hand away)
 */
typedef struct {
    int x, y, z;
    int reserved;
} gestic_position_t;

/* Structure: gestic_gesture_t
 *
 * Contains information about gestures
 *
 * gesture    - The type of the gesture (See <gestic_gesture_t>)
 * flags      - Flags for the gesture (See <gestic_gesture_flags_t>)
 * last_event - Count of samples since the last detected gesture
 *
 * gesture will be set only once. It will be reset to <gestic_gest_none> by the
 * next call to <gestic_data_stream_update> if no subsequent gesture occured.
 *
 * Gesture data could be accessed with <gestic_get_gesture>.
 *
 * Note:
 *    last_event is based on the TimeStamp field of SENSOR_DATA_OUTPUT message.
 *    To be accurate the device isn't allowed to sleep and has to send at least
 *    one such message per second.
 *
 * Note:
 *    internally last_event is set to the count of samples between start-up
 *    and last gesture.
 */
typedef struct {
    gestic_gestures_t gesture;
    gestic_gesture_flags_t flags;
    int last_event;
} gestic_gesture_t;

/* Struct: gestic_touch_t
 *
 * Contains information about touch events.
 *
 * flags                  - Flags for the touch (See <gestic_touch_flags_t>)
 * last_event             - Count of samples since last touch event
 * tap_flags              - Flags for tap events
 * last_tap_event         - Count of samples since last tap event
 * last_touch_event_start - Count of samples since last touch event started
 *
 * Note:
 *    The *last_event fields are based on the TimeStamp field of
 *    SENSOR_DATA_OUTPUT message.
 *    To be accurate the device isn't allowed to sleep and has to send at least
 *    one such message per second.
 *
 * Note:
 *    internally the *last_event fiels are set to the count of samples between
 *    start-up and last gesture.
 */
typedef struct {
    gestic_touch_flags_t flags;
    int last_event;
    gestic_tap_flags_t tap_flags;
    int last_tap_event;
    int last_touch_event_start;
} gestic_touch_t;

/* Struct: gestic_air_wheel_t
 *
 * Contains information about AirWheel events.
 *
 * counter    - Counter which indicates how far the AirWheel rotation has
 *              progressed. Incrementing values indicate a clockwise rotation.
 *              Decrementing values indicate counter clockwise rotation.
 *              An increment of 32 approximates one full rotation.
 * active     - Boolean value indicating whether AirWheel is currently detected
 * last_event - Count of samples since last change of active
 *
 * Note:
 *    last_event is based on the TimeStamp field of SENSOR_DATA_OUTPUT message.
 *    To be accurate the device isn't allowed to sleep and has to send at least
 *    one such message per second.
 *
 * Note:
 *    internally last_event is set to the count of samples between start-up
 *    and last gesture.
 */
typedef struct {
    int counter;
    int active;
    int last_event;
} gestic_air_wheel_t;

/* Structure: gestic_calib_t
 *
 * Contains information about calibrations.
 *
 * reason     - Reason of the calibration as OR-combination of
 *              <gestic_calib_reason_t>.
 * last_event - Count of samples since last calibration
 *
 * reason will be set only once. It will be reset to 0 by subsequent calls to
 * <gestic_data_stream_update> when no further calibration occurs.
 *
 * Calibration data could be accessed with <gestic_get_calibration>.
 * Calibrations are updated when gestic_data_mask_sd is used.
 *
 * See also:
 *    <gestic_data_stream DEPRECATED>, <gestic_set_auto_calibration>,
 *    <gestic_force_calibration>
 *
 * Note:
 *    last_event is based on the TimeStamp field of SENSOR_DATA_OUTPUT message.
 *    To be accurate the device isn't allowed to sleep and has to send at least
 *    one such message per second.
 *
 * Note:
 *    internally last_event is set to the count of samples between start-up
 *    and last gesture.
 */
typedef struct {
    gestic_calib_reason_t reason;
    int last_event;
} gestic_calib_t;

/* Structure: gestic_freq_t
 *
 * Contains information about the working frequency.
 *
 * frequency    - The currently active frequency in kHz
 * freq_changed - Boolean value set to 1 when last frequency changed during
 *                last call to <gestic_data_stream_update>
 * last_event   - Count of samples since last calibration
 *
 * Frequency data could be accessed with <gestic_get_frequency>.
 * Frequency is always updated when any data is retrieved.
 *
 * See also:
 *    <gestic_data_stream DEPRECATED>, <gestic_select_frequencies>
 *
 * Note:
 *    last_event is based on the TimeStamp field of SENSOR_DATA_OUTPUT message.
 *    To be accurate the device isn't allowed to sleep and has to send at least
 *    one such message per second.
 *
 * Note:
 *    internally last_event is set to the count of samples between start-up
 *    and last gesture.
 */
typedef struct {
    int frequency;
    int freq_changed;
    int last_event;
} gestic_freq_t;

/* Structure: gestic_noise_power_t
 *
 * Contains current noise value
 *
 * value - The value of the noise power
 * valid - Not zero when value is valid or zero otherwise
 *
 * Noise power data is accessed with <gestic_get_noise_power>.
 * Or directly via gestic->results.noise_power when using the
 * static API.
 */
typedef struct {
    float value;
    int valid;
} gestic_noise_power_t;

#endif

/* ======== Section: Data Retrieval ======== */

#ifndef GESTIC_NO_DATA_RETRIEVAL

/* Function: gestic_data_stream_update
 *
 * Retrieves the availabe data output from the device and updates the result
 * buffer.
 *
 * skipped - If a pointer to an integer is provided it will be updated with the
 *           count of skipped data-sets.
 *
 * Returns 0 on success or a negative <gestic_error_t> code if no new data is
 * available or the communication is broken.
 *
 * It is possible that multiple updates were received during calls to different
 * functions. In this case the buffer will contain an accumulation of received
 * data after this call and skipped will be set to the amount of skipped
 * data-sets.
 *
 * The fetched data could be accessed with the gestic_get_* functions.
 *
 * Note:
 *    <gestic_data_stream_update> may process only fragments of the incoming
 *    data. It is therefore usefull to call it repeatedly until no more data are
 *    available to keep it in sync with the device.
 *
 * See also:
 *    <gestic_get_cic>, <gestic_get_sd>,
 *    <gestic_get_position>, <gestic_get_gesture>, <gestic_get_calibration>
 */
GESTIC_API int CDECL gestic_data_stream_update(gestic_t *gestic, int *skipped);

#endif

/* ======== Section: Real time control (RTC) ======== */

#ifndef GESTIC_NO_FW_VERSION

/* Function: gestic_query_fw_version
 *
 * Reads the library version information.
 *
 * version  - Pointer to a buffer receiving the version string
 * v_length - Maximal length of version in bytes including the terminating 0.
 * timeout  - Timeout in milliseconds to wait for a response
 *
 * Note:
 *    The complete firmware-version-string has a maximum size of 120
 *    characters.
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 */
GESTIC_API int CDECL gestic_query_fw_version(gestic_t *gestic,
                                             char *version,
                                             int v_length,
                                             int timeout);

#endif

#ifndef GESTIC_NO_DATA_RETRIEVAL

/* Function: gestic_set_output_enable_mask
 *
 * Changes the data ouput of the GestIC device.
 *
 * flags   - Which data to be streamed as a combination of
 *           <gestic_data_mask_t>-values
 * locked  - Which of the streamed data have to be always included in the
 *           message even when they have no valid data.
 * mask    - The mask of which data should get configured by this call
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_output_enable_mask>
 */
GESTIC_API int CDECL gestic_set_output_enable_mask(gestic_t *gestic,
                                                   gestic_data_mask_t flags,
                                                   gestic_data_mask_t locked,
                                                   gestic_data_mask_t mask,
                                                   int timeout);

/* Function: gestic_get_output_enable_mask
 *
 * Reads back the mask of data output set in the GestIC device.
 *
 * flags   - Pointer to variable receiving which data is to be streamed as a
 *           combination of <gestic_data_mask_t>-values. May be 0.
 * locked  - Pointer to variable receiving which of the streamed data has to
 *           be included always even though they are not valid. May be 0.
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of flags and locked is not defined.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_output_enable_mask>
 */
GESTIC_API int CDECL gestic_get_output_enable_mask(gestic_t *gestic,
                                                   gestic_data_mask_t *flags,
                                                   gestic_data_mask_t *locked,
                                                   int timeout);

#endif

#ifndef GESTIC_NO_RTC

/* Enumeration: gestic_frequencies_t
 *
 * Bitmask for working frequencies that could be enabled.
 * The actual frequencies depend on the library version in place and are listed
 * in the libraries manual.
 * The currently selected frequency may be fetched with <gestic_get_frequency>.
 *
 * gestic_freq1 - Frequency 1 enabled (highest)
 * gestic_freq2 - Frequency 2 enabled
 * gestic_freq3 - Frequency 3 enabled
 * gestic_freq4 - Frequency 4 enabled
 * gestic_freq5 - Frequency 5 enabled (lowest)
 * gestic_all_freq - All frequencies enabled
 *
 * See also:
 *    <gestic_select_frequencies>
 */
typedef enum {
    gestic_freq1 = 0x01,
    gestic_freq2 = 0x02,
    gestic_freq3 = 0x04,
    gestic_freq4 = 0x08,
    gestic_freq5 = 0x10,
    gestic_all_freq = 0x1F
} gestic_frequencies_t;

/* Function: gestic_set_auto_calibration
 *
 * Enables automatic calibration if enabled is set to a value other than 0.
 *
 * enabled - Boolean value whether calibration is enabled
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 if the change was succesfull or a negative value on failure.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_auto_calibration>, <gestic_force_calibration>,
 *    <gestic_get_calibration>
 */
GESTIC_API int CDECL gestic_set_auto_calibration(gestic_t *gestic,
                                           int enabled,
                                           int timeout);

/* Function: gestic_get_auto_calibration
 *
 * Reads back whether automatic calibration is enabled.
 *
 * enabled - Pointer to variable that receives the state of auto-calibration
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of enabled will not be changed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_auto_calibration>, <gestic_force_calibration>
 */
GESTIC_API int CDECL gestic_get_auto_calibration(gestic_t *gestic,
                                                 int *enabled,
                                                 int timeout);

/* Function: gestic_force_calibration
 *
 * Enforces a immediate calibration of the device.
 *
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 if the device acknoledged the calibration-request.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_auto_calibration>
 */
GESTIC_API int CDECL gestic_force_calibration(gestic_t *gestic, int timeout);

/* Function: gestic_select_frequencies
 *
 * Selects which working frequencies are allowed.
 *
 * frequencies - Allowed frequencies as a combination of <gestic_frequencies_t>
 * timeout     - Timeout in milliseconds to wait for a response
 *
 * Selects the allowed working frequencies for the device. The allowed
 * frequencies are passed in the argument frequencies as a bitwise-or
 * combination of <gestic_frequencies_t>-values.
 *
 * Returns 0 if the change was successfull.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 */
GESTIC_API int CDECL gestic_select_frequencies(gestic_t *gestic,
                                         gestic_frequencies_t frequencies,
                                         int timeout);

/* Function: gestic_set_approach_detection
 *
 * Enables approach detection for power saving.
 *
 * enabled - Boolean value whether approach detection is enabled
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 if the change was successfull.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_approach_detection>
 */
GESTIC_API int CDECL gestic_set_approach_detection(gestic_t *gestic,
                                             int enabled,
                                             int timeout);

/* Function: gestic_get_approach_detection
 *
 * Reads back whether approach detection is enabled.
 *
 * enabled - Pointer to a variable receiving the state of the approach detection
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of enabled will not be changed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_approach_detection>
 */
GESTIC_API int CDECL gestic_get_approach_detection(gestic_t *gestic,
                                                   int *enabled,
                                                   int timeout);

/* Function: gestic_set_enabled_gestures
 *
 * Makes certain gestures available for gesture recognition.
 *
 * gestures - Bitmask of gestures that are to be enabled
 * timeout  - Timeout in milliseconds to wait for a response
 *
 * Returns 0 if the enabled gestures was successfully changed.
 *
 * The individual gestures are represented as 1 shifted by the value of the
 * according <gestic_gestures_t>-constant. E.g. Horizontal flicks are
 * represented by (1 << gestic_flick_w2e) | (1 << gestic_flick_e2w)
 *
 * After a failure or no response within the timeout the request gest resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_enabled_gestures>, <gestic_get_gesture>
 */
GESTIC_API int CDECL gestic_set_enabled_gestures(gestic_t *gestic,
                                           int gestures,
                                           int timeout);

/* Function: gestic_get_enabled_gestures
 *
 * Reads back which gestures are currently available for gesture detection.
 *
 * gestures - Pointer to a variable receiving the bitmask of enabled gestures
 * timeout  - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of gestures will not be changed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_enabled_gestures>
 */
GESTIC_API int CDECL gestic_get_enabled_gestures(gestic_t *gestic,
                                                 int *gestures,
                                                 int timeout);

/* Function: gestic_set_touch_detection
 *
 * En-/Disables touch detection.
 *
 * enabled - Boolean value whether touch detection is enabled
 * timeout - Timeout in milliseconds to wait for response
 *
 * Returns 0 if the change was successfull.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_touch_detection>, <gestic_get_touch>
 */
GESTIC_API int CDECL gestic_set_touch_detection(gestic_t *gestic,
                                                int enabled,
                                                int timeout);

/* Function: gestic_get_touch_detection
 *
 * Reads back whether touch detection is enabled
 *
 * enabled - Pointer to a variable receiving the state of touch-detection
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of enabled will not be changed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_touch_detection>
 */
GESTIC_API int CDECL gestic_get_touch_detection(gestic_t *gestic,
                                                int *enabled,
                                                int timeout);

/* Function: gestic_set_air_wheel_enabled
 *
 * En-/Disables AirWheel.
 *
 * enabled - Boolean value whether AirWheel is enabled
 * timeout - Timeout in milliseconds to wait for response
 *
 * Returns 0 if the change was successfull.
 *
 * If AirWheel is enabled no circle-gestures could be detected.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_get_air_wheel_enabled>, <gestic_get_air_wheel>
 */
GESTIC_API int CDECL gestic_set_air_wheel_enabled(gestic_t *gestic,
                                                  int enabled,
                                                  int timeout);

/* Function: gestic_get_air_wheel_enabled
 *
 * Reads back whether the AirWheel-feature is enabled
 *
 * enabled - Pointer to a variable receiving the state of AirWheel-feature
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * If the request failed the content of enabled will not be changed.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * See also:
 *    <gestic_set_air_wheel_enabled>
 */
GESTIC_API int CDECL gestic_get_air_wheel_enabled(gestic_t *gestic,
                                                  int *enabled,
                                                  int timeout);

/* Function: gestic_make_persistent
 *
 * Stores current value for all parameters of a category to the flash.
 *
 * category - Parameter category which should be stored
 * timeout  - Timeout in milliseconds to wait for response
 *
 * Returns 0 if parameters where successfully stored or
 * the negative error-code on failure.
 *
 * After a failure or no response within the timeout the request gets
 * resend up to three times.
 *
 * See also:
 *    <gestic_set_param>, <gestic_param_category>
 */
GESTIC_API int CDECL gestic_make_persistent(gestic_t *gestic,
                                            gestic_param_category category,
                                            int timeout);

#endif

/* ======== Section: Flashing Firmware Libraries ======== */

#ifndef GESTIC_NO_FLASH

/* Enumeration: gestic_UpdateFunction_t
 *
 * The mode of the flash-session
 *
 * gestic_UpdateFunction_ProgramFlash - Records will be written to the flash
 * gestic_UpdateFunction_VerifyOnly   - Code will only be verified but not changed
 *                                      in flash.
 * gestic_UpdateFunction_Restart      - Only used internally for final restart.
 *                                      Don't provide this to the gestic_flash_*
 *                                      functions
 *
 * If a session gets started by calling <gestic_flash_begin> with
 * <gestic_UpdateFunction_VerifyOnly> no other value is allowed in subsequent
 * <gestic_flash_write> calls.
 *
 * If a session gets started with <gestic_UpdateFunction_ProgramFlash> it has to
 * be finished with <gestic_UpdateFunction_ProgramFlash> too in order to have a valid
 * firmware.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_begin>,
 *    <gestic_flash_write>
 */
typedef enum {
    gestic_UpdateFunction_ProgramFlash = 0,
    gestic_UpdateFunction_VerifyOnly = 1,
    gestic_UpdateFunction_Restart = 3
} gestic_UpdateFunction_t;

/* Structure: gestic_flash_block_t
 *
 * Contains one record of a firmware library image.
 *
 * address - The address of the record aligned to 128 bytes
 * length  - The length of the data packed in the record.
 *           This is NOT the size of the record itself.
 * data    - The 128-byte long content of the record.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_image_t>
 */
typedef struct {
    unsigned short address;
    unsigned char length;
    unsigned char data[128];
} gestic_flash_record_t;

/* Structure: gestic_flash_image_t
 *
 * Contains the data of a library image as required by <gestic_flash_image>
 *
 * record_count - The actual count of records
 * iv           - The initialization vector.
 * fw_version   - The version containing an ASCII-string
 * data         - The first of record_count records
 *
 * For instructions on how to obtain a gestic_flash_image_t instance see
 * <Firmware Version and Update>.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>
 */
typedef struct {
    int record_count;
    unsigned char iv[14];
    unsigned char fw_version[120];
    gestic_flash_record_t data[1];
} gestic_flash_image_t;

/* Function: gestic_flash_begin
 *
 * Starts a flash session to update the contained firmware-library
 *
 * session_id - A random session-id. Can be any value except 0.
 * iv         - The 14-byte long initialization vector.
 *              It is provided as part of the library-image.
 * mode       - The mode for this session
 * timeout    - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * This function resets the device and starts an session for updating the
 * contained firmware-library. The session has to be finished with
 * gestic_flash_end or another reset to cancel it.
 *
 * Note:
 *    If the mode is <gestic_UpdateFunction_VerifyOnly> subsequent calls to
 *    <gestic_flash_write> have to be called with the same mode.
 *    <gestic_flash_end> implicitely uses the same mode to finish the session.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_write>,
 *    <gestic_flash_end>
 */
GESTIC_API int CDECL gestic_flash_begin(gestic_t *gestic,
                                        unsigned int session_id,
                                        void *iv,
                                        gestic_UpdateFunction_t mode,
                                        int timeout);

/* Function: gestic_flash_write
 *
 * Writes one record during a flash session
 *
 * address - The address of the record, This is part of the library-image
 * length  - The size of the data packed in the record.
 *           This is NOT the size of the record
 * record  - The 128-byte long data of the record
 * mode    - The update-mode for this record.
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * The address is aligned to 128 bytes and the record is always 128-bytes long
 * independant of size-argument. If the provided record is less than 128-bytes
 * the rest of the record has to be filled with zeros.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * Note:
 *    If <gestic_flash_begin> was called with <gestic_UpdateFunction_VerifyOnly>
 *    this mode has to be used in the <gestic_flash_write> too.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_begin>,
 *    <gestic_flash_end>
 */
GESTIC_API int CDECL gestic_flash_write(gestic_t *gestic,
                                        unsigned short address,
                                        unsigned char length,
                                        unsigned char *record,
                                        gestic_UpdateFunction_t mode,
                                        int timeout);

/* Function: gestic_flash_end
 *
 * Finishes the flash process and ends the flash session.
 *
 * version - The 120-byte long library-version containing an ASCII-string
 *           It is provided as part of the the firmware image
 * timeout - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * This function ends the flash session by finally writing the version-string
 * and resetting the device. After this function returns the device
 * needs some time until it finishes the reset and subsequent start-up
 * procedure. <gestic_wait_for_version_info> waits until the
 * wanted start-up procedure sends the firmware version message which
 * marks that the library loader is ready (first call) or the library itself
 * is ready (second call).
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * Note:
 *    The final reset triggered by <gestic_flash_end> may be delayed by
 *    the firmware for a few seconds. This could be shortened by a manual
 *    <gestic_reset> about 200 milliseconds after <gestic_flash_end> succeeded.
 *
 * Important:
 *    After flashing of loader-updates the chip requires some time (about 20-25 seconds)
 *    to complete the update before it is ready for flashing the firmware.
 *    <gestic_flash_wait_loader_updated> does the required check.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_begin>,
 *    <gestic_flash_write>
 */
GESTIC_API int CDECL gestic_flash_end(gestic_t *gestic,
                                      unsigned char *version,
                                      int timeout);

/* Function: gestic_flash_image
 *
 * Does the whole process of writing the image to device including the resets.
 *
 * session_id - A random session-id. Can be any value except 0.
 * image      - Ptr to a <gestic_flash_image_t>-structure containing the image.
 * timeout    - Timeout in milliseconds to wait for a response
 *
 * Returns 0 on success or a negative value when the request failed.
 *
 * Runs the whole process of flashing the firmware library on the provided
 * image. This includeds the initial and final restarts of the device.
 * If the image is not available in a <gestic_flash_image_t>-structure the
 * functions <gestic_flash_begin>, <gestic_flash_write> and <gestic_flash_end>
 * could be used directly.
 *
 * After a failure or no response within the timeout the request gets resend
 * up to three times.
 *
 * Important:
 *    After flashing of loader-updates the chip requires some time to
 *    complete the update before it is ready for flashing the firmware.
 *    <gestic_flash_wait_loader_updated> does the required check.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_begin>, <gestic_flash_write>,
 *    <gestic_flash_end>
 */
GESTIC_API int CDECL gestic_flash_image(gestic_t *gestic,
                                        unsigned int session_id,
                                        gestic_flash_image_t *image,
                                        gestic_UpdateFunction_t mode,
                                        int timeout);

/* Function: gestic_flash_wait_loader_updated
 *
 * Waits until loader update is finished.
 *
 * timeout - The timeout in milliseconds to wait until finished.
 *           Ensure that value is big enough that update could be finished.
 *
 * Return 0 on success or a negative value when request failed.
 *
 * Call this function after a loader got uploaded to the chip to ensure
 * that it was completely processed and stored in flash before starting
 * the firmware-update.
 *
 * Important:
 *    Only call this function after a loader-upload.
 *    Otherwise it will fail and return an error.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_flash_image>, <gestic_flash_end>
 */
GESTIC_API int CDECL gestic_flash_wait_loader_updated(gestic_t *gestic,
                                                      int timeout);

/* Function: gestic_wait_for_version_info
 *
 * After a reset waits until version info message was received.
 *
 * timeout - The timeout in milliseconds to wait until finished.
 *
 * Return 0 on success or the negative <gestic_error_t> code
 * when request failed.
 *
 * This function waits after a reset until the library loader
 * version info was received. At this point the loader is ready
 * to receive instructions.
 *
 * Note:
 *    <gestic_flash_begin> already contains the reset and waiting
 *    until the loader is ready. There is no need to do an extra
 *    call to <gestic_reset> and <gestic_wait_for_version_info>
 *
 * A second subsequent call will wait until the library version
 * information was received. At that point the GestIC library is
 * completely operational.
 *
 * Note:
 *    This function is only intended to be used for waiting until
 *    the different start-up states are finished.
 *
 * See also:
 *    <Firmware Version and Update>, <gestic_reset>
 */
GESTIC_API int CDECL gestic_wait_for_version_info(gestic_t *gestic,
                                                  int timeout);
#endif

/* Include the dynamic API that has to be used when accessing
 * the GestIC-library via dynamic linking.
 * This is required because the exact configuration of the dynamic
 * library and therefore the structure of gestic_t is unknown.
 */
#if defined(GESTIC_API_DYNAMIC) || defined(GESTIC_HAS_DYNAMIC)
#include "gestic_dynamic.h"
#endif

/* Include the static part of the API that is not accessible
 * to applications linking dynamically against the GestIC-library.
 */
#if defined(GESTIC_API_EXPORT) || !defined(GESTIC_API_DYNAMIC)
#include "gestic_static.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* GESTIC_API_H */
