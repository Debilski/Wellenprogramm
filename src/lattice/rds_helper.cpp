/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * rds_helper.cpp
 *
 *  Created on: 01.12.2008
 *      Author: rikebs
 */

#include "rds_helper.h"

/**
 * Bestimmt das kgV von @b a und @b b.
 */
unsigned long leastCommonMultiple(int a, int b)
{
    long ab = a * b;
    long k = 1;
    while (isEven(a) && isEven(b)) {
        a = a / 2;
        b = b / 2;
        k = k * 2;
    }
    int t = (!isEven(a)) ? -b : a;
    while (t != 0) {
        while (isEven(t)) {
            t = t / 2;
        }
        if (t > 0) {
            a = t;
        } else {
            b = -t;
        }

        t = a - b;
    }
    return ab / (a * k);
}
