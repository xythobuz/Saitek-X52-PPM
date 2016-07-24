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
#include "config.h"

class GamePadEventData;

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

    uint8_t getInvert(uint8_t ch) {
        if (ch < CHANNELS_MAX) return invert[ch];
        else return 0;
    }

    void setInvert(uint8_t ch, uint8_t i) {
        if (ch < CHANNELS_MAX) invert[ch] = i;
    }

    uint16_t getMinimum(uint8_t ch) {
        if (ch < CHANNELS_MAX) return minimum[ch];
        else return 0;
    }

    void setMinimum(uint8_t ch, uint16_t i) {
        if (ch < CHANNELS_MAX) minimum[ch] = i;
    }

    uint16_t getMaximum(uint8_t ch) {
        if (ch < CHANNELS_MAX) return maximum[ch];
        else return 0;
    }

    void setMaximum(uint8_t ch, uint16_t i) {
        if (ch < CHANNELS_MAX) maximum[ch] = i;
    }

    int16_t getTrim(uint8_t ch) {
        if (ch < CHANNELS_MAX) return trim[ch];
        else return 0;
    }

    void setTrim(uint8_t ch, int16_t i) {
        if (ch < CHANNELS_MAX) trim[ch] = i;
    }

  private:
    uint16_t getJoystickAxis(const GamePadEventData& evt, uint8_t ch);
    uint16_t getJoystickMax(uint8_t ch);

    uint16_t values[CHANNELS_MAX];
    uint8_t invert[CHANNELS_MAX];
    uint16_t minimum[CHANNELS_MAX];
    uint16_t maximum[CHANNELS_MAX];
    int16_t trim[CHANNELS_MAX];
};

class JoystickEventsButtons : public JoystickEvents {
  public:
    JoystickEventsButtons(JoystickEvents* client = 0);
    virtual void OnButtonDown(uint8_t but_id);

    uint8_t getCurrentMode() { return currentMode; }

  private:
    enum MenuState {
        NONE = 0,
        MAINMENU,
        CPPMMENU,
        TRIMAXISMENU,
        TRIMENDPOINTMENU,
        INVERTAXISMENU,

        STATES_EDIT,
        EDIT_CHANNELS,
        EDIT_FRAME_LENGTH,
        EDIT_PULSE_LENGTH,
        EDIT_INVERT,
        EDIT_CPPM_PIN,
        EDIT_MIN_ROLL,
        EDIT_MAX_ROLL,
        EDIT_MIN_PITCH,
        EDIT_MAX_PITCH,
        EDIT_MIN_YAW,
        EDIT_MAX_YAW,
        EDIT_MIN_THROTTLE,
        EDIT_MAX_THROTTLE,
        EDIT_MIN_AUX1,
        EDIT_MAX_AUX1,
        EDIT_MIN_AUX2,
        EDIT_MAX_AUX2,
        EDIT_INVERT_ROLL,
        EDIT_INVERT_PITCH,
        EDIT_INVERT_YAW,
        EDIT_INVERT_THROTTLE,
        EDIT_INVERT_AUX1,
        EDIT_INVERT_AUX2,

        STATES_EDIT_SIGNED,
        EDIT_TRIM_ROLL,
        EDIT_TRIM_PITCH,
        EDIT_TRIM_YAW,
        EDIT_TRIM_THROTTLE,
        EDIT_TRIM_AUX1,
        EDIT_TRIM_AUX2,

        STATES_MAX
    };

    void printMenu();
    void menuHelper(uint8_t count, const char** menu, const char* title);
    void printValue(uint16_t min, uint16_t max, const char* title);
    void printSignedValue(int16_t min, int16_t max, const char* title);

    MenuState state;
    uint8_t index;
    uint16_t value;
    int16_t signedValue;
    uint8_t currentMode;
};

extern JoystickEventsCPPM joyCPPM;
extern JoystickEventsButtons joyButtons;
extern JoystickEventsDeadZone joyDeadZone;

#endif // __JOYSTICK_EVENTS_H__

