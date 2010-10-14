/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * detachable_tab_widget.h
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#ifndef DETACHABLE_TAB_WIDGET_H_
#define DETACHABLE_TAB_WIDGET_H_

#include <qtabwidget.h>

class DetachableTabWidget : public QTabWidget {
Q_OBJECT
public:
    DetachableTabWidget(QWidget* parent = 0);
    ~DetachableTabWidget();
public slots:
    void showTabMenu(const QPoint& p);
signals:
    void detached(int index);
private:
    class PrivateData;
    PrivateData* d_data;
};

#endif /* DETACHABLE_TAB_WIDGET_H_ */
