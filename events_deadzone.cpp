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

#include "data.h"
#include "events.h"

//#define DEBUG_OUTPUT_RAW Serial
//#define DEBUG_OUTPUT Serial

/*
 * Deadzone around the axis center, in both directions.
 * You can try to counteract a faulty Yaw-Potentiometer
 * with this, but it won't be enough in extreme cases.
 * X, Y, Z, Rx, Ry, Rz, Slider
 */
const GamePadEventData JoystickEventsDeadZone::deadZone(
    4, 4, 2, 2, 5, 25, 2
);
const uint8_t JoystickEventsDeadZone::deadZoneMouseX = 1;
const uint8_t JoystickEventsDeadZone::deadZoneMouseY = 1;

/*
 * Absolute values of the axis center. Deadzone will be applied
 * in both directions around these values.
 * X 11bit, Y 11bit, Z 8bit, Rx 8bit, Ry 8bit, Rz 10bit, Slider 8bit
 */
const GamePadEventData JoystickEventsDeadZone::centerValue(
    0x3FF, 0x3FF, 0x7F, 0x7F, 0x7F, 0x1FF, 0x7F
);
const uint8_t JoystickEventsDeadZone::centerMouseX = 0x07;
const uint8_t JoystickEventsDeadZone::centerMouseY = 0x07;

void JoystickEventsDeadZone::OnGamePadChanged(const GamePadEventData& evt) {
#ifdef DEBUG_OUTPUT_RAW
    DEBUG_OUTPUT_RAW.print("Raw X: ");
    DEBUG_OUTPUT_RAW.print(evt.X, HEX);
    DEBUG_OUTPUT_RAW.print(" Y: ");
    DEBUG_OUTPUT_RAW.print(evt.Y, HEX);
    DEBUG_OUTPUT_RAW.print(" Z: ");
    DEBUG_OUTPUT_RAW.print(evt.Z, HEX);
    DEBUG_OUTPUT_RAW.print(" Rx: ");
    DEBUG_OUTPUT_RAW.print(evt.Rx, HEX);
    DEBUG_OUTPUT_RAW.print(" Ry: ");
    DEBUG_OUTPUT_RAW.print(evt.Ry, HEX);
    DEBUG_OUTPUT_RAW.print(" Rz: ");
    DEBUG_OUTPUT_RAW.print(evt.Rz, HEX);
    DEBUG_OUTPUT_RAW.print(" S: ");
    DEBUG_OUTPUT_RAW.println(evt.Slider, HEX);
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
        DEBUG_OUTPUT.print("X: ");
        DEBUG_OUTPUT.print(newData.X, HEX);
        DEBUG_OUTPUT.print(" Y: ");
        DEBUG_OUTPUT.print(newData.Y, HEX);
        DEBUG_OUTPUT.print(" Z: ");
        DEBUG_OUTPUT.print(newData.Z, HEX);
        DEBUG_OUTPUT.print(" Rx: ");
        DEBUG_OUTPUT.print(newData.Rx, HEX);
        DEBUG_OUTPUT.print(" Ry: ");
        DEBUG_OUTPUT.print(newData.Ry, HEX);
        DEBUG_OUTPUT.print(" Rz: ");
        DEBUG_OUTPUT.print(newData.Rz, HEX);
        DEBUG_OUTPUT.print(" S: ");
        DEBUG_OUTPUT.println(newData.Slider, HEX);
    }
#endif

    if (client) {
        client->OnGamePadChanged(newData);
    }
}

void JoystickEventsDeadZone::OnMouseMoved(uint8_t x, uint8_t y) {
#ifdef DEBUG_OUTPUT_RAW
    DEBUG_OUTPUT_RAW.print("Raw Mouse X: ");
    DEBUG_OUTPUT_RAW.print(x, HEX);
    DEBUG_OUTPUT_RAW.print("\tY: ");
    DEBUG_OUTPUT_RAW.println(y, HEX);
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
        DEBUG_OUTPUT.print("Mouse X: ");
        DEBUG_OUTPUT.print(newX, HEX);
        DEBUG_OUTPUT.print("\tY: ");
        DEBUG_OUTPUT.println(newY, HEX);
    }
#endif

    if (client) {
        client->OnMouseMoved(x, y);
    }
}

