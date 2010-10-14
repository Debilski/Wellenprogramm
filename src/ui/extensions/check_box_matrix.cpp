/*
 * check_box_matrix.cpp
 *
 *  Created on: 19.06.2009
 *      Author: rikebs
 */

/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

#include "check_box_matrix.h"

#include "qcheckbox.h"
#include  "qdebug.h"

/*! \class CheckBoxMatrix
 *  CheckBoxMatrix provides a simple way of handling a matrix of checkboxes and keeping them in sync with a QVector< int >.
 *
 *  The rows in the matrix specify the index of the QVector while the ith column sets the value to the ith power of 2.
 *
 */

CheckBoxMatrix::CheckBoxMatrix(int rows, int cols)
{
    matrix_.resize( rows );
    checkBoxMatrix_.resize( rows );
    for (int i = 0; i < checkBoxMatrix_.size(); ++i) {
        checkBoxMatrix_[ i ].resize( cols );
    }
}

void CheckBoxMatrix::associateCheckBox(QCheckBox* checkBox, int row, int col,
                                       bool keepStateOfCheckBox)
{
    checkBoxMatrix_[ row ][ col ] = checkBox;
    if ( keepStateOfCheckBox ) {
        setChecked( checkBox->isChecked(), row, col );
    } else {
        checkBox->setChecked( isChecked( row, col ) );
    }
    connect( checkBox, SIGNAL( toggled(bool) ), this, SLOT( checkBoxToggled() ) );
}

bool CheckBoxMatrix::isChecked(int row, int col) const
{
    qDebug() << matrix_;
    return matrix_[ row ] & (2 << col);
}

void CheckBoxMatrix::setChecked(bool check, int row, int col)
{
    checkBoxMatrix_[ row ][ col ]->setChecked( check );
    if ( check ) {
        matrix_[ row ] |= (2 << col);
    } else {
        matrix_[ row ] &= !(2 << col);
    }
}

void CheckBoxMatrix::setMatrix(QVector< int > matrix)
{
    matrix_ = matrix;
}

QVector< int > CheckBoxMatrix::getMatrix() const
{
    return matrix_;
}

void CheckBoxMatrix::checkBoxToggled()
{
    for (int r = 0; r < checkBoxMatrix_.size(); ++r) {
        for (int c = 0; c < checkBoxMatrix_[ r ].size(); ++c) {
            if ( sender() == checkBoxMatrix_[ r ][ c ] ) {
                bool isChecked = checkBoxMatrix_[ r ][ c ]->isChecked();
                setChecked( isChecked, r, c );
            }
        }
    }
}
