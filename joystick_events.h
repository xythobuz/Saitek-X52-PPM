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

#ifndef __JOYSTICK_EVENTS_H__
#define __JOYSTICK_EVENTS_H__

struct GamePadEventData {
    uint16_t X, Y, Rz; // 11bit, 11bit, 10bit
    uint8_t Z, Rx, Ry, Slider;
};

class JoystickEvents {
  public:
    virtual void OnGamePadChanged(const GamePadEventData *evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDn(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);
};

#endif // __JOYSTICK_EVENTS_H__

