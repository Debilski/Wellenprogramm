/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * configuration.cpp
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#include "configuration.h"

#include <iostream>
#include <qdebug.h>
#include <qsettings.h>


Configuration::Configuration()
{
    QSettings settings;
// Ist noch nicht Environment-spezifisch.
    QString libDir = "target/models";
#ifdef Q_OS_DARWIN
    QString libPattern = "*lattice.dylib";
#else
    QString libPattern = "*lattice.so";
#endif

    addOption( "libraryDirectory", libDir, "global/libraryDirectory" ). addCommandLineString(
        "libDir" );

    addOption( "libraryPattern", libPattern, "global/libraryPattern" ). addCommandLineString(
        "libPattern" );

    addOption( "last_model", "FhnLattice", "global/lastModel" ). addCommandLineString( "model" );

    addOption( "last_size_x", "128", "global/lastSizeX" ). addCommandLineString( "sizex" );
    addOption( "last_size_y", "128", "global/lastSizeY" ). addCommandLineString( "sizey" );
    addOption( "last_lattice_size_x", "128", "global/lastLatticeSizeX" ). addCommandLineString( "latticesizex" );
    addOption( "last_lattice_size_y", "128", "global/lastLatticeSizeY" ). addCommandLineString( "latticesizey" );
}

Option& Configuration::operator()(const QString& name)
{
    if ( optionList.contains( name ) ) {
        //qDebug() << optionList[ name ].value();
        return optionList[ name ];
    } else {
        qDebug() << "Option not found:" << name;
    }
    return emptyOption;
}

Option Configuration::option(const QString& name) const
{
    return optionList.value( name );
}

Option& Configuration::addOption(const QString& name, const QVariant& defaultValue)
{
    return addOption( name, defaultValue, name );
}

Option& Configuration::addOption(const QString& name, const QVariant& defaultValue,
                                 const QString& settingsKey)
{
    Option newOption( name, defaultValue, settingsKey );
    return optionList.insert( name, newOption ).value();
}

void Configuration::debug()
{
    qDebug() << "Registered Variables";
    foreach( Option o, optionList ) {
            qDebug() << o.name() << o.value() << "Default" << o.defaultValue();
        }
}
void Configuration::read()
{
    for (optionListIterator it = optionList.begin(); it != optionList.end(); ++it) {
        (*it).read();
    }

    /*
     QStringList arguments = QCoreApplication::instance()->arguments();

     QStringList pathList = arguments.filter( "--libdir=" );

     #ifdef Q_OS_DARWIN
     libraryPattern = "*lattice.dylib";
     #else
     libraryPattern = "*lattice.so";
     #endif
     QString testLibDir;
     if ( !pathList.empty() ) {
     testLibDir = pathList.first();
     testLibDir.replace( "--libdir=", "" );
     } else {
     QSettings settings;
     settings.beginGroup( "General" );
     #ifdef Q_OS_DARWIN
     testLibDir = settings.value( "inputSplitter", "builds/darwin/models" ).toString();
     #else
     testLibDir = settings.value( "inputSplitter", "builds/lomo/models" ).toString();
     #endif
     settings.endGroup();
     }
     libraryDirectory = testLibDir;
     */
}

void Configuration::write()
{
    for (optionListIterator it = optionList.begin(); it != optionList.end(); ++it) {
        (*it).write();
    }
}
