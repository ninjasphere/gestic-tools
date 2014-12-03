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

#ifndef GESTIC_NO_DATA_RETRIEVAL

unsigned char systemModeElectrodes[] = { 4, 5 };

void gestic_handle_data_output(gestic_t *gestic,
                               const unsigned char *data)
{
    int dataOutputConfig = GET_U16(data + 4);
    unsigned char timestamp = GET_U8(data + 6);
    int systemInfo = GET_U8(data + 7);
    const unsigned char *cursor = data + 8;
    int systemMode = (dataOutputConfig & gestic_DataOutConfigMask_ElectrodeConfiguration) >> 8;
    int electrodeCount = systemModeElectrodes[systemMode];
    int increment;

    gestic_input_data_t *dest = &gestic->internal;

    int airWheelActive = (systemInfo & gestic_SystemInfo_PositionValid) ? 1 : 0;

#ifdef GESTIC_SYNC_THREADING
    /* Synchronize against interaction with the internal buffer
     * from the Application-Layer
     */
    GESTIC_SYNC_LOCK(gestic->io_sync);
#endif

    /* NOTE Overflows should not be a problem as long as more
     * than one message per 256 samples is received.
     * Otherwise this algorithm will loose precision but should
     * still work as counts are only compared for equality
     * or via substraction.
     */
    increment = (unsigned char)(timestamp -
                                gestic->last_time_stamp);
    dest->frame_counter += increment ? increment : 1;
    gestic->last_time_stamp = timestamp;

    if(dataOutputConfig & gestic_DataOutConfigMask_DSPStatus) {
        int calibration = GET_U8(cursor);
        int frequency = GET_U8(cursor + 1);
        if(calibration != 0) {
            dest->calib.reason = calibration;
            dest->calib.last_event = dest->frame_counter;
        }
        if(frequency != dest->frequency.frequency) {
            dest->frequency.frequency = frequency;
            dest->frequency.freq_changed = 1;
            dest->frequency.last_event = dest->frame_counter;
        }
        cursor += 2;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_GestureInfo) {
        int gestureInfo = GET_U32(cursor);
        int raw = gestureInfo & 0xFF;
        int gesture = raw > 1 ? raw - 1 : 0;
        if(gesture) {
            dest->gesture.gesture = gesture;
            dest->gesture.flags = gestureInfo & gestic_gesture_flags_mask;
            dest->gesture.last_event = dest->frame_counter;
        }
        cursor += 4;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_TouchInfo) {
        int info = GET_U32(cursor);
        int touch = info & gestic_touch_mask;
        int tap = info & gestic_tap_mask;
        if((gestic_touch_flags_t)touch != dest->touch.flags) {
            dest->touch.flags = touch;
            dest->touch.last_event = dest->frame_counter;
            dest->touch.last_touch_event_start = dest->frame_counter - ((info & 0xFF0000) >> 16);
        }
        if(tap) {
            dest->touch.tap_flags = tap;
            dest->touch.last_tap_event = dest->frame_counter;
        }
        cursor += 4;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_AirWheelInfo) {
        if(airWheelActive) {
            int counter = GET_U8(cursor);
            if(counter != dest->air_wheel.counter)
                dest->air_wheel.counter = counter;
        }
        cursor += 2;
    }
    if(airWheelActive != dest->air_wheel.active) {
        dest->air_wheel.active = airWheelActive;
        dest->air_wheel.last_event = dest->frame_counter;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_xyzPosition) {
        if(systemInfo & gestic_SystemInfo_PositionValid) {
            dest->pos.x = GET_U16(cursor);
            dest->pos.y = GET_U16(cursor+2);
            dest->pos.z = GET_U16(cursor+4);
        }
        cursor += 6;
    }
    dest->noise_power.valid = 0;
    if(dataOutputConfig & gestic_DataOutConfigMask_NoisePower) {
        if(systemInfo & gestic_SystemInfo_NoisePowerValid) {
            dest->noise_power.value = GET_F32(cursor);
            dest->noise_power.valid = 1;
        }
        cursor += 4;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_CICData) {
        if(systemInfo & gestic_SystemInfo_RawDataValid) {
            int i;
            for(i = 0; i < electrodeCount; ++i)
                dest->cic.channel[i] = GET_F32(cursor + 4*i);
            for(i = electrodeCount; i < 5; ++i)
                dest->cic.channel[i] = GESTIC_UNDEFINED_VALUE;
        }
        cursor += electrodeCount * 4;
    }
    if(dataOutputConfig & gestic_DataOutConfigMask_SDData) {
        if(systemInfo & gestic_SystemInfo_RawDataValid) {
            int i;
            for(i = 0; i < electrodeCount; ++i)
                dest->sd.channel[i] = GET_F32(cursor + 4*i);
            for(i = electrodeCount; i < 5; ++i)
                dest->sd.channel[i] = GESTIC_UNDEFINED_VALUE;
        }
        cursor += electrodeCount * 4;
    }

#ifdef GESTIC_SYNC_THREADING
    /* Release synchronization against Application-Layer */
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#endif
}

int gestic_data_stream_update(gestic_t *gestic, int *skipped) {
    int count;
    int error = GESTIC_NO_DATA;
    int last_counter, current_counter;

    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    last_counter = gestic->result.frame_counter;

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
    /* Synchronize against Hardware-Layer */
    GESTIC_SYNC_LOCK(gestic->io_sync);
#endif

    for(;;) {
        /* Check whether a full data-frame is available */
        current_counter = gestic->internal.frame_counter;
        count = current_counter - last_counter;
        if(count > 0)
            break;

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
        /* Temporarily release synchronization */
        GESTIC_SYNC_UNLOCK(gestic->io_sync);
#endif

        /* Receive and handle message */
        error = gestic_message_receive(gestic, NULL);

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
        /* Relock after message handling */
        GESTIC_SYNC_LOCK(gestic->io_sync);
#endif

        if(error != GESTIC_NO_ERROR)
            break;
    }

    if(count > 0) {
        gestic->result = gestic->internal;

        if(gestic->result.gesture.last_event <= last_counter) {
            gestic->result.gesture.gesture = 0;
            /* Reset flags except for the in-progress flag */
            gestic->result.gesture.flags &= gestic_gesture_in_progress;
        }
        gestic->result.gesture.last_event = current_counter - gestic->result.gesture.last_event;

        gestic->result.touch.last_event = current_counter - gestic->result.touch.last_event;
        if(gestic->result.touch.last_tap_event <= last_counter)
            gestic->result.touch.tap_flags = 0;
        gestic->result.touch.last_tap_event = current_counter - gestic->result.touch.last_tap_event;
        gestic->result.touch.last_touch_event_start = current_counter - gestic->result.touch.last_touch_event_start;

        gestic->result.air_wheel.last_event = current_counter -
                gestic->result.air_wheel.last_event;

        if(gestic->result.calib.last_event <= last_counter)
            gestic->result.calib.reason = 0;
        gestic->result.calib.last_event = current_counter - gestic->result.calib.last_event;

        if(gestic->result.frequency.last_event <= last_counter)
            gestic->result.frequency.freq_changed = 0;
        gestic->result.frequency.last_event = current_counter - gestic->result.frequency.last_event;

        if(skipped)
            *skipped = count - 1;

        error = GESTIC_NO_ERROR;
    }

#if defined(GESTIC_SYNC_INTERRUPT) || defined(GESTIC_SYNC_THREADING)
    /* Release synchronization against Hardware-Layer */
    GESTIC_SYNC_UNLOCK(gestic->io_sync);
#endif

    return error;
}

#endif
