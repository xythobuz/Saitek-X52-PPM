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

#define CPPM_OUTPUT_PIN 13

void cppmInit(void);
void cppmCopy(uint16_t *data);

#endif // __CPPM_H__

