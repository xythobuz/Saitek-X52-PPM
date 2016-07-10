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
#include "x52.h"
#include "events.h"

#define DEBUG_BUTTON_MFD

JoystickEventsButtons::JoystickEventsButtons(X52* x, JoystickEvents* client) : JoystickEvents(client), x52(x) {

}

void JoystickEventsButtons::OnGamePadChanged(const GamePadEventData& evt) {
    if (client) {
        client->OnGamePadChanged(evt);
    }
}

void JoystickEventsButtons::OnHatSwitch(uint8_t hat) {
    if (client) {
        client->OnHatSwitch(hat);
    }
}

void JoystickEventsButtons::OnButtonUp(uint8_t but_id) {
    if (client) {
        client->OnButtonUp(but_id);
    }
}

void JoystickEventsButtons::OnButtonDn(uint8_t but_id) {
    if (client) {
        client->OnButtonDn(but_id);
    }

#ifdef DEBUG_BUTTON_MFD
    String text = "Button " + String(but_id) + " down";
    x52->setMFDText(1, text.c_str());
#endif

    if (but_id == 23) {
        x52->setLEDBrightness(2);
        x52->setMFDBrightness(2);
    } else if (but_id == 24) {
        x52->setLEDBrightness(1);
        x52->setMFDBrightness(1);
    } else if (but_id == 25) {
        x52->setLEDBrightness(0);
        x52->setMFDBrightness(0);
    }
}

void JoystickEventsButtons::OnMouseMoved(uint8_t x, uint8_t y) {
    if (client) {
        client->OnMouseMoved(x, y);
    }
}

