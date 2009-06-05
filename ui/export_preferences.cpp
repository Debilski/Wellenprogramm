/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * export_preferences.cpp
 *
 *  Created on: 04.06.2009
 *      Author: rikebs
 */

#include "export_preferences.h"

#include <qgridlayout.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qtoolbar.h>
#include <qaction.h>
#include <qdebug.h>
#include <qgroupbox.h>

class ExportPreferences::PrivateData {
public:
    QGridLayout* layout;
    QWidget* pngInnerWidget;
    QGroupBox* groupBox;
};

ExportPreferences::ExportPreferences(QWidget* parent) :
    PreferencePager( parent ), d_data( new PrivateData() )
{

    pngExport = new QWidget( parent );
    QVBoxLayout* vlayout = new QVBoxLayout( pngExport );

    d_data->groupBox = new QGroupBox( tr("Export only the checked views"), pngExport );
    vlayout->addWidget( d_data->groupBox );
    d_data->layout = new QGridLayout( d_data->groupBox );
    d_data->layout->setSizeConstraint( QLayout::SetFixedSize );
    //setLayout( d_data->layout );

    addPage( pngExport, QIcon( ":/icons/icons/photo.svg" ), "PNG" );
    QWidget* p = new QWidget( parent );

    QGridLayout* l = new QGridLayout( p );
    l->addWidget(new QLabel( "1" ));
    l->addWidget(new QLabel( "2" ));
    l->addWidget(new QLabel( "3" ));
    l->addWidget(new QLabel( "4" ));

    addPage( p, QIcon( ":/icons/icons/matlab.svg" ), "Matlab" );
}

void ExportPreferences::setViewNames(const QStringList& names)
{

    QLayoutItem *l;
    d_data->layout->setEnabled(false);
    while ((l = d_data->layout->takeAt(0))) {
        delete l->widget();
        delete l;
    }

    d_data->layout->addWidget( new QLabel( tr("raw"), d_data->groupBox ), 0, 1 );
    d_data->layout->addWidget( new QLabel( tr("full"), d_data->groupBox ), 0, 2 );
    int i = 1;
    foreach (QString name, names)
        {
            d_data->layout->addWidget( new QLabel( name, d_data->groupBox ), i, 0 );
            QCheckBox* box = new QCheckBox( d_data->groupBox );
            box->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
            d_data->layout->addWidget( box, i, 1 );
            QCheckBox* box1 = new QCheckBox( d_data->groupBox );
            box1->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );
            d_data->layout->addWidget( box1, i, 2 );
            ++i;
        }
    d_data->layout->setEnabled(true);
    d_data->layout->update();
    resizeToMinimalSize();
}
