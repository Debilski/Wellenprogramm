/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * resettable_label.cpp
 *
 *  Created on: 04.04.2009
 *      Author: rikebs
 */

#include "resetable_label.h"

#include <qmenu.h>

class ResetableLabel::PrivateData {
public:
    PrivateData(ResetableLabel* self);
    ~PrivateData();
    void init();
    ResetableLabel* self_;
    QAction* action;
    QMenu* menu;
};

ResetableLabel::PrivateData::PrivateData(ResetableLabel* self) :
    self_( self )
{
    menu = new QMenu( self_ );
    action = new QAction( tr( "Reset" ), self_ );
    menu->addAction( action );
}

ResetableLabel::PrivateData::~PrivateData()
{
    delete menu;
    delete action;
}

void ResetableLabel::PrivateData::init()
{
    self_->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect(
        self_, SIGNAL( customContextMenuRequested(const QPoint&) ), self_,
        SLOT(showResetMenu(const QPoint&)) );
    QObject::connect( action, SIGNAL( triggered() ), self_, SIGNAL( reset() ) );
}


/*!
 * \class ResetableLabel
 *
 * This class adds an additional context menu to QLabel, providing a single action: „Reset“.
 *
 * When reset is triggered a signal \b reset() is emitted.
 */

ResetableLabel::ResetableLabel(QWidget * parent, Qt::WindowFlags f) :
    QLabel( parent, f )
{
    d_data = new PrivateData( this );
    d_data->init();
}

ResetableLabel::ResetableLabel(const QString & text, QWidget * parent, Qt::WindowFlags f) :
    QLabel( text, parent, f )
{
    d_data = new PrivateData( this );
    d_data->init();
}

ResetableLabel::~ResetableLabel()
{
    delete d_data;
}

void ResetableLabel::showResetMenu(const QPoint& p)
{
    d_data->menu->popup( mapToGlobal( p ) );
}
