/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * simple_plot_label.h
 *
 *  Created on: 04.04.2009
 *      Author: rikebs
 */

#ifndef SIMPLE_PLOT_LABEL_H_
#define SIMPLE_PLOT_LABEL_H_

class QwtPlotMarker;

class SimplePlotLabel {
public:
    QwtPlotMarker toQwtPlotMarker();
};

#endif /* SIMPLE_PLOT_LABEL_H_ */
