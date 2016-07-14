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

#define DEBUG_OUTPUT
//#define DEBUG_BUTTON_MFD

#define MENU_BUTTON_ENTER_1 29
#define MENU_BUTTON_ENTER_2 26
#define MENU_BUTTON_DOWN 27
#define MENU_BUTTON_UP 28

#define MODE_BUTTON_GREEN 23
#define MODE_BUTTON_YELLOW 24
#define MODE_BUTTON_RED 25

JoystickEventsButtons::JoystickEventsButtons(X52* x, JoystickEvents* client)
        : JoystickEvents(client), x52(x), state(NONE), index(0), value(0) { }

void JoystickEventsButtons::OnGamePadChanged(const GamePadEventData& evt) {
    if (client) {
        client->OnGamePadChanged(evt);
    }
}

void JoystickEventsButtons::OnHatSwitch(uint8_t hat) {
#ifdef DEBUG_BUTTON_MFD
    String text = "Hat is " + String(hat);
    x52->setMFDText(1, text.c_str());
#endif

    if (client) {
        client->OnHatSwitch(hat);
    }
}

void JoystickEventsButtons::OnButtonUp(uint8_t but_id) {
#ifdef DEBUG_BUTTON_MFD
    String text = "Button " + String(but_id) + " up";
    x52->setMFDText(1, text.c_str());
#endif

    if (client) {
        client->OnButtonUp(but_id);
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
        "Channels", "Frame Length",
        "Pulse Length", "Invert"
    };
    static const uint8_t mainMenuCount = sizeof(mainMenu) / sizeof(mainMenu[0]);

    if (state == MAINMENU) {
        menuHelper(mainMenuCount, mainMenu, "Main Menu");
    } else if (state == EDIT_CHANNELS) {
        printValue(4, CHANNELS_MAX, mainMenu[0]);
    } else if (state == EDIT_FRAME_LENGTH) {
        printValue(10000, 30000, mainMenu[1]);
    } else if (state == EDIT_PULSE_LENGTH) {
        printValue(100, 1000, mainMenu[2]);
    } else if (state == EDIT_INVERT) {
        printValue(0, 1, mainMenu[3]);
    }
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
        } else if (state == MAINMENU) {
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

void JoystickEventsButtons::OnMouseMoved(uint8_t x, uint8_t y) {
    if (client) {
        client->OnMouseMoved(x, y);
    }
}

