/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * tiny_double_edit.cpp
 *
 *  Created on: 03.03.2009
 *      Author: rikebs
 */

#include "tiny_double_edit.h"

TinyDoubleEdit::TinyDoubleEdit(QWidget* parent, double value, ExceptionMode mode) : QDialog(parent), initial_(value), mode_(mode)
{
    setupUi(this);
    doubleEditSpinBox->setValue(value);
}

double TinyDoubleEdit::value()
{
    if (result() != QDialog::Accepted) {
        if (mode_ == ThrowException) {
            throw UninitialisedValueException();
        } else {
            return initial_;
        }
    }
    return doubleEditSpinBox->value();
}

double TinyDoubleEdit::value(double defaultValue)
{
    if (result() != QDialog::Accepted) {
        return defaultValue;
    }
    return doubleEditSpinBox->value();
}
