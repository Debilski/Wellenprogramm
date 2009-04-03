/*
 * option.cpp
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#include "option.h"

#include <qcoreapplication.h>
#include <qsettings.h>
#include <qdebug.h>

/*!
 \class Option
 \brief Stellt eine Option dar, welche gelesen und gespeichert werden kann.
 */

    Option::Option(const Option& o):
    name_(o.name_), value_(o.value_), defaultValue_(o.defaultValue_), commandLineStrings_(o.commandLineStrings_),
    settingsKey_(o.settingsKey_), saveToConfig_(o.saveToConfig_), changedByUser_(o.changedByUser_)
    {
        qDebug() << "Attempt to copy from" << o.name_ << o.value_;
    }
    Option::Option() :
        name_(), defaultValue_(), settingsKey_(), saveToConfig_( false ), changedByUser_( false )
    {
        qDebug() << "Created empty Option";
    }
    Option::Option(const QString& name, QVariant defaultValue) :
        name_( name ), defaultValue_( defaultValue ), saveToConfig_( false ),
            changedByUser_( false )
    {
        qDebug() << "Created Option" << name;
    }
    Option::Option(const QString& name, QVariant defaultValue, const QString& settingsKey) :
        name_( name ), defaultValue_( defaultValue ), settingsKey_( settingsKey ), saveToConfig_(
            true ), changedByUser_( false )
    {
        qDebug() << "Created Option" << name;
    }

    const QString& Option::name() const
    {
        return name_;
    }

    const QVariant& Option::value() const
    {
        if ( value_.isValid() )
            return value_;
        return defaultValue_;
    }

    const QVariant& Option::defaultValue() const
    {
        return defaultValue_;
    }

    void Option::setValue(const QVariant& value)
    {
        changedByUser_ = true;
        value_ = value;
    }


    Option& Option::addCommandLineString(const QString& commandLineString, bool longFormat /*= true*/ )
    {

        QString format = longFormat ? "--%1=" : "-%1=";
        commandLineStrings_ << format.arg( commandLineString );
        return *this;

    }

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
    if ( !(saveToConfig_ && changedByUser_) ) {

        qDebug() << "Do not write value" << settingsKey_ << value_ << saveToConfig_ << changedByUser_;
        return;
    }
    qDebug() << "Write value" << settingsKey_ << value_;
    QSettings settings;
    settings.setValue( settingsKey_, value_ );

}
