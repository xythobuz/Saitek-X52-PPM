/*
 * Saitek X52 Arduino USB Host Shield Library.
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * Based on the USB Host Library HID Joystick example
 * https://www.circuitsathome.com/mcu/hid-joystick-code-sample
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#include <Arduino.h>
#include "data.h"
#include "cppm.h"
#include "events.h"

JoystickEventsCPPM::JoystickEventsCPPM(JoystickEvents* client) : JoystickEvents(client) {
    for (uint8_t i = 0; i < channels; i++) {
        values[i] = 1500;
        invert[i] = 0;
        minimum[i] = 1000;
        maximum[i] = 2000;
        trim[i] = 0;
    }

    /*
     * Aux switches are commonly used for arming.
     * Ensure we're not sending high values when
     * no joystick has been connected...
     */
    values[CHANNEL_AUX1] = 1000;
    values[CHANNEL_AUX2] = 1000;

    /*
     * Default values to match my personal setup.
     * Can be changed using the on-screen menu.
     */
    invert[CHANNEL_THROTTLE] = 1;
    invert[CHANNEL_PITCH] = 1;
    minimum[CHANNEL_THROTTLE] = 1010;
    maximum[CHANNEL_THROTTLE] = 1950;
    minimum[CHANNEL_ROLL] = 1050;
    maximum[CHANNEL_ROLL] = 1950;
    minimum[CHANNEL_PITCH] = 1080;
    maximum[CHANNEL_PITCH] = 1890;
    minimum[CHANNEL_AUX1] = 990;
    maximum[CHANNEL_AUX1] = 2100;
    minimum[CHANNEL_AUX2] = 990;
    maximum[CHANNEL_AUX2] = 1990;

    CPPM::instance().copy(values);
}

void JoystickEventsCPPM::OnGamePadChanged(const GamePadEventData& evt) {
    for (uint8_t i = 0; i < (CHANNEL_AUX2 + 1); i++) {
        uint16_t value = ((int32_t)getJoystickAxis(evt, i)) + trim[i];
        values[i] = map(value, 0, getJoystickMax(i),
                invert[i] ? maximum[i] : minimum[i], invert[i] ? minimum[i] : maximum[i]);
    }

    CPPM::instance().copy(values);

    if (client) {
        client->OnGamePadChanged(evt);
    }
}

uint16_t JoystickEventsCPPM::getJoystickAxis(const GamePadEventData& evt, uint8_t ch) {
    if (ch == CHANNEL_ROLL) {
        return evt.X;
    } else if (ch == CHANNEL_PITCH) {
        return evt.Y;
    } else if (ch == CHANNEL_THROTTLE) {
        return evt.Z;
    } else if (ch == CHANNEL_YAW) {
        return evt.Rz;
    } else if (ch == CHANNEL_AUX1) {
        return evt.Ry;
    } else if (ch == CHANNEL_AUX2) {
        return evt.Slider;
    } else if (ch == (CHANNEL_AUX2 + 1)) {
        return evt.Rx;
    } else {
        return 0;
    }
}

uint16_t JoystickEventsCPPM::getJoystickMax(uint8_t ch) {
    if (ch == CHANNEL_ROLL) {
        return 0x7FF;
    } else if (ch == CHANNEL_PITCH) {
        return 0x7FF;
    } else if (ch == CHANNEL_THROTTLE) {
        return 0xFF;
    } else if (ch == CHANNEL_YAW) {
        return 0x3FF;
    } else if (ch == CHANNEL_AUX1) {
        return 0xFF;
    } else if (ch == CHANNEL_AUX2) {
        return 0xFF;
    } else if (ch == (CHANNEL_AUX2 + 1)) {
        return 0xFF;
    } else {
        return 0xFF;
    }
}

