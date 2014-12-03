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
#include <stdio.h>
#include <windows.h>

/* This demo shows how to use gestic_t directly.
 * This may be used to avoid the allocation on the heap.
 */
gestic_t gestic;

int main() {
    int i;

    /* Bitmask later used for starting a stream with SD- and position-data */
    const int stream_flags = gestic_data_mask_position | gestic_data_mask_sd;

    /* Convenience pointer to the internal data-buffers
     * Another way to get them would be to call gestic_get_*
     */
    const gestic_signal_t * const sd = &gestic.result.sd;
    const gestic_position_t * const pos = &gestic.result.pos;

    printf("GestIC-Stream-Demo version %s\n", GESTIC_VERSION_STRING);

    /* Initialize all variables and required resources of gestic */
    gestic_initialize(&gestic);

    /* Try to open a connection to the device */
    if(gestic_open(&gestic) < 0) {
        fprintf(stderr, "Could not open connection to device.\n");
        return -1;
    }

    /* Try to reset the device to the default state:
     * - Automatic calibration enabled
     * - All frequencies allowed
     * - Approach detection disabled
     */
    if(gestic_set_auto_calibration(&gestic, 1, 100) < 0 ||
       gestic_select_frequencies(&gestic, gestic_all_freq, 100) < 0 ||
       gestic_set_approach_detection(&gestic, 0, 100) < 0)
    {
        fprintf(stderr, "Could not reset device to default state.\n");
        return -1;
    }

    /* Set output-mask to the bitmask defined above and stream all data */
    if(gestic_set_output_enable_mask(&gestic, stream_flags, stream_flags,
                                     gestic_data_mask_all, 100) < 0)
    {
        fprintf(stderr, "Could not set output-mask for streaming.\n");
        return -1;
    }

    /* Listens for about 10 seconds to incoming messages */
    for(i = 0; i < 200; ++i) {
        /* Try to fetch stream-data until no more messages are available*/
        while(!gestic_data_stream_update(&gestic, 0)) {
            /* Output the position */
            printf("Position: %5d %5d %5d\n", pos->x, pos->y, pos->z);

            /* Output the SD-data */
            printf("SD-Data:  %5.0f %5.0f %5.0f %5.0f %5.0f\n",
                   sd->channel[0], sd->channel[1], sd->channel[2],
                    sd->channel[3], sd->channel[4]);
        }
        Sleep(50);
    }

    /* Close connection to device */
    gestic_close(&gestic);

    /* Release further resources that were used by gestic */
    gestic_cleanup(&gestic);

    return 0;
}
