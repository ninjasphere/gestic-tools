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

#include <gestic_api.h>


/* ======== Configuration Bits ======== */

#ifdef __PIC32MX__
    #pragma config UPLLEN   = ON            // USB PLL Enabled
    #pragma config FPLLMUL  = MUL_15        // PLL Multiplier
    #pragma config UPLLIDIV = DIV_2         // USB PLL Input Divider
    #pragma config FPLLIDIV = DIV_2         // PLL Input Divider
    #pragma config FPLLODIV = DIV_1         // PLL Output Divider
    #pragma config FPBDIV   = DIV_1         // Peripheral Clock divisor
    #pragma config FWDTEN   = OFF           // Watchdog Timer
    #pragma config WDTPS    = PS1           // Watchdog Timer Postscale
    //#pragma config FCKSM    = CSDCMD        // Clock Switching & Fail Safe Clock Monitor
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

#include "HardwareProfile.h"

#define BUTTON_NO        0
#define BUTTON_RESET     1 /* SW1 */
#define BUTTON_PROGRAM   2 /* SW2 */
#define BUTTON_VERIFY    3 /* SW3 */

BYTE button = BUTTON_NO;

void sleep(int ms);

BYTE readbutton( void )
{
    if(GetHWButtonMCLR() == HW_BUTTON_PRESS)
    {
        /* SW1 */
        sleep(10);
        if(GetHWButtonMCLR() == HW_BUTTON_PRESS)
            return BUTTON_RESET;
    }
    else if(GetHWButtonProgram() == HW_BUTTON_PRESS)
    {
        /* SW2 */
        sleep(10);
        if(GetHWButtonProgram() == HW_BUTTON_PRESS)
            return BUTTON_PROGRAM;
    }
    else if(GetHWButtonVerify() == HW_BUTTON_PRESS)
    {
        /* SW3 */
        sleep(10);
        if(GetHWButtonVerify() == HW_BUTTON_PRESS)
            return BUTTON_VERIFY;
    }
    else
    {
        return BUTTON_NO;
    }
}

// Forward declaration of Library and Loader
// NOTE:
//    The structure of those variables is compatible with gestic_flash_image_t
//    Therefore we declare them here directly as gestic_flash_image_t to avoid
//    a redeclaration of the type and later cast
extern gestic_flash_image_t Library;
extern gestic_flash_image_t Loader;

int main(void)
{
    gestic_t gestic;
    char fwversion[120];

    SYSTEMConfigWaitStatesAndPB( GetSystemClock() );
    CheKseg0CacheOn();

    //Initialize the hardware switch
    HardwareButtonInit();
    //Initialize the hardware LEDs
    SetLEDDirection();
    TurnLEDAllOff();

    /* Initialize all variables and resources of gestic */
    gestic_initialize(&gestic);

    /* Open connection to device */
    gestic_set_dev_handler(&gestic, gestic_dev0_handler);
    gestic_open(&gestic);

    /* Fetch current firmware version
     *
     * This could be used for debugging or to implement version specific logic
     */
    if(gestic_query_fw_version(&gestic, fwversion, sizeof(fwversion), 100) != 0)
        TurnLEDOn(LED_FAIL); /* LED1 */

    while(1)
    {
        // Turn only ready LED on
        button = readbutton();

        switch(button)
        {
        case BUTTON_PROGRAM: /* SW2 */
            {
                const unsigned int session_id = 1; /* Random value */
                int error = 0;

                TurnLEDOff(LED_PASS); /* LED3 */
                TurnLEDOff(LED_FAIL); /* LED1 */

                /* Flash the loader-updater image */
                error = gestic_flash_image(&gestic, session_id,
                                           (gestic_flash_image_t*)&Loader,
                                           gestic_UpdateFunction_ProgramFlash, 100);

                if(!error) {
                    /* Wait until loader-updater updated library-loader */
                    error = gestic_flash_wait_loader_updated(&gestic, 20000);
                }

                if(!error) {
                    /* Flash the library image */
                    error = gestic_flash_image(&gestic, session_id, &Library,
                                               gestic_UpdateFunction_ProgramFlash, 100);
                }

                /* Change LEDs based on result */
                if(error)
                    TurnLEDOn(LED_FAIL); /* LED1 */
                else
                    TurnLEDOn(LED_PASS); /* LED3 */
                break;
            }
        case BUTTON_VERIFY: /* SW3 */
            {
                const unsigned int session_id = 2; /* Random value */
                int error = 0;

                TurnLEDOff(LED_PASS); /* LED3 */
                TurnLEDOff(LED_FAIL); /* LED1 */

                /* Verify library image */
                error = gestic_flash_image(&gestic, session_id, &Library,
                                           gestic_UpdateFunction_VerifyOnly, 100);

                /* Change LEDs based on result */
                if(error)
                    TurnLEDOn(LED_FAIL); /* LED1 */
                else
                    TurnLEDOn(LED_PASS); /* LED3 */
                break;
            }
        case BUTTON_RESET: /* SW1 */
            {
                TurnLEDAllOn();
                sleep(100);
                TurnLEDAllOff();
                sleep(100);
                TurnLEDAllOn();
                sleep(100);
                TurnLEDAllOff();
                gestic_reset(&gestic);
                break;
            }
        }
    }
    return 0;
}
