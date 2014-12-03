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
#ifndef GESTIC_CUSTOM_H
#define GESTIC_CUSTOM_H

/* This file is included by gestic_api.h and should not be included directly.
 *
 * As most IDEs don't take definitions from the file that is including the
 * header into consideration when parsing a header this file in turn
 * includes gestic_api.h to help the parser find the correct definitions.
 */
#ifndef GESTIC_API_H
#include "gestic_api.h"
#endif

#include "HardwareProfile.h"

/* Disable features that are not needed by this application */

#define GESTIC_NO_DATA_RETRIEVAL
#define GESTIC_NO_RTC
#define GESTIC_NO_LOGGING

/* Notify GestIC-API that we provide a custom IO-implementation */

#ifdef GESTIC_IO
#undef GESTIC_IO
#endif
#define GESTIC_IO GESTIC_IO_CUSTOM

/* Definition of custom IO-implementation */

typedef enum {
    GestICDev_GetI2CPort,
    GestICDev_GetI2CAddress,
    GestICDev_TsLine_Init,
    GestICDev_TsLine_Get,
    GestICDev_TsLine_Assert,
    GestICDev_TsLine_Release,
    GestICDev_Reset_Init,
    GestICDev_Reset_Assert,
    GestICDev_Reset_Release
} gestic_dev_cmd_t;

typedef int (*gestic_dev_handler)(gestic_dev_cmd_t cmd);

typedef struct {
    I2C_MODULE I2cPort;
    I2C_7_BIT_ADDRESS I2cSlaveAddr;
    gestic_dev_handler device;
    int connected;
    unsigned char read_buffer[160];
} gestic_io_t;

void gestic_set_dev_handler(gestic_t *gestic, gestic_dev_handler device);

int gestic_dev0_handler(gestic_dev_cmd_t cmd);

#define GESTIC_CONNECTED(gestic) ((gestic)->io.connected)

#include <assert.h>
#define GESTIC_ASSERT(X) assert(X)

#endif /* GESTIC_CUSTOM_H */
