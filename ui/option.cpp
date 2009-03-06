/*
 * option.cpp
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#include "option.h"

#include <iostream>


/*!
 \class Option
 \brief Stellt eine Option dar, welche gelesen und gespeichert werden kann.
 */

QString Option::toString() const
{
    return value().toString();
}

bool Option::readCommandLine()
{
    qDebug() << "read Command Line" << name_;
    if ( commandLineStrings_.isEmpty() )
        return false;

    QStringList arguments = QCoreApplication::instance()->arguments();
    QStringList filteredArgumentsList;


    foreach( QString command, commandLineStrings_ ) {
        QStringList filtered = arguments.filter( command );


        foreach( QString s, filtered ) {
            filteredArgumentsList << s.replace( command, "" );
        }

    }

    if ( filteredArgumentsList.isEmpty() )
        return false;


    value_ = filteredArgumentsList;
    qDebug() << value_ << value_.isValid();

    return true;
}

bool Option::readQSettings()
{
    qDebug() << "read QSettings" << name_;
    if ( !settingsKey_.isEmpty() ) {
            QSettings settings;
            value_ = settings.value( settingsKey_, defaultValue_ );
        }
    return value_.isValid();

}

void Option::read()
{

    if ( readCommandLine() ) {
        qDebug() << "Value" << name_ << value_ << defaultValue_;
        return;
    }


    if (readQSettings() )
        return;

    qDebug() << "Value" << name_ << value_ << defaultValue_;
    // Fallback
    value_ = defaultValue_;

    qDebug() << "Value" << name_ << value_ << defaultValue_;
}

void Option::write()
{
    if ( !(saveToConfig_ && changedByUser_) )
        return;

    QSettings settings;
    settings.setValue( settingsKey_, value_ );

}
