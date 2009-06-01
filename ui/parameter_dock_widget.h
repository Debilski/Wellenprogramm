/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * parameter_dock_widget.h
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#ifndef PARAMETER_DOCK_WIDGET_H_
#define PARAMETER_DOCK_WIDGET_H_

#include <qdockwidget.h>

class ParameterDockWidget : public QDockWidget {
    Q_OBJECT
public:
    ParameterDockWidget(QWidget* parent = 0, Qt::WindowFlags flags = 0);
};

#endif /* PARAMETER_DOCK_WIDGET_H_ */
