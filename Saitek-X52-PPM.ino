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

#define ENABLE_SERIAL_PORT
#define DEBUG_OUTPUT
//#define DEBUG_MFD_UPTIME

USB usb;
USBHub hub(&usb);
HIDUniversal hid(&usb);
X52 x52(&usb, &hid);
JoystickEventsCPPM joyCPPM;
JoystickEventsButtons joyButtons(&x52, (JoystickEvents*)&joyCPPM);
JoystickEventsDeadZone joyevents((JoystickEvents*)&joyButtons);
JoystickReportParser joy(&joyevents);

void setup() {
#ifdef ENABLE_SERIAL_PORT
    Serial.begin(115200);
#endif

#ifdef DEBUG_OUTPUT
    Serial.println("Start");
#endif

    if (usb.Init() == -1) {
#ifdef DEBUG_OUTPUT
        Serial.println("OSC did not start.");
#endif
    }

    if (!hid.SetReportParser(0, &joy)) {
        ErrorMessage<uint8_t >(PSTR("SetReportParser"), 1);
    }

    CPPM::instance().init();
}

void init_joystick() {
    x52.initialize();
    x52.setMFDText(0, "Arduino X52 Host");
    x52.setMFDText(1, "should be ready!");
    x52.setMFDText(2, " OK for options ");
}

void loop() {
    usb.Task();

    static unsigned long lastTime = 0;
    static uint8_t initialized = 0;
    if ((millis() - lastTime) >= 1000) {
        lastTime = millis();
        if (!initialized) {
            init_joystick();
            initialized = 1;
        }

#ifdef DEBUG_MFD_UPTIME
        String text = "Uptime: " + String(millis() / 1000) + "s";
        x52.setMFDText(2, text.c_str());
#endif
    }
}

