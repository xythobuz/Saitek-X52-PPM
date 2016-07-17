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

//#define DEBUG_OUTPUT
//#define DEBUG_BUTTON_MFD

#define MENU_BUTTON_ENTER_1 29
#define MENU_BUTTON_ENTER_2 26
#define MENU_BUTTON_DOWN 27
#define MENU_BUTTON_UP 28

#define MODE_BUTTON_GREEN 23
#define MODE_BUTTON_YELLOW 24
#define MODE_BUTTON_RED 25

JoystickEventsButtons::JoystickEventsButtons(X52* x, JoystickEvents* client)
        : JoystickEvents(client), x52(x), state(NONE), index(0), value(0), menuTime(0) { }

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
        x52->setMFDText(0, title);
        line = 1;
        end = start + 1;
    }

    if (end >= count) {
        end = count - 1;
    }

    for (uint8_t i = start; i <= end; i++) {
        String tmp = (index == i) ? "-> " : "   ";
        x52->setMFDText(line++, (tmp + menu[i]).c_str());
    }

    if (line == 2) {
        x52->setMFDText(2);
    }

#ifdef DEBUG_OUTPUT
    Serial.print("menuHelper() state=");
    Serial.print(state);
    Serial.print(" index=");
    Serial.print(index);
    Serial.print(" start=");
    Serial.print(start);
    Serial.print(" end=");
    Serial.println(end);
#endif
}

void JoystickEventsButtons::printMenu() {
    static const char* mainMenu[] = {
        "Status", "Trim Axis", "Trim Endpoint", "Invert Axis", "CPPM Config"
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

    if (state == MAINMENU) {
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
        printValue(4, CHANNELS_MAX, mainMenu[0]);
    } else if (state == EDIT_FRAME_LENGTH) {
        printValue(10000, 30000, mainMenu[1]);
    } else if (state == EDIT_PULSE_LENGTH) {
        printValue(100, 1000, mainMenu[2]);
    } else if (state == EDIT_INVERT) {
        printValue(0, 1, mainMenu[3]);
    }

    menuTime = millis();
}

void JoystickEventsButtons::printValue(uint16_t min, uint16_t max, const char* title) {
#ifdef DEBUG_OUTPUT
    Serial.print("printValue() state=");
    Serial.print(state);
    Serial.print(" index=");
    Serial.print(index);
    Serial.print(" min=");
    Serial.print(min);
    Serial.print(" max=");
    Serial.println(max);
#endif

    if (value < min) {
        value = min;
    }
    if (value > max) {
        value = max;
    }

    x52->setMFDText(0, (String(title) + ":").c_str());
    x52->setMFDText(1, String(value).c_str());
    x52->setMFDText(2, "Press OK to save");
}

void JoystickEventsButtons::OnButtonDown(uint8_t but_id) {
#ifdef DEBUG_BUTTON_MFD
    String text = "Button " + String(but_id) + " down";
    x52->setMFDText(1, text.c_str());
#endif

    if (but_id == MODE_BUTTON_GREEN) {
        x52->setLEDBrightness(2);
        x52->setMFDBrightness(2);
    } else if (but_id == MODE_BUTTON_YELLOW) {
        x52->setLEDBrightness(1);
        x52->setMFDBrightness(1);
    } else if (but_id == MODE_BUTTON_RED) {
        x52->setLEDBrightness(0);
        x52->setMFDBrightness(0);
    } else if ((but_id == MENU_BUTTON_ENTER_1) || (but_id == MENU_BUTTON_ENTER_2)) {
        if (state == NONE) {
            state = MAINMENU;
            index = 0;
        } else if (state == MAINMENU) {
            if (index == 0) {
                state = NONE;
            } else if (index == 1) {
                //state = TRIMAXISMENU;
                //index = 0;
            } else if (index == 2) {
                //state = TRIMENDPOINTMENU;
                //index = 0;
            } else if (index == 3) {
                //state = INVERTAXISMENU;
                //index = 0;
            } else if (index == 4) {
                state = CPPMMENU;
                index = 0;
            }
        } else if (state == TRIMAXISMENU) {
            if (index == 0) {
            } else if (index == 1) {
            } else if (index == 2) {
            } else if (index == 3) {
            } else if (index == 4) {
            } else if (index == 5) {
            } else if (index == 6) {
                state = MAINMENU;
                index = 0;
            }
        } else if (state == TRIMENDPOINTMENU) {
            if (index == 0) {
            } else if (index == 1) {
            } else if (index == 2) {
            } else if (index == 3) {
            } else if (index == 4) {
            } else if (index == 5) {
            } else if (index == 6) {
            } else if (index == 7) {
            } else if (index == 8) {
            } else if (index == 9) {
            } else if (index == 10) {
            } else if (index == 11) {
            } else if (index == 12) {
                state = MAINMENU;
                index = 0;
            }
        } else if (state == INVERTAXISMENU) {
            if (index == 0) {
            } else if (index == 1) {
            } else if (index == 2) {
            } else if (index == 3) {
            } else if (index == 4) {
            } else if (index == 5) {
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
            state = MAINMENU;
        } else if (state == EDIT_FRAME_LENGTH) {
            CPPM::instance().setFrameLength(value);
            state = MAINMENU;
        } else if (state == EDIT_PULSE_LENGTH) {
            CPPM::instance().setPulseLength(value);
            state = MAINMENU;
        } else if (state == EDIT_INVERT) {
            CPPM::instance().setInvert(value);
            state = MAINMENU;
        }
        printMenu();
    } else if (but_id == MENU_BUTTON_DOWN) {
        if (state > STATES_EDIT) {
            if (value > 0) {
                value--;
            }
        } else if (state != NONE) {
            index++;
        }
        printMenu();
    } else if (but_id == MENU_BUTTON_UP) {
        if (state > STATES_EDIT) {
            if (value < 0xFFFF) {
                value++;
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

