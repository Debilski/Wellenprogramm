/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * diffusion_spin_box.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef DIFFUSION_SPIN_BOX_H_
#define DIFFUSION_SPIN_BOX_H_

#include <qspinbox.h>

class DiffusionSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    DiffusionSpinBox(int diffusionIndex, QWidget* parent = 0);
    ~DiffusionSpinBox();
signals:
    void valueChanged(int, const double&);
private:
    int index_;
private slots:
    void emitChanged(const double& val);
};

#endif /* DIFFUSION_SPIN_BOX_H_ */
