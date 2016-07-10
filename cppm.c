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

#include <avr/io.h>
#include <avr/interrupt.h>

#include "cppm.h"

#define MAX_STATES 17
#define CHANNELS 8
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
volatile uint16_t triggerTimeRemaining = 0;

#define NONE 0
#define COMPARE_MATCH 1
#define OVERFLOW 2
volatile uint8_t triggerState = NONE;

static void triggerIn(uint16_t us);
static void nextState(void);

void cppmInit(void) {
    // Set pin to output mode
    CPPM_DDR |= (1 << CPPM_PIN);

    // Start with a low pulse
    CPPM_PORT &= ~(1 << CPPM_PIN);

    TCCR0B |= (1 << CS01); // Prescaler: 8

#ifdef DEBUG
    TIMSK0 |= (1 << TOIE0) | (1 << OCIE0A); // Enable Overflow and Compare Match Interrupt
#else
    TIMSK |= (1 << TOIE0) | (1 << OCIE0A); // Enable Overflow and Compare Match Interrupt
#endif
    OCR0A = 0;

    state = 0;
    delaySum = MIN_WAIT;
    triggerIn(PULSE_LOW);
}

void cppmCopy(uint16_t *data) {
    cli();
    for (int i = 0; i < CHANNELS; i++) {
        cppmData[i] = data[i];
    }
    sei();
}

static void triggerIn(uint16_t us) {
    TCNT0 = 0; // Reset Timer
    if (us <= (TIME_AFTER_OVERFLOW - 1)) {
        triggerState = COMPARE_MATCH;
        OCR0A = us * TIME_MULTIPLIER;
    } else {
        triggerState = OVERFLOW;
        triggerTimeRemaining = us - TIME_AFTER_OVERFLOW;
    }
}

static void nextState(void) {
    state++;
    if (state > MAX_STATES) {
        state = 0;
        delaySum = MIN_WAIT;
    }
    if ((state % 2) == 0) {
        // pulse pause
        CPPM_PORT &= ~(1 << CPPM_PIN);
        triggerIn(PULSE_LOW);
    } else {
        CPPM_PORT |= (1 << CPPM_PIN);
        if (state <= 15) {
            // normal ppm pulse
            uint8_t index = state / 2;
            triggerIn(cppmData[index]);
            delaySum += PULSE_WIDTH - cppmData[index];
        } else {
            // sync pulse
            triggerIn(delaySum);
        }
    }
}

#ifdef DEBUG
ISR(TIMER0_OVF_vect) {
#else
ISR(TIM0_OVF_vect) {
#endif
    if (triggerState == OVERFLOW) {
        if (triggerTimeRemaining == 0) {
            triggerState = NONE;
            nextState();
        } else {
            triggerIn(triggerTimeRemaining);
        }
    }
}

#ifdef DEBUG
ISR(TIMER0_COMPA_vect) {
#else
ISR(TIM0_COMPA_vect) {
#endif
    if (triggerState == COMPARE_MATCH) {
        triggerState = NONE;
        nextState();
    }
}

