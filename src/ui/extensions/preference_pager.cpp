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
#include <qlayout.h>
#include <qdebug.h>

#include "rightclickable_tool_bar.h"

class PreferencePager::PrivateData {
public:
    PrivateData(PreferencePager* self);
    ~PrivateData();
    void init();

    QAction* toggleViewAction;
    RightclickableToolBar* toolbar;
    QStackedWidget* stack;
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
    stack = new QStackedWidget( self );
    mapper = new QSignalMapper( self );
}

PreferencePager::PreferencePager(QWidget* parent) :
    QMainWindow( parent )
{
    d_data = new PrivateData( this );
    d_data->init();

    QWidget* centralwidget = new QWidget(this);
    centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
    QHBoxLayout* horizontalLayout = new QHBoxLayout(centralwidget);
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));

    horizontalLayout->addWidget(d_data->stack);

    setCentralWidget(centralwidget);

    connect( d_data->mapper, SIGNAL(mapped( int )), d_data->stack, SLOT( setCurrentIndex(int)) );
    connect( d_data->stack, SIGNAL(currentChanged(int)), this, SLOT(resizeToMinimalSize()) );

    d_data->toggleViewAction = new QAction( this );
    d_data->toggleViewAction->setCheckable( true );
    d_data->toggleViewAction->setText( windowTitle() );

    connect( d_data->toggleViewAction, SIGNAL(triggered(bool)), this, SLOT(toggleView(bool)) );
    // connect( this, SIGNAL(visibilityChanged(bool)), d_data->toggleViewAction, SLOT(triggered(bool)) );

    d_data->toolbar = new RightclickableToolBar( "toolbar", this );

    addToolBar( d_data->toolbar );
    d_data->toolbar->setFloatable( false );
    d_data->toolbar->setMovable( false );
    setUnifiedTitleAndToolBarOnMac( true );
}

int PreferencePager::addPage(QWidget* page, const QString& label)
{
    return addPage( page, QIcon(), label );
}

int PreferencePager::addPage(QWidget* page, const QIcon& icon, const QString& label)
{
    if ( !page )
        return -1;
    int index = d_data->stack->insertWidget( -1, page );
    QAction* ac = d_data->toolbar->addAction( icon, label );

    connect( ac, SIGNAL( triggered() ), d_data->mapper, SLOT( map() ) );
    d_data->mapper->setMapping( ac, index );

    return index;
}

void PreferencePager::resizeToMinimalSize()
{
/*    for (int i=0; i<d_data->stack->count(); ++i) {
        if ( d_data->stack->widget(i)->isHidden() ) {
            d_data->stack->widget(i)->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored);
        } else {
            d_data->stack->widget(i)->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred);
        }
    }*/
    this->resize( sizeHint() );
}

QAction* PreferencePager::toggleViewAction() const
{
    return d_data->toggleViewAction;
}

void PreferencePager::toggleView(bool b)
{
    if ( b == isHidden() ) {
        if ( b )
            show();
        else
            close();
    }
}

void PreferencePager::showEvent(QShowEvent * event)
{
    d_data->toggleViewAction->setChecked( true );
    resizeToMinimalSize();
    event->accept();
}

void PreferencePager::closeEvent(QCloseEvent * event)
{
    d_data->toggleViewAction->setChecked( false );
    event->accept();
}

