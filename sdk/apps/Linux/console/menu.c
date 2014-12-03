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
#include "console.h"

enum {
    MENU_AUTO_CALIB,
    MENU_CALIBRATE,
    MENU_TOUCH_DETECT,
    MENU_AIR_WHEEL,
    MENU_QUIT,
    MENU_SIZE
};

#define ACTION_INVOKE 0
#define ACTION_INCREASE 1
#define ACTION_DECREASE 2

void action(data_t *data, int action)
{
    switch(data->menu_current)
    {
        case MENU_AUTO_CALIB:
            switch_auto_calib(data);
            break;
        case MENU_CALIBRATE:
            if(action == ACTION_INVOKE)
                calibrate_now(data);
            break;
        case MENU_TOUCH_DETECT:
            switch_touch_detect(data);
            break;
        case MENU_AIR_WHEEL:
            switch_air_wheel(data);
            break;
        case MENU_QUIT:
            if(action == ACTION_INVOKE)
                data->running = 0;
            break;
    }
}

void update_menu(data_t *data)
{
    int redraw = 1;
    int c;

    if(data->menu_current < 0) {
        /* Print the menu and select the first entry on startup */
        data->menu_current = 0;
        redraw = 2;
    }

    c = getch();
    switch(c) {
    case 10:
        action(data, ACTION_INVOKE);
        break;
    case KEY_UP:
        data->menu_current = (MENU_SIZE + data->menu_current - 1) % MENU_SIZE;
        break;
    case KEY_DOWN:
        data->menu_current = (MENU_SIZE + data->menu_current + 1) % MENU_SIZE;
        break;
    case KEY_LEFT:
        action(data, ACTION_DECREASE);
        break;
    case KEY_RIGHT:
        action(data, ACTION_INCREASE);
        break;
    default:
        if(redraw == 1)
            redraw = 0;
        break;
    }

    if(redraw)
    {
        /* Print the current state of the menu */
        move(14, 0);
        printw("%c Calibration:     %s\n",
               data->menu_current == MENU_AUTO_CALIB ? '>' : ' ',
               data->auto_calib ? "Automatic" : "Manual   ");
        printw("%c Calibrate Now\n",
               data->menu_current == MENU_CALIBRATE ? '>' : ' ');
        printw("%c Touch Detection: %s\n",
               data->menu_current == MENU_TOUCH_DETECT ? '>' : ' ',
               data->touch_detect ? "Enabled " : "Disabled");
        printw("%c Air Wheel:       %s\n",
               data->menu_current == MENU_AIR_WHEEL ? '>' : ' ',
               data->air_wheel ? "Enabled " : "Disabled");
        printw("%c Quit\n",
               data->menu_current == MENU_QUIT ? '>' : ' ');
    }
}
