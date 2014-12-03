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

// *****************************************************************************
// *****************************************************************************
// Configuration Bits
// *****************************************************************************
// *****************************************************************************

#ifdef __PIC32MX__
    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    #pragma config OSCIOFNC = OFF           // CLKO Enable
    #pragma config POSCMOD  = HS            // Primary Oscillator
    #pragma config IESO     = OFF           // Internal/External Switch-over
    #pragma config FSOSCEN  = OFF           // Secondary Oscillator Enable (KLO was off)
    #pragma config FNOSC    = PRIPLL        // Oscillator Selection
    #pragma config CP       = OFF           // Code Protect
    #pragma config BWP      = OFF           // Boot Flash Write Protect
    #pragma config PWP      = OFF           // Program Flash Write Protect
    #pragma config ICESEL   = ICS_PGx2      // ICE/ICD Comm Channel Select
#else
    #error Cannot define configuration bits.
#endif

#include <gestic_api.h>

int failure()
{
    /* Turn on all LEDs to signal an error */
    TurnLEDOn(LED_TOUCH);
    TurnLEDOn(LED_BUSY);
    TurnLEDOn(LED_GESTURE);

    /* Halt with an endless loop */
    while(1) { }
}

int main(void)
{
    /* Bitmask used for starting a stream with touch- and gesture-data */
    const int stream_flags = gestic_data_mask_gesture | gestic_data_mask_touch;
    gestic_t gestic;

    SYSTEMConfigWaitStatesAndPB( GetSystemClock() );
    CheKseg0CacheOn();

    /* Initialize the hardware LEDs */
    SetLEDDirection();
    TurnLEDAllOff();

    /* Initialize gestic */
    gestic_initialize(&gestic);

    /* Intialize I²C connection */
    gestic_set_dev_handler(&gestic, gestic_dev0_handler);
    gestic_open(&gestic);

    /* Try to reset the device to the default state:
     * - Automatic calibration enabled
     * - All frequencies allowed
     * - Approach detection enabled for power saving
     */
    if(gestic_set_auto_calibration(&gestic, 1, 100) < 0) {
        /* TODO Insert custom error handling */
        failure();
    }
    if(gestic_select_frequencies(&gestic, gestic_all_freq, 100) < 0) {
        /* TODO Insert custom error handling */
        failure();
    }
    if(gestic_set_approach_detection(&gestic, 1, 100) < 0) {
        /* TODO Insert custom error handling */
        failure();
    }

    /* Set output-mask to the bitmask defined above and stream all data */
    if(gestic_set_output_enable_mask(&gestic, stream_flags, stream_flags,
                                     gestic_data_mask_all, 100) < 0)
    {
        /* TODO Insert custom error handling */
        failure();
    }

    /* Stream incomming data */
    while(1)
    {
        if(gestic_data_stream_update(&gestic, 0) == 0) {
            if(gestic.result.touch.flags)
                TurnLEDOn(LED_TOUCH); /* LED1 */
            else
                TurnLEDOff(LED_TOUCH); /* LED1 */
            if(gestic.result.gesture.gesture > 0 && gestic.result.gesture.gesture <= 6) {
                ToggleLED(LED_GESTURE); /* LED3 */
            }
        }
    }
    return 0;
}
