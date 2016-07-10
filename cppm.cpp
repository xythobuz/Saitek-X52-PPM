/*
 * Combined-PPM signal generator
 *
 * Based on the code from:
 * https://quadmeup.com/generate-ppm-signal-with-arduino/
 * https://github.com/DzikuVx/ppm_encoder/blob/master/ppm_encoder_source.ino
 *
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */
#include <Arduino.h>
#include "cppm.h"

//#define DEBUG_OUTPUT

#define CHANNELS 8 // set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1500 // set the default servo value
#define FRAME_LENGTH 20000 // set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 100 // set the pulse length
#define ON_STATE 1 // set polarity of the pulses: 1 is positive, 0 is negative

static volatile uint16_t cppmData[CHANNELS];
static volatile uint8_t state = 1;
static volatile uint8_t currentChannel = CHANNELS;
static volatile uint16_t calcRest = 0;

void cppmInit(void) {
#ifdef DEBUG_OUTPUT
    Serial.println("Initializing Timer...");
#endif

    for (uint8_t i = 0; i < CHANNELS; i++) {
        cppmData[i] = CHANNEL_DEFAULT_VALUE;
    }

    pinMode(CPPM_OUTPUT_PIN, OUTPUT);
    digitalWrite(CPPM_OUTPUT_PIN, ON_STATE ? LOW : HIGH);

    cli();
    TCCR1A = 0; // set entire TCCR1 register to 0
    TCCR1B = 0;
    OCR1A = 100; // compare match register
    TCCR1B |= (1 << WGM12); // turn on CTC mode
    TCCR1B |= (1 << CS11); // 8 prescaler: 0,5 microseconds at 16mhz
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
    sei();
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

ISR(TIMER1_COMPA_vect){
    TCNT1 = 0;
    if (state) {
        // start pulse
        digitalWrite(CPPM_OUTPUT_PIN, ON_STATE ? HIGH : LOW);
        OCR1A = PULSE_LENGTH << 1;
        state = 0;
    } else {
        // end pulse and calculate when to start the next pulse
        digitalWrite(CPPM_OUTPUT_PIN, ON_STATE ? LOW : HIGH);
        state = 1;
        if (currentChannel >= CHANNELS) {
            currentChannel = 0;
            calcRest += PULSE_LENGTH;
            OCR1A = (FRAME_LENGTH - calcRest) << 1;
            calcRest = 0;
        } else {
            OCR1A = (cppmData[currentChannel] - PULSE_LENGTH) << 1;
            calcRest += cppmData[currentChannel];
            currentChannel++;
        }
    }
}

