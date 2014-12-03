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

/* This demo shows how to use gestic_t directly.
 * This may be used to avoid the allocation on the heap.
 */

int main() {
    /* Create GestIC-Instance */
    gestic_t *gestic = gestic_create();
    if(gestic == NULL) {
        fprintf(stderr, "Could not create gestic_t instance.\n");
        return -1;
    }

    /* Initialize all variables and required resources of gestic */
    gestic_initialize(gestic);


    /* Try to open a connection to the device */
    if(gestic_open(gestic) < 0) {
        fprintf(stderr, "Could not open connection to device.\n");
        return -1;
    }

    /* TODO Use gestic */

    /* Close connection to device */
    gestic_close(gestic);

    /* Release further resources that were used by gestic */
    gestic_cleanup(gestic);
    gestic_free(gestic);

    return 0;
}
