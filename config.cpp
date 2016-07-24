/*
 * EEPROM configuration storage utility
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */
#include <EEPROM.h>
#include "cppm.h"
#include "events.h"
#include "config.h"

//#define DEBUG_OUTPUT Serial

static void toEEPROM(const ConfigData& data) {
    for (uint8_t i = 0; i < CONFIG_VERSION_LENGTH; i++) {
        EEPROM.write(i, CONFIG_VERSION[i]);
    }

    const uint8_t* buffer = (const uint8_t*)((const void*)&data);
    uint8_t sum = 0;
    for (uint8_t i = 0; i < CONFIG_DATA_LENGTH; i++) {
        EEPROM.write(CONFIG_VERSION_LENGTH + i, buffer[i]);
        sum ^= buffer[i];
    }

    EEPROM.write(CONFIG_VERSION_LENGTH + CONFIG_DATA_LENGTH, sum);

#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.println("EEPROM write success!");
#endif
}

// Return 0 on valid data, otherwise header or checksum wrong
static uint8_t fromEEPROM(ConfigData& data) {
    for (uint8_t i = 0; i < CONFIG_VERSION_LENGTH; i++) {
        if (EEPROM.read(i) != CONFIG_VERSION[i]) {
#ifdef DEBUG_OUTPUT
            DEBUG_OUTPUT.println("EEPROM header wrong!");
#endif
            return 1;
        }
    }

    uint8_t* buffer = (uint8_t*)((void*)&data);
    uint8_t sum = 0;
    for (uint8_t i = 0; i < CONFIG_DATA_LENGTH; i++) {
        buffer[i] = EEPROM.read(CONFIG_VERSION_LENGTH + i);
        sum ^= buffer[i];
    }

    uint8_t storedSum = EEPROM.read(CONFIG_VERSION_LENGTH + CONFIG_DATA_LENGTH);
    if (storedSum != sum) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("EEPROM checksum wrong!");
#endif
        return 2;
    }

#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.println("EEPROM read success!");
#endif

    return 0;
}

void eepromRead() {
    ConfigData data;
    if (fromEEPROM(data) != 0) {
        data.channels = DEFAULT_CHANNELS;
        data.frameLength = DEFAULT_FRAME_LENGTH;
        data.pulseLength = DEFAULT_PULSE_LENGTH;
        data.inverted = DEFAULT_INVERT_STATE;
        for (uint8_t i = 0; i < CHANNELS_MAX; i++) {
            data.invert[i] = 0;
            data.minimum[i] = CHANNEL_MINIMUM_VALUE;
            data.maximum[i] = CHANNEL_MAXIMUM_VALUE;
            data.trim[i] = 0;
        }

        /*
         * Default values to match my personal setup.
         * Can be changed using the on-screen menu.
         */
        data.invert[CHANNEL_THROTTLE] = 1;
        data.invert[CHANNEL_PITCH] = 1;
        data.minimum[CHANNEL_THROTTLE] = 1010;
        data.maximum[CHANNEL_THROTTLE] = 1950;
        data.minimum[CHANNEL_ROLL] = 1050;
        data.maximum[CHANNEL_ROLL] = 1950;
        data.minimum[CHANNEL_PITCH] = 1080;
        data.maximum[CHANNEL_PITCH] = 1890;
        data.minimum[CHANNEL_AUX1] = 990;
        data.maximum[CHANNEL_AUX1] = 2100;
        data.minimum[CHANNEL_AUX2] = 990;
        data.maximum[CHANNEL_AUX2] = 1990;

        toEEPROM(data);
    }

    CPPM::instance().setChannels(data.channels);
    CPPM::instance().setFrameLength(data.frameLength);
    CPPM::instance().setPulseLength(data.pulseLength);
    CPPM::instance().setInvert(data.inverted);
    for (uint8_t i = 0; i < CHANNELS_MAX; i++) {
        joyCPPM.setInvert(i, data.invert[i]);
        joyCPPM.setMinimum(i, data.minimum[i]);
        joyCPPM.setMaximum(i, data.maximum[i]);
        joyCPPM.setTrim(i, data.trim[i]);
    }
}

void eepromWrite() {
    ConfigData data;
    data.channels = CPPM::instance().getChannels();
    data.frameLength = CPPM::instance().getFrameLength();
    data.pulseLength = CPPM::instance().getPulseLength();
    data.inverted = CPPM::instance().getInvert();
    for (uint8_t i = 0; i < CHANNELS_MAX; i++) {
        data.invert[i] = joyCPPM.getInvert(i);
        data.minimum[i] = joyCPPM.getMinimum(i);
        data.maximum[i] = joyCPPM.getMaximum(i);
        data.trim[i] = joyCPPM.getTrim(i);
    }
    toEEPROM(data);
}

