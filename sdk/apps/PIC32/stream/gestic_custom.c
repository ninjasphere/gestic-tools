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

#include "HardwareProfile.h"

void sleep(int ms)
{
    int i;
    for( i = 0; i < 3200*ms; i++)
    {
        asm("NOP");
    }
}

int gestic_dev0_handler(gestic_dev_cmd_t cmd)
{
    switch(cmd) {
        case GestICDev_GetI2CPort:
            return DEV0_I2C_Port;
        case GestICDev_GetI2CAddress:
            return DEV0_I2C_Address;
        case GestICDev_TsLine_Init:
            DEV0_TS_INIT;
            break;
        case GestICDev_TsLine_Get:
            return DEV0_TS;
        case GestICDev_TsLine_Assert:
            DEV0_TS_ASSERT;
            break;
        case GestICDev_TsLine_Release:
            DEV0_TS_RELEASE;
            break;
        case GestICDev_Reset_Init:
            DEV0_RESET_INIT;
            break;
        case GestICDev_Reset_Assert:
            DEV0_RESET_ASSERT;
            break;
        case GestICDev_Reset_Release:
            DEV0_RESET_RELEASE;
            break;
    }
}

void gestic_set_dev_handler(gestic_t *gestic, gestic_dev_handler device)
{
    gestic->io.device = device;
}

int gestic_open(gestic_t *gestic)
{
    GESTIC_ASSERT(gestic);
    GESTIC_ASSERT(gestic->io.device);

    /* Maybe power up */

    gestic->io.connected = 1;
    gestic->io.I2cSlaveAddr.address = gestic->io.device(GestICDev_GetI2CAddress);
    gestic->io.I2cPort = gestic->io.device(GestICDev_GetI2CPort);

    gestic->io.device(GestICDev_TsLine_Init);
    gestic->io.device(GestICDev_Reset_Init);

    I2CEnable(gestic->io.I2cPort, FALSE);
    I2CConfigure(gestic->io.I2cPort, I2C_ENABLE_SLAVE_CLOCK_STRETCHING | I2C_ENABLE_HIGH_SPEED);
    //I2CSetFrequency(gestic->io.I2cPort, 40*1000*1000, 400*1000);
    I2CSetFrequency(gestic->io.I2cPort, 40*1000*1000, 25*1000);

    I2CEnable(gestic->io.I2cPort, TRUE);
    return GESTIC_NO_ERROR;
}

void gestic_close(gestic_t *gestic)
{
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));
    gestic->io.connected = 0;
    gestic->io.I2cSlaveAddr.address = 0;
    gestic->io.I2cPort = 0;
}

int gestic_reset(gestic_t *gestic)
{
    GESTIC_ASSERT(gestic && GESTIC_CONNECTED(gestic));

    gestic->io.device(GestICDev_Reset_Assert);
    sleep(5);
    gestic->io.device(GestICDev_Reset_Release);
    sleep(20);
    return 0;
}

static int transmit_byte(I2C_MODULE port, UINT8 data)
{
    while(!I2CTransmitterIsReady(port));

    if(I2CSendByte(port, data) == I2C_MASTER_BUS_COLLISION)
        return -1;

    while(!I2CTransmissionHasCompleted(port));

    return 0;
}

int transfer_start(I2C_MODULE port, I2C_7_BIT_ADDRESS address, UINT8 rw)
{
    I2C_STATUS status;
    int result = 0;

    while(!I2CBusIsIdle(port));

    if(I2CStart(port) != I2C_SUCCESS)
        return -1;

    do {
        status = I2CGetStatus(port);
    } while(!(status & I2C_START));

    /* Send address */
    address.rw = rw;
    result = transmit_byte(port, address.byte);
    if(result < 0)
        return result;

    if(!I2CByteWasAcknowledged(port))
        return -1;
    
    return result;
}
static int transfer_stop(I2C_MODULE port)
{
    I2C_STATUS status;

    I2CStop(port);

    do {
        status = I2CGetStatus(port);
    } while(!(status & I2C_STOP));
}

void* message_read(gestic_t *gestic, int *size)
{
    UINT8 RcvCount = 0;
    UINT8 writeIndex = 0;
    I2C_MODULE port = gestic->io.I2cPort;
    unsigned char *result = gestic->io.read_buffer;

    TurnLEDOn(LED_BUSY); /* LED2 */

    // Get TSLine
    if(gestic->io.device(GestICDev_TsLine_Get) == 1)
        result = NULL;

    if(result)
        gestic->io.device(GestICDev_TsLine_Assert);

    if(result && transfer_start(port, gestic->io.I2cSlaveAddr, 1))
        result = NULL;

    // Read the data from the desired address
    while(result) {
        UINT8 i2cbyte;

        /* Check for buffer overflow */
        if(I2CReceiverEnable(port, TRUE) == I2C_RECEIVE_OVERFLOW) {
            result = NULL;
            break;
        }

        while(!I2CReceivedDataIsAvailable(port));

        i2cbyte = I2CGetByte(gestic->io.I2cPort);
        result[writeIndex] = i2cbyte;
        writeIndex++;

        if(RcvCount == 0) {
            /* Set size if message fits into buffer */
            if(i2cbyte < sizeof(gestic->io.read_buffer))
                RcvCount = i2cbyte;
            else
                result = NULL;
        }

        /* In case of length == 0 or last byte (length == 1) send a NACK */
        if(RcvCount == 0 || RcvCount == 1)
            I2CAcknowledgeByte(port, FALSE);
        else
            I2CAcknowledgeByte(port, TRUE);

        while(!I2CAcknowledgeHasCompleted( gestic->io.I2cPort ));

        RcvCount--;
        if(RcvCount <= 0)
            break;
    }

    gestic->io.device(GestICDev_TsLine_Release);

    transfer_stop(gestic->io.I2cPort);
    
    TurnLEDOff(LED_BUSY); /* LED2 */

    if(result && size)
        *size = result[0];

    return result;
}

int gestic_message_receive(gestic_t *gestic, int *timeout)
{
    int result = GESTIC_NO_ERROR;
    int msg_size;
    void *msg;

    for(;;) {
        msg = message_read(gestic, &msg_size);
        if(msg) {
            gestic_message_handle(gestic, msg, msg_size);
            break;
        }

        if(!timeout || (*timeout <= 0)) {
            result = GESTIC_NO_DATA;
            break;
        }

        sleep(10);
        *timeout -= 10;
    }

    return result;
}

int gestic_message_write(gestic_t *gestic, void *msg, int size)
{
    I2C_MODULE port = gestic->io.I2cPort;
    const unsigned char * const buffer = (unsigned char*)msg;
    int result = 0;
    int i;

    GESTIC_ASSERT(buffer[0] == size);

    TurnLEDOn(LED_BUSY); /* LED2 */

    if(transfer_start(port, gestic->io.I2cSlaveAddr, 0))
        result = -1;

    for(i = 0; !result && i < size; ++i) {
        if(transmit_byte(port, buffer[i]) < 0)
            result = -1;
        
        if(!I2CByteWasAcknowledged(port))
            result = -1;
    }

    transfer_stop(port);

    TurnLEDOff(LED_BUSY); /* LED2 */
    
    return result;
}
