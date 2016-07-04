#include "hidjoystickrptparser.h"

/*
 * Adapted for the Saitek X52 flight controller
 */

JoystickReportParser::JoystickReportParser(JoystickEvents *evt) :
joyEvents(evt), oldHat(0), oldButtons(0), oldMouse(0) {
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                oldPad[i] = 0;
}

void JoystickReportParser::Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *bufPart) {
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

        /*
        Serial.println("");
        Serial.print("Packet: ");
        for (uint8_t i = 0; i < (8 + len); i++) {
          PrintHex<uint8_t >(buf[i], 0x80);
          Serial.print(" ");
        }
        Serial.println("");
        Serial.println("");
        */

        // Checking if there are changes in report since the method was last called
        bool match = true;
        for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++)
                if (buf[i] != oldPad[i]) {
                        match = false;
                        break;
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
                
                joyEvents->OnGamePadChanged((const GamePadEventData*)&buffer);

                for (uint8_t i = 0; i < RPT_GEMEPAD_LEN; i++) oldPad[i] = buf[i];
        }

        // Calling Hat Switch event handler`
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
                                        joyEvents->OnButtonDn(i);
                                } else {
                                        joyEvents->OnButtonUp(i);
                                }
                        }
                }
                oldButtons = buttons;
        }

        if (oldMouse != buf[13] && joyEvents) {
          oldMouse = buf[13];
          joyEvents->OnMouseMoved((buf[13] & 0xF0) >> 4, buf[13] & 0x0F);
        }
}

void JoystickEvents::OnGamePadChanged(const GamePadEventData *evt) {
        Serial.print("X: ");
        PrintHex<uint16_t > (evt->X, 0x80);
        Serial.print("\tY: ");
        PrintHex<uint16_t > (evt->Y, 0x80);
        Serial.print("\tZ: ");
        PrintHex<uint8_t > (evt->Z, 0x80);
        Serial.print("\tRx: ");
        PrintHex<uint8_t > (evt->Rx, 0x80);
        Serial.print("\tRy: ");
        PrintHex<uint8_t > (evt->Ry, 0x80);
        Serial.print("\tRz: ");
        PrintHex<uint16_t > (evt->Rz, 0x80);
        Serial.print("\tSlider: ");
        PrintHex<uint8_t > (evt->Slider, 0x80);
        Serial.println("");
}

void JoystickEvents::OnHatSwitch(uint8_t hat) {
        Serial.print("Hat Switch: ");
        PrintHex<uint8_t > (hat, 0x80);
        Serial.println("");
}

void JoystickEvents::OnButtonUp(uint8_t but_id) {
        Serial.print("Up: ");
        Serial.println(but_id, DEC);
}

void JoystickEvents::OnButtonDn(uint8_t but_id) {
        Serial.print("Dn: ");
        Serial.println(but_id, DEC);
}

void JoystickEvents::OnMouseMoved(uint8_t x, uint8_t y) {
        Serial.print("Mouse X: ");
        PrintHex<uint8_t >(x, 0x80);
        Serial.print("\tY: ");
        PrintHex<uint8_t >(y, 0x80);
        Serial.println("");
}

