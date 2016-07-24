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

#include <Arduino.h>
#include "data.h"
#include "x52.h"
#include "cppm.h"
#include "events.h"
#include "config.h"

//#define DEBUG_OUTPUT Serial
//#define DEBUG_BUTTON_MFD

#define MENU_BUTTON_ENTER_1 29
#define MENU_BUTTON_ENTER_2 26
#define MENU_BUTTON_DOWN 27
#define MENU_BUTTON_UP 28

#define MODE_BUTTON_GREEN 23
#define MODE_BUTTON_YELLOW 24
#define MODE_BUTTON_RED 25

#define MIN_FRAME_LENGTH 10000
#define MAX_FRAME_LENGTH 30000
#define MIN_PULSE_LENGTH 100
#define MAX_PULSE_LENGTH 1000
#define MIN_LOW_ENDPOINT 500
#define MAX_LOW_ENDPOINT 1500
#define MIN_HIGH_ENDPOINT 1500
#define MAX_HIGH_ENDPOINT 2500
#define MIN_TRIM -100
#define MAX_TRIM 100

void statusCallback(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2);

JoystickEventsButtons::JoystickEventsButtons(JoystickEvents* client)
        : JoystickEvents(client), state(NONE), index(0),
        value(0), signedValue(0), currentMode(0) { }

void JoystickEventsButtons::printMenu() {
    static const char* mainMenu[] = {
        "Status", "Trim Axis", "Trim Endpoint", "Invert Axis", "CPPM Config",
        "Save EEPROM", "Load EEPROM"
    };
    static const uint8_t mainMenuCount = sizeof(mainMenu) / sizeof(mainMenu[0]);

    static const char* cppmMenu[] = {
        "Channels", "Frame Length", "Pulse Length", "Invert", "Main Menu"
    };
    static const uint8_t cppmMenuCount = sizeof(cppmMenu) / sizeof(cppmMenu[0]);

    static const char* axisMenu[] = {
        "Roll", "Pitch", "Yaw", "Throttle", "Aux1", "Aux2", "Main Menu"
    };
    static const uint8_t axisMenuCount = sizeof(axisMenu) / sizeof(axisMenu[0]);

    static const char* endpointMenu[] = {
        "Roll Min", "Roll Max", "Pitch Min", "Pitch Max", "Yaw Min", "Yaw Max",
        "Throttle Min", "Throttle Max", "Aux1 Min", "Aux1 Max", "Aux2 Min",
        "Aux2 Max", "Main Menu"
    };
    static const uint8_t endpointMenuCount = sizeof(endpointMenu) / sizeof(endpointMenu[0]);

    if (state == NONE) {
        statusCallback(0, 0, 0, 0);
    } else if (state == MAINMENU) {
        menuHelper(mainMenuCount, mainMenu, "Main Menu");
    } else if (state == CPPMMENU) {
        menuHelper(cppmMenuCount, cppmMenu, "CPPM Config Menu");
    } else if (state == TRIMAXISMENU) {
        menuHelper(axisMenuCount, axisMenu, "Trim Axis Menu");
    } else if (state == TRIMENDPOINTMENU) {
        menuHelper(endpointMenuCount, endpointMenu, "Trim Endpoints");
    } else if (state == INVERTAXISMENU) {
        menuHelper(axisMenuCount, axisMenu, "Invert Axis Menu");
    } else if (state == EDIT_CHANNELS) {
        printValue(4, CHANNELS_MAX, cppmMenu[0]);
    } else if (state == EDIT_FRAME_LENGTH) {
        printValue(MIN_FRAME_LENGTH, MAX_FRAME_LENGTH, cppmMenu[1]);
    } else if (state == EDIT_PULSE_LENGTH) {
        printValue(MIN_PULSE_LENGTH, MAX_PULSE_LENGTH, cppmMenu[2]);
    } else if (state == EDIT_INVERT) {
        printValue(0, 1, cppmMenu[3]);
    } else if ((state >= EDIT_INVERT_ROLL) && (state <= EDIT_INVERT_AUX2)) {
        uint8_t index = state - EDIT_INVERT_ROLL;
        printValue(0, 1, (String("Invert ") + axisMenu[index]).c_str());
    } else if ((state >= EDIT_MIN_ROLL) && (state <= EDIT_MAX_AUX2)) {
        uint8_t index = state - EDIT_MIN_ROLL;
        if (index & 0x01) {
            printValue(MIN_HIGH_ENDPOINT, MAX_HIGH_ENDPOINT, endpointMenu[index]);
        } else {
            printValue(MIN_LOW_ENDPOINT, MAX_LOW_ENDPOINT, endpointMenu[index]);
        }
    } else if ((state >= EDIT_TRIM_ROLL) && (state <= EDIT_TRIM_AUX2)) {
        uint8_t index = state - EDIT_TRIM_ROLL;
        printSignedValue(MIN_TRIM, MAX_TRIM, (String("Trim ") + axisMenu[index]).c_str());
    }
}

void JoystickEventsButtons::OnButtonDown(uint8_t but_id) {
#ifdef DEBUG_BUTTON_MFD
    String text = "Button " + String(but_id) + " down";
    x52.setMFDText(1, text.c_str());
#endif

    if (but_id == MODE_BUTTON_GREEN) {
        x52.setLEDBrightness(2);
        x52.setMFDBrightness(2);
        currentMode = 1;
    } else if (but_id == MODE_BUTTON_YELLOW) {
        x52.setLEDBrightness(1);
        x52.setMFDBrightness(1);
        currentMode = 2;
    } else if (but_id == MODE_BUTTON_RED) {
        x52.setLEDBrightness(0);
        x52.setMFDBrightness(0);
        currentMode = 3;
    } else if ((but_id == MENU_BUTTON_ENTER_1) || (but_id == MENU_BUTTON_ENTER_2)) {
        if (state == NONE) {
            state = MAINMENU;
            index = 0;
        } else if (state == MAINMENU) {
            if (index == 0) {
                state = NONE;
            } else if (index == 1) {
                state = TRIMAXISMENU;
                index = 0;
            } else if (index == 2) {
                state = TRIMENDPOINTMENU;
                index = 0;
            } else if (index == 3) {
                state = INVERTAXISMENU;
                index = 0;
            } else if (index == 4) {
                state = CPPMMENU;
                index = 0;
            } else if (index == 5) {
                eepromWrite();
                index = 0;
            } else if (index == 6) {
                eepromRead();
                index = 0;
            }
        } else if (state == TRIMAXISMENU) {
            if (index == 0) {
                state = EDIT_TRIM_ROLL;
                signedValue = joyCPPM.getTrim(CHANNEL_ROLL);
            } else if (index == 1) {
                state = EDIT_TRIM_PITCH;
                signedValue = joyCPPM.getTrim(CHANNEL_PITCH);
            } else if (index == 2) {
                state = EDIT_TRIM_YAW;
                signedValue = joyCPPM.getTrim(CHANNEL_YAW);
            } else if (index == 3) {
                state = EDIT_TRIM_THROTTLE;
                signedValue = joyCPPM.getTrim(CHANNEL_THROTTLE);
            } else if (index == 4) {
                state = EDIT_TRIM_AUX1;
                signedValue = joyCPPM.getTrim(CHANNEL_AUX1);
            } else if (index == 5) {
                state = EDIT_TRIM_AUX2;
                signedValue = joyCPPM.getTrim(CHANNEL_AUX2);
            } else if (index == 6) {
                state = MAINMENU;
                index = 0;
            }
        } else if (state == TRIMENDPOINTMENU) {
            if (index == 0) {
                state = EDIT_MIN_ROLL;
                value = joyCPPM.getMinimum(CHANNEL_ROLL);
            } else if (index == 1) {
                state = EDIT_MAX_ROLL;
                value = joyCPPM.getMaximum(CHANNEL_ROLL);
            } else if (index == 2) {
                state = EDIT_MIN_PITCH;
                value = joyCPPM.getMinimum(CHANNEL_PITCH);
            } else if (index == 3) {
                state = EDIT_MAX_PITCH;
                value = joyCPPM.getMaximum(CHANNEL_PITCH);
            } else if (index == 4) {
                state = EDIT_MIN_YAW;
                value = joyCPPM.getMinimum(CHANNEL_YAW);
            } else if (index == 5) {
                state = EDIT_MAX_YAW;
                value = joyCPPM.getMaximum(CHANNEL_YAW);
            } else if (index == 6) {
                state = EDIT_MIN_THROTTLE;
                value = joyCPPM.getMinimum(CHANNEL_THROTTLE);
            } else if (index == 7) {
                state = EDIT_MAX_THROTTLE;
                value = joyCPPM.getMaximum(CHANNEL_THROTTLE);
            } else if (index == 8) {
                state = EDIT_MIN_AUX1;
                value = joyCPPM.getMinimum(CHANNEL_AUX1);
            } else if (index == 9) {
                state = EDIT_MAX_AUX1;
                value = joyCPPM.getMaximum(CHANNEL_AUX1);
            } else if (index == 10) {
                state = EDIT_MIN_AUX2;
                value = joyCPPM.getMinimum(CHANNEL_AUX2);
            } else if (index == 11) {
                state = EDIT_MAX_AUX2;
                value = joyCPPM.getMaximum(CHANNEL_AUX2);
            } else if (index == 12) {
                state = EDIT_MIN_ROLL;
                state = MAINMENU;
                index = 0;
            }
        } else if (state == INVERTAXISMENU) {
            if (index == 0) {
                state = EDIT_INVERT_ROLL;
                value = joyCPPM.getInvert(CHANNEL_ROLL);
            } else if (index == 1) {
                state = EDIT_INVERT_PITCH;
                value = joyCPPM.getInvert(CHANNEL_PITCH);
            } else if (index == 2) {
                state = EDIT_INVERT_YAW;
                value = joyCPPM.getInvert(CHANNEL_YAW);
            } else if (index == 3) {
                state = EDIT_INVERT_THROTTLE;
                value = joyCPPM.getInvert(CHANNEL_THROTTLE);
            } else if (index == 4) {
                state = EDIT_INVERT_AUX1;
                value = joyCPPM.getInvert(CHANNEL_AUX1);
            } else if (index == 5) {
                state = EDIT_INVERT_AUX2;
                value = joyCPPM.getInvert(CHANNEL_AUX2);
            } else if (index == 6) {
                state = MAINMENU;
                index = 0;
            }
        } else if (state == CPPMMENU) {
            if (index == 0) {
                state = EDIT_CHANNELS;
                value = CPPM::instance().getChannels();
            } else if (index == 1) {
                state = EDIT_FRAME_LENGTH;
                value = CPPM::instance().getFrameLength();
            } else if (index == 2) {
                state = EDIT_PULSE_LENGTH;
                value = CPPM::instance().getPulseLength();
            } else if (index == 3) {
                state = EDIT_INVERT;
                value = CPPM::instance().getInvert();
            } else if (index == 4) {
                state = MAINMENU;
                index = 0;
            }
        } else if (state == EDIT_CHANNELS) {
            CPPM::instance().setChannels(value);
            state = CPPMMENU;
        } else if (state == EDIT_FRAME_LENGTH) {
            CPPM::instance().setFrameLength(value);
            state = CPPMMENU;
        } else if (state == EDIT_PULSE_LENGTH) {
            CPPM::instance().setPulseLength(value);
            state = CPPMMENU;
        } else if (state == EDIT_INVERT) {
            CPPM::instance().setInvert(value);
            state = CPPMMENU;
        } else if (state == EDIT_INVERT_ROLL) {
            joyCPPM.setInvert(CHANNEL_ROLL, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_INVERT_PITCH) {
            joyCPPM.setInvert(CHANNEL_PITCH, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_INVERT_YAW) {
            joyCPPM.setInvert(CHANNEL_YAW, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_INVERT_THROTTLE) {
            joyCPPM.setInvert(CHANNEL_THROTTLE, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_INVERT_AUX1) {
            joyCPPM.setInvert(CHANNEL_AUX1, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_INVERT_AUX2) {
            joyCPPM.setInvert(CHANNEL_AUX2, value);
            state = INVERTAXISMENU;
        } else if (state == EDIT_MIN_ROLL) {
            joyCPPM.setMinimum(CHANNEL_ROLL, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_ROLL) {
            joyCPPM.setMaximum(CHANNEL_ROLL, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MIN_PITCH) {
            joyCPPM.setMinimum(CHANNEL_PITCH, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_PITCH) {
            joyCPPM.setMaximum(CHANNEL_PITCH, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MIN_YAW) {
            joyCPPM.setMinimum(CHANNEL_YAW, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_YAW) {
            joyCPPM.setMaximum(CHANNEL_YAW, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MIN_THROTTLE) {
            joyCPPM.setMinimum(CHANNEL_THROTTLE, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_THROTTLE) {
            joyCPPM.setMaximum(CHANNEL_THROTTLE, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MIN_AUX1) {
            joyCPPM.setMinimum(CHANNEL_AUX1, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_AUX1) {
            joyCPPM.setMaximum(CHANNEL_AUX1, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MIN_AUX2) {
            joyCPPM.setMinimum(CHANNEL_AUX2, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_MAX_AUX2) {
            joyCPPM.setMaximum(CHANNEL_AUX2, value);
            state = TRIMENDPOINTMENU;
        } else if (state == EDIT_TRIM_ROLL) {
            joyCPPM.setTrim(CHANNEL_ROLL, signedValue);
            state = TRIMAXISMENU;
        } else if (state == EDIT_TRIM_PITCH) {
            joyCPPM.setTrim(CHANNEL_PITCH, signedValue);
            state = TRIMAXISMENU;
        } else if (state == EDIT_TRIM_YAW) {
            joyCPPM.setTrim(CHANNEL_YAW, signedValue);
            state = TRIMAXISMENU;
        } else if (state == EDIT_TRIM_THROTTLE) {
            joyCPPM.setTrim(CHANNEL_THROTTLE, signedValue);
            state = TRIMAXISMENU;
        } else if (state == EDIT_TRIM_AUX1) {
            joyCPPM.setTrim(CHANNEL_AUX1, signedValue);
            state = TRIMAXISMENU;
        } else if (state == EDIT_TRIM_AUX2) {
            joyCPPM.setTrim(CHANNEL_AUX2, signedValue);
            state = TRIMAXISMENU;
        }
        printMenu();
    } else if (but_id == MENU_BUTTON_DOWN) {
        if ((state > STATES_EDIT) && (state < STATES_EDIT_SIGNED)) {
            if (value > 0) {
                value--;
            }
        } else if (state > STATES_EDIT_SIGNED) {
            if (signedValue > -32767) {
                signedValue--;
            }
        } else if (state != NONE) {
            index++;
        }
        printMenu();
    } else if (but_id == MENU_BUTTON_UP) {
        if ((state > STATES_EDIT) && (state < STATES_EDIT_SIGNED)) {
            if (value < 0xFFFF) {
                value++;
            }
        } else if (state > STATES_EDIT_SIGNED) {
            if (signedValue < 32767) {
                signedValue++;
            }
        } else if (state != NONE) {
            if (index > 0) {
                index--;
            }
        }
        printMenu();
    }

    if (client) {
        client->OnButtonDown(but_id);
    }
}

void JoystickEventsButtons::menuHelper(uint8_t count, const char** menu, const char* title) {
    if (index >= count) {
        index = count - 1;
    }

    uint8_t start = 0, line = 0;
    if (index > 1) {
        start = index - 1;
    }

    uint8_t end = start + 2;
    if (index == 0) {
        x52.setMFDText(0, title);
        line = 1;
        end = start + 1;
    }

    if (end >= count) {
        end = count - 1;
    }

    for (uint8_t i = start; i <= end; i++) {
        String tmp = (index == i) ? "-> " : "   ";
        x52.setMFDText(line++, (tmp + menu[i]).c_str());
    }

    if (line == 2) {
        x52.setMFDText(2);
    }

#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.print("menuHelper() state=");
    DEBUG_OUTPUT.print(state);
    DEBUG_OUTPUT.print(" index=");
    DEBUG_OUTPUT.print(index);
    DEBUG_OUTPUT.print(" start=");
    DEBUG_OUTPUT.print(start);
    DEBUG_OUTPUT.print(" end=");
    DEBUG_OUTPUT.println(end);
#endif
}

void JoystickEventsButtons::printValue(uint16_t min, uint16_t max, const char* title) {
#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.print("printValue() state=");
    DEBUG_OUTPUT.print(state);
    DEBUG_OUTPUT.print(" index=");
    DEBUG_OUTPUT.print(index);
    DEBUG_OUTPUT.print(" min=");
    DEBUG_OUTPUT.print(min);
    DEBUG_OUTPUT.print(" max=");
    DEBUG_OUTPUT.println(max);
#endif

    if (value < min) {
        value = min;
    }
    if (value > max) {
        value = max;
    }

    x52.setMFDText(0, (String(title) + ":").c_str());
    x52.setMFDText(1, String(value).c_str());
    x52.setMFDText(2, "Press OK to save");
}

void JoystickEventsButtons::printSignedValue(int16_t min, int16_t max, const char* title) {
#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.print("printSignedValue() state=");
    DEBUG_OUTPUT.print(state);
    DEBUG_OUTPUT.print(" index=");
    DEBUG_OUTPUT.print(index);
    DEBUG_OUTPUT.print(" min=");
    DEBUG_OUTPUT.print(min);
    DEBUG_OUTPUT.print(" max=");
    DEBUG_OUTPUT.println(max);
#endif

    if (signedValue < min) {
        signedValue = min;
    }
    if (signedValue > max) {
        signedValue = max;
    }

    x52.setMFDText(0, (String(title) + ":").c_str());
    x52.setMFDText(1, String(signedValue).c_str());
    x52.setMFDText(2, "Press OK to save");
}

