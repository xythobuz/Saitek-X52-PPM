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

#include "events.h"
#include "parser.h"
#include "x52.h"
#include "cppm.h"
#include "frsky.h"

#define ENABLE_SERIAL_PORT
//#define DEBUG_OUTPUT Serial
//#define DEBUG_MFD_UPTIME

USB usb;
USBHub hub(&usb);
HIDUniversal hid(&usb);
X52 x52(&usb, &hid);
JoystickEventsCPPM joyCPPM;
JoystickEventsButtons joyButtons(&x52, (JoystickEvents*)&joyCPPM);
JoystickEventsDeadZone joyDeadZone((JoystickEvents*)&joyButtons);
JoystickReportParser joy(&joyDeadZone);
FrSky frsky(&Serial);

void setup() {
#ifdef ENABLE_SERIAL_PORT
    Serial.begin(115200);
#endif

#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.println("Start");
#endif

    pinMode(13, OUTPUT);
    digitalWrite(13, LOW);

    if (usb.Init() == -1) {
        digitalWrite(13, HIGH);
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("OSC did not start.");
#endif
    }

    if (!hid.SetReportParser(0, &joy)) {
        digitalWrite(13, HIGH);
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("Error adding report parser.");
#endif
    }

    CPPM::instance().init();
}

void init_joystick() {
    x52.initialize();
    x52.setMFDText(0, "Arduino X52 Host");
    x52.setMFDText(1, "should be ready!");
    x52.setMFDText(2, " OK for options ");

    // Sometimes the first message is lost, so send again
    if (joyButtons.getCurrentMode() == 1) {
        x52.setLEDBrightness(2);
        x52.setMFDBrightness(2);
    } else if (joyButtons.getCurrentMode() == 2) {
        x52.setLEDBrightness(1);
        x52.setMFDBrightness(1);
    } else if (joyButtons.getCurrentMode() == 3) {
        x52.setLEDBrightness(0);
        x52.setMFDBrightness(0);
    }
}

void loop() {
    usb.Task();
    frsky.poll();

    static unsigned long lastTime = 0;
    static uint8_t initialized = 0;
    if ((millis() - lastTime) >= 1000) {
        lastTime = millis();
        if (initialized == 0) {
            initialized = 1;
        } else if (initialized == 1) {
            init_joystick();
            initialized = 2;
        }

#ifdef DEBUG_MFD_UPTIME
        String text = "Uptime: " + String(millis() / 1000) + "s";
        x52.setMFDText(2, text.c_str());
#endif
    }
}

