/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * parameter_spin_box.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef PARAMETER_SPIN_BOX_H_
#define PARAMETER_SPIN_BOX_H_

#include <qspinbox.h>

#include "lattice_interface.h"

class ParameterSpinBox : public QDoubleSpinBox {
Q_OBJECT
public:
    ParameterSpinBox(Parameter< double >* parameterReference, QWidget* parent = 0);
    ~ParameterSpinBox();
    void updateReference(Parameter< double >* parameterReference);
public slots:
    void updateValue();
    void update();
signals:
    void valueChanged(const QString&, const double&);
private:
    Parameter< double >* parameter_;
    QString parameterName_;
private slots:
    void emitChanged(const double& val);
};

#endif /* PARAMETER_SPIN_BOX_H_ */
