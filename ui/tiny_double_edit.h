/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * tiny_double_edit.h
 *
 *  Created on: 03.03.2009
 *      Author: rikebs
 */

#ifndef TINY_DOUBLE_EDIT_H_
#define TINY_DOUBLE_EDIT_H_
#include <stdexcept>
#include "ui_tiny_double_edit.h"

class TinyDoubleEdit : public QDialog, private Ui::tinyDoubleEdit {
Q_OBJECT
public:
    enum ExceptionMode { NoException = 0, ThrowException = 1 };
    TinyDoubleEdit(QWidget* parent = 0, double value = 0, ExceptionMode mode = ThrowException);
    double value();
    double value( double defaultValue );
private:
    double initial_;
    ExceptionMode mode_;
};

class UninitialisedValueException : public std::runtime_error {
public:
    UninitialisedValueException() :
        std::runtime_error( "Uninitialised Value" )
    {
    }
};

#endif /* TINY_DOUBLE_EDIT_H_ */
