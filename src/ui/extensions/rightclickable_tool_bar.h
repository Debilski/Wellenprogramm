/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * rightclickable_tool_bar.h
 *
 *  Created on: 03.06.2009
 *      Author: rikebs
 */

#ifndef RIGHTCLICKABLE_TOOL_BAR_H_
#define RIGHTCLICKABLE_TOOL_BAR_H_

#include <qtoolbar.h>

class RightclickableToolBar : public QToolBar
{
    Q_OBJECT
public:
    RightclickableToolBar(const QString& title, QWidget* parent = 0);
    RightclickableToolBar(QWidget* parent = 0);
    ~RightclickableToolBar();
public slots:
    void changeToolButtonStyle(int style);
    void toolBarCustomContextMenu(const QPoint& pos);

private:
    void init();
    class PrivateData;
    PrivateData* d_data;
};

#endif /* RIGHTCLICKABLE_TOOL_BAR_H_ */
