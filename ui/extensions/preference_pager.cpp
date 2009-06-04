/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * preference_pager.cpp
 *
 *  Created on: 04.06.2009
 *      Author: rikebs
 */

#include "preference_pager.h"

#include <qstackedwidget.h>
#include <qaction.h>
#include <qevent.h>
#include <qsignalmapper.h>

#include "rightclickable_tool_bar.h"

class PreferencePager::PrivateData {
public:
    PrivateData(PreferencePager* self);
    ~PrivateData();
    void init();

    QAction* toggleViewAction;
    RightclickableToolBar* toolbar;
    QStackedWidget *stack;
    QSignalMapper* mapper;
    PreferencePager* self;
};

PreferencePager::PrivateData::PrivateData(PreferencePager* self) :
    stack( 0 ), self( self )
{
}

PreferencePager::PrivateData::~PrivateData()
{
}

void PreferencePager::PrivateData::init()
{
    stack = new QStackedWidget(self);
    mapper = new QSignalMapper(self);
}

PreferencePager::PreferencePager(QWidget* parent) :
    QMainWindow( parent )
{
    // connect( toolBox, SIGNAL(currentChanged(int)), this, SLOT(resizeToMinimalSize()) );

    d_data = new PrivateData(this);
    d_data->init();

    connect( d_data->mapper, SIGNAL(mapped( int )), d_data->stack, SLOT( setCurrentIndex(int)) );

    d_data->toggleViewAction = new QAction( this );
    d_data->toggleViewAction->setCheckable( true );
    d_data->toggleViewAction->setText( windowTitle() );
    connect( d_data->toggleViewAction, SIGNAL(triggered(bool)), this, SLOT(toggleView(bool)) );
    connect( this, SIGNAL(visibilityChanged(bool)), d_data->toggleViewAction, SLOT(triggered(bool)));

    d_data->toolbar = new RightclickableToolBar( "toolbar", this );

    addToolBar(d_data->toolbar);
    setUnifiedTitleAndToolBarOnMac(true);

}

int PreferencePager::addPage(QWidget* page, const QString& label)
{
    return addPage(page, QIcon(), label);
}

int PreferencePager::addPage(QWidget* page, const QIcon& icon, const QString& label)
{
    if (!page)
        return -1;
    int index = d_data->stack->insertWidget(-1, page);
    QAction* ac = d_data->toolbar->addAction(icon, label);

    connect( ac, SIGNAL( triggered() ), d_data->mapper, SLOT( map() ));
    d_data->mapper->setMapping(ac, index);

    return index;
}

void PreferencePager::resizeToMinimalSize()
{
    this->resize( sizeHint() );
}

QAction* PreferencePager::toggleViewAction() const
{
    return d_data->toggleViewAction;
}

void PreferencePager::toggleView(bool b)
{
    if (b == isHidden()) {
        if (b)
            show();
        else
            close();
    }
}

void PreferencePager::showEvent ( QShowEvent * event )
{
    d_data->toggleViewAction->setChecked(true);
    event->accept();
}

void PreferencePager::closeEvent ( QCloseEvent * event )
{
    d_data->toggleViewAction->setChecked(false);
    event->accept();
}

