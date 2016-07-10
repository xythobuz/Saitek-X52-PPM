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

#ifndef __GAMEPAD_EVENT_DATA_H__
#define __GAMEPAD_EVENT_DATA_H__

#include <stdint.h>

class GamePadEventData {
  public:
    GamePadEventData(uint16_t v) : X(v), Y(v), Z(v), Rx(v), Ry(v), Rz(v), Slider(v) { }
    GamePadEventData(uint16_t x, uint16_t y, uint8_t z, uint8_t rx, uint8_t ry,
            uint16_t rz, uint8_t slider)
            : X(x), Y(y), Z(z), Rx(rx), Ry(ry), Rz(rz), Slider(slider) { }

    uint16_t X, Y, Rz; // 11bit, 11bit, 10bit
    uint8_t Z, Rx, Ry, Slider;
};

#endif // __GAMEPAD_EVENT_DATA_H__

