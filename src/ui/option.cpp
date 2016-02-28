/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
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

Option::Option(const Option& o) : name_(o.name_), value_(o.value_), defaultValue_(o.defaultValue_), commandLineStrings_(o.commandLineStrings_), settingsKey_(o.settingsKey_), saveToConfig_(o.saveToConfig_), changedByUser_(o.changedByUser_)
{
}

Option::Option() : name_(), defaultValue_(), settingsKey_(), saveToConfig_(false), changedByUser_(false)
{
}

Option::Option(const QString& name, QVariant defaultValue) : name_(name), defaultValue_(defaultValue), saveToConfig_(false), changedByUser_(false)
{
}

Option::Option(const QString& name, QVariant defaultValue, const QString& settingsKey) : name_(name), defaultValue_(defaultValue), settingsKey_(settingsKey), saveToConfig_(true), changedByUser_(false)
{
}

const QString& Option::name() const
{
    return name_;
}

const QVariant& Option::value() const
{
    if (value_.isValid())
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


Option& Option::addCommandLineString(const QString& commandLineString, bool longFormat /*= true*/)
{

    QString format = longFormat ? "--%1=" : "-%1=";
    commandLineStrings_ << format.arg(commandLineString);
    return *this;
}

QString Option::toString() const
{
    return value().toString();
}

bool Option::readCommandLine()
{
    // qDebug() << "read Command Line" << name_;
    if (commandLineStrings_.isEmpty())
        return false;

    QStringList arguments = QCoreApplication::instance()->arguments();
    QStringList filteredArgumentsList;


    foreach (QString command, commandLineStrings_) {
        QStringList filtered = arguments.filter(command);


        foreach (QString s, filtered) {
            filteredArgumentsList << s.replace(command, "");
        }
    }

    if (filteredArgumentsList.isEmpty())
        return false;


    value_ = filteredArgumentsList;
    //qDebug() << value_ << value_.isValid();

    return true;
}

bool Option::readQSettings()
{
    //qDebug() << "read QSettings" << name_;
    if (!settingsKey_.isEmpty()) {
        QSettings settings;
        value_ = settings.value(settingsKey_, defaultValue_);
    }
    return value_.isValid();
}

void Option::read()
{

    if (readCommandLine()) {
        return;
    }

    if (readQSettings())
        return;

    // Fallback
    value_ = defaultValue_;
}

void Option::write()
{
    if (!(saveToConfig_ && changedByUser_)) {
        return;
    }
    QSettings settings;
    settings.setValue(settingsKey_, value_);
}
