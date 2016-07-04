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

#include <hid.h>
#include <hiduniversal.h>
#include <usbhub.h>

#include "hid_parser.h"
#include "x52.h"

USB usb;
USBHub hub(&usb);
HIDUniversal hid(&usb);
X52 x52(&usb, &hid);
JoystickEvents joyevents;
JoystickReportParser joy(&joyevents);

void setup() {  
    Serial.begin(115200);
    Serial.println("Start");

    if (usb.Init() == -1) {
        Serial.println("OSC did not start.");
    }

    delay(200);

    if (!hid.SetReportParser(0, &joy)) {
        ErrorMessage<uint8_t >(PSTR("SetReportParser"), 1);
    }
}

void loop() {
    usb.Task();

    static unsigned long lastTime = 0;
    static uint8_t d = 0;
    if ((millis() - lastTime) >= 500) {
        //x52.setDate(d, d, d);
        d++;
        lastTime = millis();

        String tmp = String(d);
        //x52.setMFDText(0, tmp.c_str());
    }

    if (Serial.available()) {
        char c = Serial.read();
        if (c == 't') {
            x52.setMFDText(0, "Arduino");
            x52.setMFDText(1, "Hello");
            x52.setMFDText(2, "World");
        } else if (c == '0') {
            x52.setMFDBrightness(0);
        } else if (c == '1') {
            x52.setMFDBrightness(1);
        } else if (c == '2') {
            x52.setMFDBrightness(2);
        } else if (c == '3') {
            x52.setLEDBrightness(0);
        } else if (c == '4') {
            x52.setLEDBrightness(1);
        } else if (c == '5') {
            x52.setLEDBrightness(2);
        } else if (c == 'q') {
            x52.setShift(1);
        } else if (c == 'w') {
            x52.setShift(0);
        } else if (c == 'a') {
            x52.setBlink(1);
        } else if (c == 's') {
            x52.setBlink(0);
        } else if (c == 'z') {
            x52.setDate(1, 1, 1);
        } else if (c == 'x') {
            x52.setTime(12, 42);
            x52.setTimeOffset(0, -120);
            x52.setTimeOffset(0, 240);
        } else {
            Serial.println("Unknown command!");
        }
    }
}

