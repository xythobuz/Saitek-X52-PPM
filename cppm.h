/*
 * Combined-PPM signal generator
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#ifndef __CPPM_H__
#define __CPPM_H__

#include <stdint.h>
#include <avr/interrupt.h>
#include "config.h"

ISR(TIMER1_COMPA_vect);

class CPPM {
  public:
    inline static CPPM& instance() {
        if (!inst) {
            inst = new CPPM();
        }
        return *inst;
    }

    void init(void);
    void copy(uint16_t* d);

    inline uint16_t getChannels() { return channels; }
    inline void setChannels(uint16_t c) {
        if (c > CHANNELS_MAX)
            c = CHANNELS_MAX;
        channels = c;
    }

    inline uint16_t getFrameLength() { return frameLength; }
    inline void setFrameLength(uint16_t fl) { frameLength = fl; }

    inline uint16_t getPulseLength() { return pulseLength; }
    inline void setPulseLength(uint16_t pl) { pulseLength = pl; }

    inline uint8_t getInvert() { return !onState; }
    inline void setInvert(uint8_t i) { onState = !i; }

    inline uint8_t getOutput() { return output; }
    void setOutput(uint8_t i);

  private:
    CPPM() : channels(DEFAULT_CHANNELS), frameLength(DEFAULT_FRAME_LENGTH),
            pulseLength(DEFAULT_PULSE_LENGTH), onState(!DEFAULT_INVERT_STATE),
            output(CPPM_OUTPUT_PIN_DEFAULT) { }
    CPPM(CPPM&) { }

    volatile uint16_t channels;
    volatile uint16_t frameLength; // PPM frame length in microseconds (1ms = 1000µs)
    volatile uint16_t pulseLength;
    volatile uint8_t onState; // polarity of the pulses: 1 is positive, 0 is negative
    volatile uint8_t output;

    volatile uint16_t data[CHANNELS_MAX];
    volatile uint8_t state;
    volatile uint8_t currentChannel;
    volatile uint16_t calcRest;

    static CPPM* inst;
    friend void TIMER1_COMPA_vect(void);
};

#endif // __CPPM_H__

