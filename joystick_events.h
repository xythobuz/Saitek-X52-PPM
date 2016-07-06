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
    GamePadEventData(uint16_t v) : X(v), Y(v), Z(v), Rx(v), Ry(v), Rz(v) { }
    
    uint16_t X, Y, Rz; // 11bit, 11bit, 10bit
    uint8_t Z, Rx, Ry, Slider;
};

class JoystickEvents {
  public:
    JoystickEvents();

    virtual void OnGamePadChanged(const GamePadEventData *evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDn(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);

  protected:
    GamePadEventData lastData;
    GamePadEventData deadZone;
    uint8_t lastMouseX, lastMouseY;
    uint8_t deadZoneMouseX, deadZoneMouseY;
};

#endif // __JOYSTICK_EVENTS_H__

