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

#ifndef __X52_H__
#define __X52_H__

#include <hid.h>

#define X52_VENDOR_REQUEST 0x91
#define X52_MFD_BRIGHTNESS 0xB1
#define X52_LED_BRIGHTNESS 0xB2
#define X52_MFD_CLEAR_LINE 0x08
#define X52_MFD_WRITE_LINE 0x00
#define X52_MFD_LINE1 0xD1
#define X52_MFD_LINE2 0xD2
#define X52_MFD_LINE3 0xD4
#define X52_SHIFT_INDICATOR 0xFD
#define X52_SHIFT_ON 0x51
#define X52_SHIFT_OFF 0x50
#define X52_BLINK_INDICATOR 0xB4
#define X52_BLINK_ON 0x51
#define X52_BLINK_OFF 0x50
#define X52_DATE_DDMM 0xC4
#define X52_DATE_YEAR 0xC8
#define X52_TIME_CLOCK1 0xC0
#define X52_OFFS_CLOCK2 0xC1
#define X52_OFFS_CLOCK3 0xC2

class X52 {
  public:
    X52(USB* u, HID* h);

    /*
     * Check if a valid PID/VID device has been found.
     */
    void initialize();

    /*
     * Set brightness of LEDs and MFD backlight.
     * Three states, 0=off, 1=dim, 2=on.
     */
    void setLEDBrightness(uint16_t val) { sendCommand(X52_LED_BRIGHTNESS, val); }
    void setMFDBrightness(uint16_t val) { sendCommand(X52_MFD_BRIGHTNESS, val); }
    
    /*
     * Print text on the MFD lines (0 - 2).
     * Maximum of 16 characters per line.
     */
    void setMFDText(uint8_t line, const char* text = "                ");

    /*
     * Enable (1) or Disable(0) the MFD shift indicator.
     */
    void setShift(uint8_t state);

    /*
     * Fast blinking of Info and Hat LEDs.
     * State can be 0=off or 1=on.
     */
    void setBlink(uint8_t state);

    /*
     * Set date. The third digit seems to be broken in hardware?
     */
    void setDate(uint8_t dd, uint8_t mm, uint8_t yy);

    /*
     * Set time. There seems to be a problem with time offsets
     * and the 12h/24h time format conflicting...?
     */
    void setTime(uint8_t h, uint8_t m);
    void setTimeOffset(uint8_t cl, int16_t offset);

  private:
    uint8_t sendCommand(uint16_t command, uint16_t val);
    
    USB* usb;
    HID* hid;
    uint8_t ready;
};

extern X52 x52;

#endif // __X52_H__

