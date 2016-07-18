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

#include "events.h"
#include "parser.h"

//#define DEBUG_OUTPUT Serial

JoystickReportParser::JoystickReportParser(JoystickEvents* evt)
        : joyEvents(evt), oldHat(0), oldButtons(0), oldMouse(0), buffer(0) {
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) {
        oldPad[i] = 0;
    }
}

void JoystickReportParser::Parse(HID* hid, bool is_rpt_id, uint8_t len, uint8_t* bufPart) {
    // Ugly hack for too small packet size in USB Host library...
    if (len == 8) {
        // First part of buffer, store and do nothing
        for (uint8_t i = 0; i < 8; i++) {
            buf[i] = bufPart[i];
        }
        return;
    } else {
        // Append second part, then evaluate
        for (uint8_t i = 0; i < len; i++) {
            buf[i + 8] = bufPart[i];
        }
    }

#ifdef DEBUG_OUTPUT
    // Dump whole USB HID packet for debugging purposes
    DEBUG_OUTPUT.println("");
    DEBUG_OUTPUT.print("Packet: ");
    for (uint8_t i = 0; i < (8 + len); i++) {
        DEBUG_OUTPUT.print(buf[i], HEX);
        DEBUG_OUTPUT.print(" ");
    }
    DEBUG_OUTPUT.println("");
    DEBUG_OUTPUT.println("");
#endif

    // Checking if there are changes in report since the method was last called
    bool match = true;
    for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) {
        if (buf[i] != oldPad[i]) {
            match = false;
            break;
        }
    }

    // Calling Game Pad event handler
    if (!match && joyEvents) {
        buffer.X = buf[0] | ((buf[1] & 0x07) << 8);
        buffer.Y = ((buf[1] & 0xF8) >> 3) | ((buf[2] & 0x3F) << 5);
        buffer.Rz = ((buf[2] & 0xC0) >> 6) | (buf[3] << 2);
        buffer.Z = buf[4];
        buffer.Rx = buf[5];
        buffer.Ry = buf[6];
        buffer.Slider = buf[7];

        joyEvents->OnGamePadChanged(buffer);

        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) {
            oldPad[i] = buf[i];
        }
    }

    // Calling Hat Switch event handler
    uint8_t hat = (buf[12] & 0xF0) >> 4;
    if (hat != oldHat && joyEvents) {
        joyEvents->OnHatSwitch(hat);
        oldHat = hat;
    }

    uint64_t buttons = buf[12] & 0x03;
    buttons <<= 8;
    buttons |= buf[11];
    buttons <<= 8;
    buttons |= buf[10];
    buttons <<= 8;
    buttons |= buf[9];
    buttons <<= 8;
    buttons |= buf[8];

    // Calling Button Event Handler for every button changed
    uint64_t changes = (buttons ^ oldButtons);
    if (changes) {
        for (uint8_t i = 0; i < 34; i++) {
            uint64_t mask = (1ull << i);
            if ((mask & changes) && joyEvents) {
                if (buttons & mask) {
                    joyEvents->OnButtonDown(i);
                } else {
                    joyEvents->OnButtonUp(i);
                }
            }
        }
        oldButtons = buttons;
    }

    // Calling Mouse Event Handler if state has changed
    if (oldMouse != buf[13] && joyEvents) {
        oldMouse = buf[13];
        joyEvents->OnMouseMoved((buf[13] & 0xF0) >> 4, buf[13] & 0x0F);
    }
}

