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
    virtual void OnGamePadChanged(const GamePadEventData& evt) { if (client) client->OnGamePadChanged(evt); }
    virtual void OnHatSwitch(uint8_t hat) { if(client) client->OnHatSwitch(hat); }
    virtual void OnButtonUp(uint8_t but_id) { if(client) client->OnButtonUp(but_id); }
    virtual void OnButtonDown(uint8_t but_id) { if(client) client->OnButtonDown(but_id); }
    virtual void OnMouseMoved(uint8_t x, uint8_t y) { if (client) client->OnMouseMoved(x, y); }

  protected:
    JoystickEvents* client;
};

class JoystickEventsDeadZone : public JoystickEvents {
  public:
    JoystickEventsDeadZone(JoystickEvents* client = 0) : JoystickEvents(client) { }
    virtual void OnGamePadChanged(const GamePadEventData& evt);
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

  private:
    const static uint8_t channels = 12;
    uint16_t values[channels];
};

class JoystickEventsButtons : public JoystickEvents {
  public:
    JoystickEventsButtons(X52* x = 0, JoystickEvents* client = 0);
    virtual void OnButtonDown(uint8_t but_id);

  private:
    enum MenuState {
        NONE = 0,
        MAINMENU,

        STATES_EDIT,
        EDIT_CHANNELS,
        EDIT_FRAME_LENGTH,
        EDIT_PULSE_LENGTH,
        EDIT_INVERT,

        STATES_MAX
    };

    void printMenu();
    void menuHelper(uint8_t count, const char** menu, const char* title);
    void printValue(uint16_t min, uint16_t max, const char* title);

    X52* x52;
    MenuState state;
    uint8_t index;
    uint16_t value;
};

#endif // __JOYSTICK_EVENTS_H__

