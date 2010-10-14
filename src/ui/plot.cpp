/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
#include <QApplication>

#include "plotwindow_2d.h"

#include "configuration.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setOrganizationName( "Debilski" );
    app.setOrganizationDomain( "debilski.de" );
    app.setApplicationName( "Wellenprogramm" );


    //Configuration config = Config::instance();
    config.read();
    config.debug();

    Waveprogram2DPlot* main = new Waveprogram2DPlot();;

    main->raise();
    main->show();
    main->activateWindow();

    app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );

    return app.exec();
}
