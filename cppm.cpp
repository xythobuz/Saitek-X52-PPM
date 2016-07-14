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

CPPM* CPPM::inst = NULL;

void CPPM::init(void) {
#ifdef DEBUG_OUTPUT
    Serial.println("Initializing Timer...");
#endif

    state = 1;
    currentChannel = channels;
    calcRest = 0;
    for (uint8_t i = 0; i < channels; i++) {
        data[i] = CHANNEL_DEFAULT_VALUE;
    }

    pinMode(CPPM_OUTPUT_PIN, OUTPUT);
    digitalWrite(CPPM_OUTPUT_PIN, CPPM::inst->onState ? LOW : HIGH);

    cli();
    TCCR1A = 0; // set entire TCCR1 register to 0
    TCCR1B = 0;
    OCR1A = 100; // compare match register
    TCCR1B |= (1 << WGM12); // turn on CTC mode
    TCCR1B |= (1 << CS11); // 8 prescaler: 0,5 microseconds at 16mhz
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
    sei();
}

void CPPM::copy(uint16_t* d) {
#ifdef DEBUG_OUTPUT
    Serial.println("New CPPM data!");
#endif

    cli();
    for (int i = 0; i < channels; i++) {
        data[i] = d[i];
    }
    sei();
}

ISR(TIMER1_COMPA_vect) {
    if (!CPPM::inst) {
        return;
    }

    TCNT1 = 0;
    if (CPPM::inst->state) {
        // start pulse
        digitalWrite(CPPM_OUTPUT_PIN, CPPM::inst->onState ? HIGH : LOW);
        OCR1A = CPPM::inst->pulseLength << 1;
        CPPM::inst->state = 0;
    } else {
        // end pulse and calculate when to start the next pulse
        digitalWrite(CPPM_OUTPUT_PIN, CPPM::inst->onState ? LOW : HIGH);
        CPPM::inst->state = 1;
        if (CPPM::inst->currentChannel >= CPPM::inst->channels) {
            CPPM::inst->currentChannel = 0;
            CPPM::inst->calcRest += CPPM::inst->pulseLength;
            OCR1A = (CPPM::inst->frameLength - CPPM::inst->calcRest) << 1;
            CPPM::inst->calcRest = 0;
        } else {
            OCR1A = (CPPM::inst->data[CPPM::inst->currentChannel] - CPPM::inst->pulseLength) << 1;
            CPPM::inst->calcRest += CPPM::inst->data[CPPM::inst->currentChannel];
            CPPM::inst->currentChannel++;
        }
    }
}

