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

//#define ON_ERROR_RESUME_NEXT

#include <gestic_api.h>

#include <stdio.h>
#include <string.h>

#define stricmp strcasecmp

// Declaration of Library and Loader
// NOTE:
//    The structure of those variables is compatible with gestic_flash_image_t
//    Therefore we declare them here directly as gestic_flash_image_t to avoid
//    a redeclaration of the type and later cast
extern gestic_flash_image_t Library;
extern gestic_flash_image_t Loader;

int flash(gestic_t *gestic, gestic_flash_image_t *image, gestic_UpdateFunction_t mode)
{
    const unsigned int session_id = 1;
    int i = 0;
    gestic_flash_record_t *record;

    /* Start flash-session */
    if(gestic_flash_begin(gestic, session_id, image->iv, mode, 100) < 0) {
        fprintf(stderr, "Could not start flashing.\n");
        return -1;
    }

    /* Write records */
    for(i = 0; i < image->record_count; ++i) {
        record = image->data + i;
        printf("Writing to 0x%04x\n", record->address);
        if(gestic_flash_write(gestic, record->address, record->length,
                              record->data, mode, 100) < 0)
        {
            fprintf(stderr, "Could not flash data at address %d.\n",
                    record->address);
            return -1;
        }
    }

    /* Finish flash-session */
    if(gestic_flash_end(gestic, image->fw_version, 100) < 0) {
        fprintf(stderr, "Could not finish flashing.\n");
        return -1;
    }

    printf("Flashing completed successfully.\n");

    return 0;
}

int main()
{
    gestic_t *gestic = gestic_create();
    char version[120];
    char confirm[80];

    printf("GestIC-ENC-Flash Demo version %s\n\n", gestic_version_str());

    /* Initialize all variables and required resources of gestic */
    gestic_initialize(gestic);

    /* Try to open a connection to the device */
    if(gestic_open(gestic) < 0) {
        fprintf(stderr, "Could not open connection to device.\n");
        gestic_cleanup(gestic);
        gestic_free(gestic);
        return -1;
    }

    /* Read the running firmware version */
    if(gestic_query_fw_version(gestic, version, sizeof(version), 100) < 0) {
        fprintf(stderr, "Could not read running firmware version.\n");
#ifndef ON_ERROR_RESUME_NEXT
        gestic_cleanup(gestic);
        gestic_free(gestic);
        return -1;
#endif
    }
    /* The version should already include an terminating \0 but
     * as it was get via IO from external source we take extra care.
     */
    version[119] = '\0';

    /* Ask whether the firmware should really be flashed */
    printf("[ GestIC Library ]\n\n"
           "Currently running:\n%s\n\n"
           "To be flashed:\n%s\n\n"
           "[ GestIC Library Loader\n\n"
           "To be flashed:\n%s\n\n",
           version, Library.fw_version, Loader.fw_version);
    printf("Do you really want to flash the new images (yes,no)? ");
    fflush(stdout);
    fgets(confirm, sizeof(confirm), stdin);
    if(stricmp(confirm, "yes\n") != 0 && stricmp(confirm, "y\n") != 0) {
        gestic_cleanup(gestic);
        gestic_free(gestic);
        return 0;
    }

    /* Do the actual flashing */
    printf("Flashing library-loader.\n");
    flash(gestic, &Loader, gestic_UpdateFunction_ProgramFlash);

    printf("Waiting until loader-update is completed.\n");
    if(gestic_flash_wait_loader_updated(gestic, 20000) != 0) {
        fprintf(stderr, "Loader-update seems to have failed. Aborting.\n");
#ifndef ON_ERROR_RESUME_NEXT
        gestic_cleanup(gestic);
        gestic_free(gestic);
        return -1;
#endif
    }

    printf("Flashing library.\n");
    flash(gestic, &Library, gestic_UpdateFunction_ProgramFlash);

    /* Close connection to device */
    gestic_close(gestic);

    /* Release further resources that were used by gestic */
    gestic_cleanup(gestic);
    gestic_free(gestic);

    return 0;
}
