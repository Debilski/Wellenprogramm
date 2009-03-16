/*
 * option.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef OPTION_H_
#define OPTION_H_

#include <QtCore>

class Option {

private:
    QString name_;
    QVariant value_;
    QVariant defaultValue_;

    QStringList commandLineStrings_;

    QString settingsKey_;

    /**
     * Gibt an, ob der Wert, falls geändert auch in der Config gespeichert werden soll.
     */
    bool saveToConfig_;

    /**
     * Gibt an, ob der Wert eine Änderung durch den User erfahren hat.
     */
    bool changedByUser_;

public:
    Option(const Option& o):
    name_(o.name_), value_(o.value_), defaultValue_(o.defaultValue_), commandLineStrings_(o.commandLineStrings_),
    settingsKey_(o.settingsKey_), saveToConfig_(o.saveToConfig_), changedByUser_(o.changedByUser_)
    {
        qDebug() << "Attempt to copy from" << o.name_ << o.value_;
    }
    Option() :
        name_(), defaultValue_(), settingsKey_(), saveToConfig_( false ), changedByUser_( false )
    {
        qDebug() << "Created empty Option";
    }
    Option(const QString& name, QVariant defaultValue) :
        name_( name ), defaultValue_( defaultValue ), saveToConfig_( false ),
            changedByUser_( false )
    {
        qDebug() << "Created Option" << name;
    }
    Option(const QString& name, QVariant defaultValue, const QString& settingsKey) :
        name_( name ), defaultValue_( defaultValue ), settingsKey_( settingsKey ), saveToConfig_(
            true ), changedByUser_( false )
    {
        qDebug() << "Created Option" << name;
    }

    const QString& name() const
    {
        return name_;
    }

    const QVariant& value() const
    {
        if ( value_.isValid() )
            return value_;
        return defaultValue_;
    }

    const QVariant& defaultValue() const
    {
        return defaultValue_;
    }

    void setValue(const QVariant& value)
    {
        changedByUser_ = true;
        value_ = value;
    }


    QString toString() const;


    Option& addCommandLineString(const QString& commandLineString, bool longFormat = true)
    {

        QString format = longFormat ? "--%1=" : "-%1=";
        commandLineStrings_ << format.arg( commandLineString );
        return *this;

    }

    bool readCommandLine();
    bool readQSettings();

    void read();

    void write();

};

#endif /* OPTION_H_ */
