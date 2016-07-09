/*
 * Saitek X52 Arduino USB Host Shield Library.
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 * 
 * Based on "x52pro-linux" by Nirenjan Krishnan
 * https://github.com/nirenjan/x52pro-linux
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#include "x52.h"

#define TIME_24H_FORMAT
#define DEBUG_OUTPUT

X52::X52(USB* u, HID* h) : usb(u), hid(h) { }

void X52::setTime(uint8_t h, uint8_t m) {
    uint8_t ret = sendCommand(X52_TIME_CLOCK1, m | ((h & 0x7F) << 8)
#ifdef TIME_24H_FORMAT
            | (1 << 15)
#endif
            );
    if (ret) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error setting time: ");
        Serial.println(ret, DEC);
#endif
    }
}

void X52::setTimeOffset(uint8_t cl, int16_t offset) {
    if (offset < -1023) {
        offset = -1023;
    }
    
    if (offset > 1023) {
        offset = 1023;
    }
    
    uint8_t negative = 0;
    if (offset < 0) {
        negative = 1;
        offset = -offset;
    }

    uint8_t ret = sendCommand(cl ? X52_OFFS_CLOCK3 : X52_OFFS_CLOCK2,
            (negative << 10) | (offset & 0x03FF)
#ifdef TIME_24H_FORMAT
            | (1 << 15)
#endif
            );
    if (ret) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error setting offset: ");
        Serial.println(ret, DEC);
#endif
    }
}

void X52::setDate(uint8_t dd, uint8_t mm, uint8_t yy) {
    uint8_t ret = sendCommand(X52_DATE_DDMM, dd | (mm << 8));
    if (!ret) {
        ret = sendCommand(X52_DATE_YEAR, yy);
    }
    if (ret) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error setting date: ");
        Serial.println(ret, DEC);
#endif
    }
}

void X52::setBlink(uint8_t state) {
    uint8_t ret = sendCommand(X52_BLINK_INDICATOR, state ? X52_BLINK_ON : X52_BLINK_OFF);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error setting blink: ");
        Serial.println(ret, DEC);
#endif
    }
}

void X52::setShift(uint8_t state) {
    uint8_t ret = sendCommand(X52_SHIFT_INDICATOR, state ? X52_SHIFT_ON : X52_SHIFT_OFF);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error setting shift: ");
        Serial.println(ret, DEC);
#endif
    }
}

void X52::setMFDText(uint8_t line, const char* text) {
    const static uint16_t lines[3] = {
        X52_MFD_LINE1,
        X52_MFD_LINE2,
        X52_MFD_LINE3
    };

    if (line >= 3) {
        return;
    }

    uint8_t ret = sendCommand(X52_MFD_CLEAR_LINE | lines[line], 0);
    if (ret) {
#ifdef DEBUG_OUTPUT
        Serial.print("Error clearing line: ");
        Serial.println(ret, DEC);
#endif
        return;
    }

    for (uint8_t i = 0; i < 16; i += 2) {
        if (text[i] == '\0') {
            break;
        }

        uint16_t value = text[i];

        if (text[i + 1] != '\0') {
            value |= text[i + 1] << 8;
        }

        ret = sendCommand(X52_MFD_WRITE_LINE | lines[line], value);
        if (ret) {
#ifdef DEBUG_OUTPUT
            Serial.print("Error writing to line: ");
            Serial.println(ret, DEC);
#endif
            return;
        }

        if (text[i + 1] == '\0') {
            break;
        }
    }
}

uint8_t X52::sendCommand(uint16_t command, uint16_t val) {
    if ((!usb) || (!hid)) {
#ifdef DEBUG_OUTPUT
        Serial.println("Invalid objects!");
#endif
        return 42;
    }

    const uint8_t valLo = (val & 0x00FF);
    const uint8_t valHi = (val & 0xFF00) >> 8;

#ifdef DEBUG_OUTPUT
    Serial.println("Sending X52 Ctrl-Req...");
#endif

    uint8_t ret = usb->ctrlReq(hid->GetAddress(), 0,
                              USB_SETUP_TYPE_VENDOR | USB_SETUP_RECIPIENT_DEVICE,
                              X52_VENDOR_REQUEST, valLo, valHi, command,
                              0, 0, NULL, NULL);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        Serial.print("Ctrl-Req Error Code: ");
        Serial.println(val, DEC);
#endif
    }

    return ret;
}

