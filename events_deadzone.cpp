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

#include <usb.h>

#include "data.h"
#include "events.h"

//#define DEBUG_OUTPUT_RAW
//#define DEBUG_OUTPUT

// X, Y, Z, Rx, Ry, Rz, Slider
const GamePadEventData JoystickEventsDeadZone::deadZone(
    4, 4, 2, 2, 5, 25, 2
);
const uint8_t JoystickEventsDeadZone::deadZoneMouseX = 1;
const uint8_t JoystickEventsDeadZone::deadZoneMouseY = 1;

// X, Y, Z, Rx, Ry, Rz, Slider
const GamePadEventData JoystickEventsDeadZone::centerValue(
    0x3FF, 0x3FF, 0x7F, 0x7F, 0x7F, 0x1FF, 0x7F
);
const uint8_t JoystickEventsDeadZone::centerMouseX = 0x07;
const uint8_t JoystickEventsDeadZone::centerMouseY = 0x07;

void JoystickEventsDeadZone::OnGamePadChanged(const GamePadEventData& evt) {
#ifdef DEBUG_OUTPUT_RAW
    Serial.print("X: ");
    PrintHex<uint16_t > (evt.X, 0x80);
    Serial.print(" Y: ");
    PrintHex<uint16_t > (evt.Y, 0x80);
    Serial.print(" Z: ");
    PrintHex<uint8_t > (evt.Z, 0x80);
    Serial.print(" Rx: ");
    PrintHex<uint8_t > (evt.Rx, 0x80);
    Serial.print(" Ry: ");
    PrintHex<uint8_t > (evt.Ry, 0x80);
    Serial.print(" Rz: ");
    PrintHex<uint16_t > (evt.Rz, 0x80);
    Serial.print(" S: ");
    PrintHex<uint8_t > (evt.Slider, 0x80);
    Serial.println("");
#endif

    GamePadEventData newData = centerValue;

#ifdef DEBUG_OUTPUT
    uint8_t updated = 0;
#endif

    if ((evt.X > (centerValue.X + deadZone.X))
            || (evt.X < (centerValue.X - deadZone.X))) {
        newData.X = evt.X;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Y > (centerValue.Y + deadZone.Y))
            || (evt.Y < (centerValue.Y - deadZone.Y))) {
        newData.Y = evt.Y;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Z > (centerValue.Z + deadZone.Z))
            || (evt.Z < (centerValue.Z - deadZone.Z))) {
        newData.Z = evt.Z;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Rx > (centerValue.Rx + deadZone.Rx))
            || (evt.Rx < (centerValue.Rx - deadZone.Rx))) {
        newData.Rx = evt.Rx;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Ry > (centerValue.Ry + deadZone.Ry))
            || (evt.Ry < (centerValue.Ry - deadZone.Ry))) {
        newData.Ry = evt.Ry;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Rz > (centerValue.Rz + deadZone.Rz))
            || (evt.Rz < (centerValue.Rz - deadZone.Rz))) {
        newData.Rz = evt.Rz;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((evt.Slider > (centerValue.Slider + deadZone.Slider))
            || (evt.Slider < (centerValue.Slider - deadZone.Slider))) {
        newData.Slider = evt.Slider;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

#ifdef DEBUG_OUTPUT
    if (updated) {
        Serial.print("X: ");
        PrintHex<uint16_t > (newData.X, 0x80);
        Serial.print(" Y: ");
        PrintHex<uint16_t > (newData.Y, 0x80);
        Serial.print(" Z: ");
        PrintHex<uint8_t > (newData.Z, 0x80);
        Serial.print(" Rx: ");
        PrintHex<uint8_t > (newData.Rx, 0x80);
        Serial.print(" Ry: ");
        PrintHex<uint8_t > (newData.Ry, 0x80);
        Serial.print(" Rz: ");
        PrintHex<uint16_t > (newData.Rz, 0x80);
        Serial.print(" S: ");
        PrintHex<uint8_t > (newData.Slider, 0x80);
        Serial.println("");
    }
#endif

    if (client) {
        client->OnGamePadChanged(newData);
    }
}

void JoystickEventsDeadZone::OnHatSwitch(uint8_t hat) {
#ifdef DEBUG_OUTPUT
    Serial.print("Hat Switch: ");
    PrintHex<uint8_t > (hat, 0x80);
    Serial.println("");
#endif

    if (client) {
        client->OnHatSwitch(hat);
    }
}

void JoystickEventsDeadZone::OnButtonUp(uint8_t but_id) {
#ifdef DEBUG_OUTPUT
    Serial.print("Up: ");
    Serial.println(but_id, DEC);
#endif

    if (client) {
        client->OnButtonUp(but_id);
    }
}

void JoystickEventsDeadZone::OnButtonDown(uint8_t but_id) {
#ifdef DEBUG_OUTPUT
    Serial.print("Down: ");
    Serial.println(but_id, DEC);
#endif

    if (client) {
        client->OnButtonDown(but_id);
    }
}

void JoystickEventsDeadZone::OnMouseMoved(uint8_t x, uint8_t y) {
#ifdef DEBUG_OUTPUT_RAW
    Serial.print("Mouse X: ");
    PrintHex<uint8_t >(x, 0x80);
    Serial.print("\tY: ");
    PrintHex<uint8_t >(y, 0x80);
    Serial.println("");
#endif

    uint8_t newX = centerMouseX;
    uint8_t newY = centerMouseY;

#ifdef DEBUG_OUTPUT
    uint8_t updated = 0;
#endif

    if ((x > (centerMouseX + deadZoneMouseX))
            || (x < (centerMouseX - deadZoneMouseX))) {
        newX = x;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

    if ((y > (centerMouseY + deadZoneMouseY))
            || (y < (centerMouseY - deadZoneMouseY))) {
        newY = y;

#ifdef DEBUG_OUTPUT
        updated = 1;
#endif
    }

#ifdef DEBUG_OUTPUT
    if (updated) {
        Serial.print("Mouse X: ");
        PrintHex<uint8_t >(newX, 0x80);
        Serial.print("\tY: ");
        PrintHex<uint8_t >(newY, 0x80);
        Serial.println("");
    }
#endif

    if (client) {
        client->OnMouseMoved(x, y);
    }
}

