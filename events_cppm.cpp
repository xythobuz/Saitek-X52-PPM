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

#define CHANNEL_THROTTLE 2
#define CHANNEL_PITCH 1
#define CHANNEL_ROLL 0
#define CHANNEL_YAW 3
#define CHANNEL_AUX1 4
#define CHANNEL_AUX2 5

JoystickEventsCPPM::JoystickEventsCPPM(JoystickEvents* client) : JoystickEvents(client) {
    for (uint8_t i = 0; i < channels; i++) {
        values[i] = 1500;
    }

    values[CHANNEL_AUX1] = 1000;
    values[CHANNEL_AUX2] = 1000;

    cppmCopy(values);
}

void JoystickEventsCPPM::OnGamePadChanged(const GamePadEventData& evt) {
    values[CHANNEL_THROTTLE] = map(evt.Z, 0, 0xFF, 1000, 2000);
    values[CHANNEL_PITCH] = map(evt.Y, 0, 0x7FF, 1000, 2000);
    values[CHANNEL_ROLL] = map(evt.X, 0, 0x7FF, 1000, 2000);
    values[CHANNEL_YAW] = map(evt.Rz, 0, 0x3FF, 1000, 2000);
    values[CHANNEL_AUX1] = map(evt.Ry, 0, 0xFF, 1000, 2000);
    values[CHANNEL_AUX2] = map(evt.Slider, 0, 0xFF, 1000, 2000);

    cppmCopy(values);

    if (client) {
        client->OnGamePadChanged(evt);
    }
}

void JoystickEventsCPPM::OnHatSwitch(uint8_t hat) {
    if (client) {
        client->OnHatSwitch(hat);
    }
}

void JoystickEventsCPPM::OnButtonUp(uint8_t but_id) {
    if (client) {
        client->OnButtonUp(but_id);
    }
}

void JoystickEventsCPPM::OnButtonDn(uint8_t but_id) {
    if (client) {
        client->OnButtonDn(but_id);
    }
}

void JoystickEventsCPPM::OnMouseMoved(uint8_t x, uint8_t y) {
    if (client) {
        client->OnMouseMoved(x, y);
    }
}

