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

#include <stdint.h>

class GamePadEventData;
class X52;

class JoystickEvents {
  public:
    JoystickEvents(JoystickEvents* _client = 0) : client(_client) { }
    virtual void OnGamePadChanged(const GamePadEventData& evt) = 0;
    virtual void OnHatSwitch(uint8_t hat) = 0;
    virtual void OnButtonUp(uint8_t but_id) = 0;
    virtual void OnButtonDown(uint8_t but_id) = 0;
    virtual void OnMouseMoved(uint8_t x, uint8_t y) = 0;

  protected:
    JoystickEvents* client;
};

class JoystickEventsDeadZone : public JoystickEvents {
  public:
    JoystickEventsDeadZone(JoystickEvents* client = 0) : JoystickEvents(client) { }
    virtual void OnGamePadChanged(const GamePadEventData& evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDown(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);

  private:
    const static GamePadEventData deadZone;
    const static uint8_t deadZoneMouseX, deadZoneMouseY;

    const static GamePadEventData centerValue;
    const static uint8_t centerMouseX, centerMouseY;
};

class JoystickEventsCPPM : public JoystickEvents {
  public:
    JoystickEventsCPPM(JoystickEvents* client = 0);
    virtual void OnGamePadChanged(const GamePadEventData& evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDown(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);

  private:
    const static uint8_t channels = 8;
    uint16_t values[channels];
};

class JoystickEventsButtons : public JoystickEvents {
  public:
    JoystickEventsButtons(X52* x = 0, JoystickEvents* client = 0);
    virtual void OnGamePadChanged(const GamePadEventData& evt);
    virtual void OnHatSwitch(uint8_t hat);
    virtual void OnButtonUp(uint8_t but_id);
    virtual void OnButtonDown(uint8_t but_id);
    virtual void OnMouseMoved(uint8_t x, uint8_t y);

  private:
    X52* x52;
};

#endif // __JOYSTICK_EVENTS_H__

