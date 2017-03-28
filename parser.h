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

#ifndef __PARSER_H__
#define __PARSER_H__

#include <usbhid.h>
#include "data.h"

#define RPT_GEMEPAD_LEN 8
#define BUFFER_SIZE 16

class JoystickEvents;

class JoystickReportParser : public HIDReportParser {
  public:
    JoystickReportParser(JoystickEvents* evt);
    virtual void Parse(USBHID* hid, bool is_rpt_id, uint8_t len, uint8_t* bufPart);

  private:
    uint8_t buf[BUFFER_SIZE];
    uint8_t oldPad[RPT_GEMEPAD_LEN];
    uint8_t oldHat;
    uint64_t oldButtons;
    uint8_t oldMouse;
    GamePadEventData buffer;
    JoystickEvents* joyEvents;
};

#endif // __PARSER_H__

