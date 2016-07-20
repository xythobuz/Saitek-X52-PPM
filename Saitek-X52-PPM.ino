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
#include <avr/wdt.h>

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

static void stringHelper(String& a, String& b, char delim, uint8_t line) {
    String s = a + delim;
    for (uint8_t i = 0; i < (14 - a.length() - b.length()); i++) {
        s += ' ';
    }
    s += b + delim;
    x52.setMFDText(line, s.c_str());
}

void statusCallback(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2) {
    x52.setMFDText(0, "Telemetry Status");

    uint16_t l1 = q1 * 100 / 255, l2 = q2 * 100 / 255;
    String link1(l1), link2(l2);
    stringHelper(link1, link2, '%', 1);

    uint32_t v1 = a1 * 330 / 255;
    uint32_t v1hundred = v1 / 100;
    uint32_t v1ten = v1 % 100;
    String volt1 = String(v1hundred) + '.' + (v1ten);
    uint32_t v2 = a2 * 330 / 255;
    uint32_t v2hundred = v2 / 100;
    uint32_t v2ten = v2 % 100;
    String volt2 = String(v2hundred) + '.' + (v2ten);
    stringHelper(volt1, volt2, 'V', 2);
}

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
    frsky.setDataHandler(&statusCallback);
    wdt_enable(WDTO_500MS);
}

void init_joystick() {
    x52.initialize();
    statusCallback(0, 0, 0, 0);

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
    wdt_reset();
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

