/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * resetable_label.h
 *
 *  Created on: 04.04.2009
 *      Author: rikebs
 */

#ifndef RESETABLE_LABEL_H_
#define RESETABLE_LABEL_H_

#include <qlabel.h>

class ResetableLabel : public QLabel {
Q_OBJECT
public:
    ResetableLabel(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ResetableLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~ResetableLabel();
signals:
    void reset();
private:
    class PrivateData;
    PrivateData* d_data;

private slots:
    void showResetMenu(const QPoint& p);
};

#endif /* RESETTABLE_LABEL_H_ */
