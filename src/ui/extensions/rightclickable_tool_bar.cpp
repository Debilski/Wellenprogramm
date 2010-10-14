/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * rightclickable_tool_bar.cpp
 *
 *  Created on: 03.06.2009
 *      Author: rikebs
 */

#include "rightclickable_tool_bar.h"

#include <qmenu.h>
#include <qsignalmapper.h>

class RightclickableToolBar::PrivateData {
public:
    QMenu toolBarRightClickMenu;
};

RightclickableToolBar::RightclickableToolBar(const QString & title, QWidget * parent) :
    QToolBar( title, parent ), d_data(new PrivateData)
{
    init();
}

RightclickableToolBar::RightclickableToolBar(QWidget * parent) :
    QToolBar( parent ), d_data(new PrivateData)
{
    init();
}

RightclickableToolBar::~RightclickableToolBar()
{
    delete d_data;
}

void RightclickableToolBar::init()
{
    setIconSize( QSize( 32, 32 ) );
    setToolButtonStyle( Qt::ToolButtonTextUnderIcon );

    typedef QPair<QString, Qt::ToolButtonStyle> T_toolBarString;
    QList< T_toolBarString > listPairs;
    listPairs << T_toolBarString( tr("Only Icons"), Qt::ToolButtonIconOnly );
    listPairs << T_toolBarString( tr("Only Text"), Qt::ToolButtonTextOnly );
    listPairs << T_toolBarString( tr("Text under Icon"), Qt::ToolButtonTextUnderIcon );
    listPairs << T_toolBarString( tr("Text beside Icon"), Qt::ToolButtonTextBesideIcon );
    QSignalMapper* toolBarRightClickMapper = new QSignalMapper(this);

    connect( toolBarRightClickMapper, SIGNAL(mapped( int )), this, SLOT( changeToolButtonStyle( int )) );

    foreach( T_toolBarString s, listPairs ) {
        QAction* action = new QAction( s.first, this );
        d_data->toolBarRightClickMenu.addAction(action);
        connect( action, SIGNAL( triggered() ), toolBarRightClickMapper, SLOT( map() ) );
        toolBarRightClickMapper->setMapping( action, s.second );
    }

    setContextMenuPolicy( Qt::CustomContextMenu );

    connect( this, SIGNAL( customContextMenuRequested ( const QPoint&  ) ), this, SLOT( toolBarCustomContextMenu ( const QPoint& ) ));
}

void RightclickableToolBar::changeToolButtonStyle( int style )
{
    setToolButtonStyle( static_cast<Qt::ToolButtonStyle>(style) );
}

void RightclickableToolBar::toolBarCustomContextMenu ( const QPoint & pos )
{
    d_data->toolBarRightClickMenu.popup( mapToGlobal( pos ) );
}
