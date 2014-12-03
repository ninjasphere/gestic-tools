// HardwareProfile.h

#ifndef _HARDWARE_PROFILE_H_
#define _HARDWARE_PROFILE_H_

// ******************* CPU Speed defintions ************************************
//  This section is required by some of the peripheral libraries and software
//  libraries in order to know what the speed of the processor is to properly
//  configure the hardware modules to run at the proper speeds
// *****************************************************************************
#if defined( __PIC32MX__)

    #define USB_A0_SILICON_WORK_AROUND
    //#define RUN_AT_48MHZ
    //#define RUN_AT_24MHZ
    #define RUN_AT_60MHZ
    
    // Various clock values
    #if defined(RUN_AT_48MHZ)
        #define GetSystemClock()            48000000UL
        #define GetPeripheralClock()        48000000UL
        #define GetInstructionClock()       (GetSystemClock() / 2) ???
    #elif defined(RUN_AT_24MHZ)
        #define GetSystemClock()            24000000UL
        #define GetPeripheralClock()        24000000UL
        #define GetInstructionClock()       (GetSystemClock() / 2) ???
    #elif defined(RUN_AT_60MHZ)    
        #define GetSystemClock()            60000000UL
        #define GetPeripheralClock()        30000000UL  // Will be divided down
        #define GetInstructionClock()       (GetSystemClock() / 2) ???
    #else
        #error Choose a speed
    #endif        

#endif

/*********************************************************************
* GestIC IO
*********************************************************************/

#define DEV0_I2C_Port               I2C3
#define DEV0_I2C_Address            0x42

#define DEV0_TS_INIT                TRISEbits.TRISE8 = 1;
#define DEV0_TS                     PORTEbits.RE8 // RE8
#define DEV0_TS_ASSERT              LATEbits.LATE8 = 0; TRISEbits.TRISE8 = 0;
#define DEV0_TS_RELEASE             LATEbits.LATE8 = 1; TRISEbits.TRISE8 = 1;

#define DEV0_RESET_INIT             LATEbits.LATE6 = 1; TRISEbits.TRISE6 = 0;
#define DEV0_RESET_ASSERT           LATEbits.LATE6 = 0;
#define DEV0_RESET_RELEASE          LATEbits.LATE6 = 1;

/*********************************************************************
* IOS FOR THE SWITCHES (SIDE BUTTONS)
*********************************************************************/
typedef enum {
    HW_BUTTON_PRESS = 0,
    HW_BUTTON_RELEASE = 1
}HW_BUTTON_STATE;

#define HardwareButtonInit()        mPORTDSetPinsDigitalIn(BIT_6|BIT_7|BIT_13)
#define GetHWButtonMCLR()           (PORTDbits.RD6) /* SW1 */
#define GetHWButtonProgram()        (PORTDbits.RD7) /* SW2 */
#define GetHWButtonVerify()         (PORTDbits.RD13) /* SW3 */


/*********************************************************************
* IOS FOR THE LEDS
*********************************************************************/
#if defined(__PIC32MX__)
#include <plib.h>

/*********************************************************************
* LEDs
*********************************************************************/
typedef enum
{
    LED_BUSY, /* LED2 */
    LED_FAIL, /* LED1 */
    LED_PASS /* LED3 */
}USBSK_LED;

#define LED_IOPORT IOPORT_D
#define LED_FAIL_BIT BIT_0
#define LED_BUSY_BIT BIT_1
#define LED_PASS_BIT BIT_2


extern inline void __attribute__((always_inline)) SetLEDDirection(void)
{
    PORTSetPinsDigitalOut(LED_IOPORT, (LED_BUSY_BIT | LED_FAIL_BIT |
                                       LED_PASS_BIT));
}

extern inline void __attribute__((always_inline)) TurnLEDOn(USBSK_LED led)
{
    if(led == LED_BUSY) /* LED2 */
        PORTSetBits(LED_IOPORT, LED_BUSY_BIT);

    if(led == LED_FAIL) /* LED1 */
        PORTSetBits(LED_IOPORT, LED_FAIL_BIT);

    if(led == LED_PASS) /* LED3 */
        PORTSetBits(LED_IOPORT, LED_PASS_BIT);
}

extern inline void __attribute__((always_inline)) TurnLEDOff(USBSK_LED led)
{
    if(led == LED_BUSY) /* LED2 */
        PORTClearBits(LED_IOPORT, LED_BUSY_BIT);

    if(led == LED_FAIL) /* LED1 */
        PORTClearBits(LED_IOPORT, LED_FAIL_BIT);

    if(led == LED_PASS) /* LED3 */
        PORTClearBits(LED_IOPORT, LED_PASS_BIT);
}

extern inline void __attribute__((always_inline)) ToggleLED(USBSK_LED led)
{
    if(led == LED_BUSY) /* LED2 */
        PORTToggleBits(LED_IOPORT, LED_BUSY_BIT);

    if(led == LED_FAIL) /* LED1 */
        PORTToggleBits(LED_IOPORT, LED_FAIL_BIT);

    if(led == LED_PASS) /* LED3 */
        PORTToggleBits(LED_IOPORT, LED_PASS_BIT);
}

extern inline void __attribute__((always_inline)) TurnLEDAllOn(void)
{
    PORTSetBits(LED_IOPORT, LED_BUSY_BIT); /* LED2 */
    PORTSetBits(LED_IOPORT, LED_FAIL_BIT); /* LED1 */
    PORTSetBits(LED_IOPORT, LED_PASS_BIT); /* LED3 */
}

extern inline void __attribute__((always_inline)) TurnLEDAllOff(void)
{
    PORTClearBits(LED_IOPORT, LED_BUSY_BIT); /* LED2 */
    PORTClearBits(LED_IOPORT, LED_FAIL_BIT); /* LED1 */
    PORTClearBits(LED_IOPORT, LED_PASS_BIT); /* LED3 */
}

#endif // #ifdef (__PIC32MX__)

#endif  

