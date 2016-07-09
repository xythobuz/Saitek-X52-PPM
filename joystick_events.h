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

class GamePadEventData {
  public:
    GamePadEventData(uint16_t v) : X(v), Y(v), Z(v), Rx(v), Ry(v), Rz(v), Slider(v) { }
    GamePadEventData(uint16_t x, uint16_t y, uint8_t z, uint8_t rx, uint8_t ry,
            uint16_t rz, uint8_t slider)
            : X(x), Y(y), Z(z), Rx(rx), Ry(ry), Rz(rz), Slider(slider) { }

    uint16_t X, Y, Rz; // 11bit, 11bit, 10bit
    uint8_t Z, Rx, Ry, Slider;
};

class JoystickEvents {
  public:
    JoystickEvents(JoystickEvents* _client = NULL) : client(_client) { }
    virtual void OnGamePadChanged(const GamePadEventData& evt) = 0;
    virtual void OnHatSwitch(uint8_t hat) = 0;
    virtual void OnButtonUp(uint8_t but_id) = 0;
    virtual void OnButtonDn(uint8_t but_id) = 0;
    virtual void OnMouseMoved(uint8_t x, uint8_t y) = 0;

  protected:
    JoystickEvents* client;
};

class JoystickEventsDeadZone : public JoystickEvents {
  public:
    virtual void OnGamePadChanged(const GamePadEventData& evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDn(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);

  protected:
    const static GamePadEventData deadZone;
    const static uint8_t deadZoneMouseX, deadZoneMouseY;

    const static GamePadEventData centerValue;
    const static uint8_t centerMouseX, centerMouseY;
};

#endif // __JOYSTICK_EVENTS_H__

