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

#ifdef Q_OS_DARWIN
    QString libPattern = "*lattice.dylib";
    QString libDir = "builds/darwin/models";
#else
    QString libPattern = "*lattice.so";
    QString libDir = "builds/lomo/models";
#endif

    addOption( "libraryDirectory", libDir, "global/libraryDirectory" ). addCommandLineString(
        "libDir" );

    addOption( "libraryPattern", libPattern, "global/libraryPattern" ). addCommandLineString(
        "libPattern" );

    addOption( "lastModel", "FhnLattice", "global/lastModel" ). addCommandLineString( "model" );
}

Option& Configuration::operator()(const QString& name)
{
    if ( optionList.contains( name ) ) {
        qDebug() << optionList[ name ].value();
        return optionList[ name ];
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
    std::cout << "----" << qPrintable( defaultValue.toString() ) << std::endl;
    Option newOption( name, defaultValue, settingsKey );
    return optionList.insert( name, newOption ).value();

}

void Configuration::debug()
{
    qDebug() << "Registered Variables";
    foreach( Option o, optionList ) {
            qDebug() << o.name() << "Value" << o.value() << "Default" << o.defaultValue();
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
