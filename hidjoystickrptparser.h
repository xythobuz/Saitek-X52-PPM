#if !defined(__HIDJOYSTICKRPTPARSER_H__)
#define __HIDJOYSTICKRPTPARSER_H__

#include <hid.h>

/*
 * Adapted for the Saitek X52 flight controller
 */

struct GamePadEventData {
        uint16_t X, Y, Rz; // 11bit, 11bit, 10bit
        uint8_t Z, Rx, Ry, Slider;
};

class JoystickEvents {
public:
        virtual void OnGamePadChanged(const GamePadEventData *evt);
        virtual void OnHatSwitch(uint8_t hat);
        virtual void OnButtonUp(uint8_t but_id);
        virtual void OnButtonDn(uint8_t but_id);
        virtual void OnMouseMoved(uint8_t x, uint8_t y);
};

#define RPT_GEMEPAD_LEN 8
#define BUFFER_SIZE 16

class JoystickReportParser : public HIDReportParser {
        JoystickEvents *joyEvents;

        uint8_t buf[BUFFER_SIZE];
        uint8_t oldPad[RPT_GEMEPAD_LEN];
        uint8_t oldHat;
        uint64_t oldButtons;
        uint8_t oldMouse;
        GamePadEventData buffer;

public:
        JoystickReportParser(JoystickEvents *evt);

        virtual void Parse(HID *hid, bool is_rpt_id, uint8_t len, uint8_t *bufPart);
};

#endif // __HIDJOYSTICKRPTPARSER_H__

