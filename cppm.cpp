/*
 * Combined-PPM signal generator
 *
 * The whole CPPM pulse should be 20ms long.
 * It contains 8 channels with 2ms each, followed
 * by 4ms of silence. One channel pulse varies between
 * 1 and 2 ms. They are seperated with a very short low
 * pulse of about 0.1ms.
 *
 * 20.000us
 *   - 8 * 2.000us = 16.000us
 *   - 9 * 100us = 900us
 *   = 3.100us
 *
 *     1     2     3     4     5     6     7     8
 *    ___   ___   ___   ___   ___   ___   ___   ___   __________
 * | |   | |   | |   | |   | |   | |   | |   | |   | |          |
 * | |   | |   | |   | |   | |   | |   | |   | |   | |          |
 * | |   | |   | |   | |   | |   | |   | |   | |   | |          |
 * |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|   |_|          |
 *
 * States:
 *  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16    17
 *
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */
#include <TimerOne.h>
#include "cppm.h"

//#define DEBUG_OUTPUT
//#define DEBUG_OUTPUT_ALL

#define CHANNELS 8
#define MAX_STATES ((2 * CHANNELS) + 1)
#define WHOLE_PULSE_WIDTH 20000
#define PULSE_WIDTH 2000
#define MAX_PULSE_WIDTH (CHANNELS * PULSE_WIDTH) // 16.000
#define PULSE_LOW 100
#define PULSE_LOW_SUM ((CHANNELS + 1) * PULSE_LOW) // 900
#define MIN_WAIT (WHOLE_PULSE_WIDTH - MAX_PULSE_WIDTH - PULSE_LOW_SUM) // 3100
#define TIME_AFTER_OVERFLOW 128
#define TIME_MULTIPLIER 2

volatile uint16_t cppmData[CHANNELS] = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500 };
volatile uint16_t delaySum = 0;
volatile uint8_t state = 0;

static void triggerIn(uint16_t us);
static void nextState(void);

void cppmInit(void) {
#ifdef DEBUG_OUTPUT
    Serial.println("Initializing Timer...");
#endif

    pinMode(CPPM_OUTPUT_PIN, OUTPUT);
    digitalWrite(CPPM_OUTPUT_PIN, LOW);
    Timer1.initialize(PULSE_LOW);
    Timer1.attachInterrupt(&nextState);
    state = 0;
    delaySum = MIN_WAIT;
}

void cppmCopy(uint16_t *data) {
#ifdef DEBUG_OUTPUT
    Serial.println("New CPPM data!");
#endif

    cli();
    for (int i = 0; i < CHANNELS; i++) {
        cppmData[i] = data[i];
    }
    sei();
}

static void triggerIn(uint16_t us) {
    Timer1.setPeriod(us);
    //Timer1.start();
}

static void nextState(void) {
    //Timer1.stop();

#ifdef DEBUG_OUTPUT_ALL
    Serial.print("CPPM state ");
    Serial.println(state, DEC);
#endif
    
    state++;
    if (state > MAX_STATES) {
        state = 0;
        delaySum = MIN_WAIT;
    }
    if (!(state & 0x01)) {
        // pulse pause
        digitalWrite(CPPM_OUTPUT_PIN, LOW);
        triggerIn(PULSE_LOW);
    } else {
        digitalWrite(CPPM_OUTPUT_PIN, HIGH);
        if (state <= 15) {
            // normal ppm pulse
            uint8_t index = state >> 1;
            triggerIn(cppmData[index]);
            delaySum += PULSE_WIDTH - cppmData[index];
        } else {
            // sync pulse
            triggerIn(delaySum);
        }
    }
}

