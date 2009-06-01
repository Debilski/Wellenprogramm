/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * parameter_dock_widget.cpp
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#include "parameter_dock_widget.h"

#include <QtGui>

ParameterDockWidget::ParameterDockWidget(QWidget* parent, Qt::WindowFlags flags) :
    QDockWidget(tr("Parameter Dock"),parent, flags)
{
    QWidget* w = new QWidget(this);
    QSpinBox* box = new QSpinBox(w);
    QFormLayout* l = new QFormLayout(w);
    l->addRow(box);
    w->setLayout(l);
    setWidget(w);
}
