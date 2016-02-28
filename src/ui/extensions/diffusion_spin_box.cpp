/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * diffusion_spin_box.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "diffusion_spin_box.h"

DiffusionSpinBox::DiffusionSpinBox(int diffusionIndex, QWidget* parent /*= 0*/) : QDoubleSpinBox(parent), index_(diffusionIndex)
{
    connect(this, SIGNAL(valueChanged(const double&)), this, SLOT(emitChanged(const double&)));
}

DiffusionSpinBox::~DiffusionSpinBox()
{
}

void DiffusionSpinBox::emitChanged(const double& val)
{
    emit valueChanged(index_, val);
}
