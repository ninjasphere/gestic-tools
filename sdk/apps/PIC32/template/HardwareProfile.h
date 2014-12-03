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

#include <plib.h>

#endif  

