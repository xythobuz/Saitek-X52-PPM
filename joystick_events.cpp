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

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {
    Serial.print("X: ");
    PrintHex<uint16_t > (evt->X, 0x80);
    Serial.print("\tY: ");
    PrintHex<uint16_t > (evt->Y, 0x80);
    Serial.print("\tZ: ");
    PrintHex<uint8_t > (evt->Z, 0x80);
    Serial.print("\tRx: ");
    PrintHex<uint8_t > (evt->Rx, 0x80);
    Serial.print("\tRy: ");
    PrintHex<uint8_t > (evt->Ry, 0x80);
    Serial.print("\tRz: ");
    PrintHex<uint16_t > (evt->Rz, 0x80);
    Serial.print("\tSlider: ");
    PrintHex<uint8_t > (evt->Slider, 0x80);
    Serial.println("");
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
    Serial.print("Hat Switch: ");
    PrintHex<uint8_t > (hat, 0x80);
    Serial.println("");
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
    Serial.print("Up: ");
    Serial.println(but_id, DEC);
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
    Serial.print("Dn: ");
    Serial.println(but_id, DEC);
}

void JoystickEvents::OnMouseMoved(uint8_t x, uint8_t y) {
    Serial.print("Mouse X: ");
    PrintHex<uint8_t >(x, 0x80);
    Serial.print("\tY: ");
    PrintHex<uint8_t >(y, 0x80);
    Serial.println("");
}

