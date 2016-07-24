/*
 * EEPROM configuration storage utility
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>

#define CPPM_OUTPUT_PIN_DEFAULT 4
#define CHANNEL_MINIMUM_VALUE 1000
#define CHANNEL_DEFAULT_VALUE 1500
#define CHANNEL_MAXIMUM_VALUE 2000
#define CHANNELS_MAX 12
#define DEFAULT_CHANNELS 6
#define DEFAULT_FRAME_LENGTH 20000
#define DEFAULT_PULSE_LENGTH 300
#define DEFAULT_INVERT_STATE 0

enum RxChannels {
    CHANNEL_ROLL = 0,
    CHANNEL_PITCH = 1,
    CHANNEL_THROTTLE = 2,
    CHANNEL_YAW = 3,
    CHANNEL_AUX1 = 4,
    CHANNEL_AUX2 = 5
};

// Increase string number when struct changes!
#define CONFIG_VERSION "USBCPPM-02"
#define CONFIG_VERSION_LENGTH (sizeof(CONFIG_VERSION) - 1)

struct ConfigData {
    uint16_t channels, frameLength, pulseLength, inverted, cppmPin;
    uint16_t minimum[CHANNELS_MAX];
    uint16_t maximum[CHANNELS_MAX];
    uint16_t invert[CHANNELS_MAX];
    int16_t trim[CHANNELS_MAX];
};
#define CONFIG_DATA_LENGTH (sizeof(ConfigData))

void eepromRead();
void eepromWrite();

#endif // __CONFIG_H__

