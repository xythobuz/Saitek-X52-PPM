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

//#define DEBUG_OUTPUT Serial

#define TIME_24H_FORMAT

X52::X52(USB* u, HID* h) : usb(u), hid(h), ready(0) { }

void X52::initialize() {
    ready = 0;
    
    if ((!usb) || (!hid)) {
        return;
    }

    // Get the USB device descriptor so we can check for a valid PID/VID
    uint8_t buf[sizeof (USB_DEVICE_DESCRIPTOR)];
    USB_DEVICE_DESCRIPTOR* udd = reinterpret_cast<USB_DEVICE_DESCRIPTOR*>(buf);
    uint8_t ret = usb->getDevDescr(hid->GetAddress(), 0, sizeof(USB_DEVICE_DESCRIPTOR), (uint8_t*)buf);
    if (ret) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Error getting descriptor: ");
        DEBUG_OUTPUT.println(ret, DEC);
#endif
    }

    uint16_t vid = udd->idVendor;
    uint16_t pid = udd->idProduct;

#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("VID: ");
        DEBUG_OUTPUT.print(vid, DEC);
        DEBUG_OUTPUT.print(" PID: ");
        DEBUG_OUTPUT.println(pid, DEC);
#endif

    if ((vid == 0x06A3) && (pid == 0x0255)) {
        // Saitek X52
        ready = 1;
    } else if ((vid == 0x06A3) && (pid == 0x0762)) {
        // Saitek X52 Pro
        ready = 1;
    } else {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("No valid VID/PID found!");
#endif
    }
}

void X52::setTime(uint8_t h, uint8_t m) {
    uint8_t ret = sendCommand(X52_TIME_CLOCK1, m | ((h & 0x7F) << 8)
#ifdef TIME_24H_FORMAT
            | (1 << 15)
#endif
            );
    if (ret) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Error setting time: ");
        DEBUG_OUTPUT.println(ret, DEC);
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
        DEBUG_OUTPUT.print("Error setting offset: ");
        DEBUG_OUTPUT.println(ret, DEC);
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
        DEBUG_OUTPUT.print("Error setting date: ");
        DEBUG_OUTPUT.println(ret, DEC);
#endif
    }
}

void X52::setBlink(uint8_t state) {
    uint8_t ret = sendCommand(X52_BLINK_INDICATOR, state ? X52_BLINK_ON : X52_BLINK_OFF);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Error setting blink: ");
        DEBUG_OUTPUT.println(ret, DEC);
#endif
    }
}

void X52::setShift(uint8_t state) {
    uint8_t ret = sendCommand(X52_SHIFT_INDICATOR, state ? X52_SHIFT_ON : X52_SHIFT_OFF);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Error setting shift: ");
        DEBUG_OUTPUT.println(ret, DEC);
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
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Invalid line: ");
        DEBUG_OUTPUT.println(line, DEC);
#endif
        return;
    }

    uint8_t ret = sendCommand(X52_MFD_CLEAR_LINE | lines[line], 0);
    if (ret) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Error clearing line: ");
        DEBUG_OUTPUT.println(ret, DEC);
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
            DEBUG_OUTPUT.print("Error writing to line: ");
            DEBUG_OUTPUT.println(ret, DEC);
#endif
            return;
        }

        if (text[i + 1] == '\0') {
            break;
        }
    }
}

uint8_t X52::sendCommand(uint16_t command, uint16_t val) {
    if (!ready) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("Invalid state!");
#endif
        return 23;
    }
    
    if ((!usb) || (!hid)) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("Invalid objects!");
#endif
        return 42;
    }

    const uint8_t valLo = (val & 0x00FF);
    const uint8_t valHi = (val & 0xFF00) >> 8;

#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.println("Sending X52 Ctrl-Req...");
#endif

    uint8_t ret = usb->ctrlReq(hid->GetAddress(), 0,
                              USB_SETUP_TYPE_VENDOR | USB_SETUP_RECIPIENT_DEVICE,
                              X52_VENDOR_REQUEST, valLo, valHi, command,
                              0, 0, NULL, NULL);
    if (ret != 0) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Ctrl-Req Error Code: ");
        DEBUG_OUTPUT.println(val, DEC);
#endif
    }

    return ret;
}

