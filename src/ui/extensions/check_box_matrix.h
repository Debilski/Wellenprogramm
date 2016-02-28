/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * check_box_matrix.h
 *
 *  Created on: 19.06.2009
 *      Author: rikebs
 */

#ifndef CHECK_BOX_MATRIX_H_
#define CHECK_BOX_MATRIX_H_

#include "qobject.h"
#include "qvector.h"

class QCheckBox;

class CheckBoxMatrix : public QObject
{
    Q_OBJECT
public:
    CheckBoxMatrix(int rows, int cols);
    void associateCheckBox(QCheckBox* checkBox, int row, int col, bool keepStateOfCheckBox);
    bool isChecked(int row, int col) const;
    void setChecked(bool check, int row, int col);
    void setMatrix(QVector<int> matrix);
    QVector<int> getMatrix() const;
protected slots:
    void checkBoxToggled();

private:
    QVector<int> matrix_;
    QVector<QVector<QCheckBox*> > checkBoxMatrix_;
};

#endif /* CHECK_BOX_MATRIX_H_ */
