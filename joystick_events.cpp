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

#include "hid_parser.h"

//#define DEBUG_OUTPUT_RAW
#define DEBUG_OUTPUT


/*
 * Uuuhhh TODO!!
 * This is not a deadzone, this is something like sensitivity.
 * For deadzone, we need to only apply it from the center outwards.
 */


JoystickEvents::JoystickEvents()
        : lastData(0), lastMouseX(0), lastMouseY(0), deadZone(0) {
    deadZone.X = 0;
    deadZone.Y = 0;
    deadZone.Z = 0;
    deadZone.Rx = 0;
    deadZone.Ry = 0;
    deadZone.Rz = 50;
    deadZoneMouseX = 0;
    deadZoneMouseY = 0;
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {
#ifdef DEBUG_OUTPUT_RAW
    Serial.print("X: ");
    PrintHex<uint16_t > (evt->X, 0x80);
    Serial.print(" Y: ");
    PrintHex<uint16_t > (evt->Y, 0x80);
    Serial.print(" Z: ");
    PrintHex<uint8_t > (evt->Z, 0x80);
    Serial.print(" Rx: ");
    PrintHex<uint8_t > (evt->Rx, 0x80);
    Serial.print(" Ry: ");
    PrintHex<uint8_t > (evt->Ry, 0x80);
    Serial.print(" Rz: ");
    PrintHex<uint16_t > (evt->Rz, 0x80);
    Serial.print(" S: ");
    PrintHex<uint8_t > (evt->Slider, 0x80);
    Serial.println("");
#endif

    GamePadEventData newData = lastData;
    uint8_t updated = 0;

    if ((evt->X > (lastData.X + deadZone.X))
            || (evt->X < (lastData.X - deadZone.X))) {
        newData.X = evt->X;
        updated = 1;
    }

    if ((evt->Y > (lastData.Y + deadZone.Y))
            || (evt->Y < (lastData.Y - deadZone.Y))) {
        newData.Y = evt->Y;
        updated = 1;
    }

    if ((evt->Z > (lastData.Z + deadZone.Z))
            || (evt->Z < (lastData.Z - deadZone.Z))) {
        newData.Z = evt->Z;
        updated = 1;
    }

    if ((evt->Rx > (lastData.Rx + deadZone.Rx))
            || (evt->Rx < (lastData.Rx - deadZone.Rx))) {
        newData.Rx = evt->Rx;
        updated = 1;
    }

    if ((evt->Ry > (lastData.Ry + deadZone.Ry))
            || (evt->Ry < (lastData.Ry - deadZone.Ry))) {
        newData.Ry = evt->Ry;
        updated = 1;
    }

    if ((evt->Rz > (lastData.Rz + deadZone.Rz))
            || (evt->Rz < (lastData.Rz - deadZone.Rz))) {
        newData.Rz = evt->Rz;
        updated = 1;
    }

    if ((evt->Slider > (lastData.Slider + deadZone.Slider))
            || (evt->Slider < (lastData.Slider - deadZone.Slider))) {
        newData.Slider = evt->Slider;
        updated = 1;
    }

    if (updated) {
#ifdef DEBUG_OUTPUT
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
#endif
    }

    lastData = *evt;
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
#ifdef DEBUG_OUTPUT
    Serial.print("Hat Switch: ");
    PrintHex<uint8_t > (hat, 0x80);
    Serial.println("");
#endif
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
#ifdef DEBUG_OUTPUT
    Serial.print("Up: ");
    Serial.println(but_id, DEC);
#endif
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
#ifdef DEBUG_OUTPUT
    Serial.print("Down: ");
    Serial.println(but_id, DEC);
#endif
}

void JoystickEvents::OnMouseMoved(uint8_t x, uint8_t y) {
#ifdef DEBUG_OUTPUT_RAW
    Serial.print("Mouse X: ");
    PrintHex<uint8_t >(x, 0x80);
    Serial.print("\tY: ");
    PrintHex<uint8_t >(y, 0x80);
    Serial.println("");
#endif

    uint8_t newX = lastMouseX;
    uint8_t newY = lastMouseY;
    uint8_t updated = 0;
    
    if ((x > (lastMouseX + deadZoneMouseX))
            || (x < (lastMouseX - deadZoneMouseX))) {
        newX = x;
        updated = 1;
    }

    if ((y > (lastMouseY + deadZoneMouseY))
            || (y < (lastMouseY - deadZoneMouseY))) {
        newY = y;
        updated = 1;
    }

    if (updated) {
#ifdef DEBUG_OUTPUT
        Serial.print("Mouse X: ");
        PrintHex<uint8_t >(newX, 0x80);
        Serial.print("\tY: ");
        PrintHex<uint8_t >(newY, 0x80);
        Serial.println("");
#endif
    }

    lastMouseX = x;
    lastMouseY = y;
}

