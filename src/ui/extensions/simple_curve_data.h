/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * simple_curve_data.h
 *
 *  Created on: 04.04.2009
 *      Author: rikebs
 */

#ifndef SIMPLE_CURVE_DATA_H_
#define SIMPLE_CURVE_DATA_H_

class QwtPlotCurve;

class SimpleCurveData {
public:
    QwtPlotCurve toQwtPlotCurve();
};

#endif /* SIMPLE_CURVE_DATA_H_ */
