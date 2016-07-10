/*
 * Combined-PPM signal generator
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#ifndef _CPPM_H
#define _CPPM_H

#include <stdint.h>

#ifdef DEBUG

// Arduino D10
#define CPPM_PORT PORTB
#define CPPM_DDR DDRB
#define CPPM_PIN PB2

#else

#define CPPM_PORT PORTB
#define CPPM_DDR DDRB
#define CPPM_PIN PB5

#endif

extern volatile uint16_t cppmData[8];

void cppmInit(void);
void cppmCopy(uint16_t *data);

#endif

