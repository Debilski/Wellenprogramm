/*
 * option.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef OPTION_H_
#define OPTION_H_

#include <qstring.h>
#include <qstringlist.h>
#include <qvariant.h>

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
    Option(const Option& o);
    Option();
    Option(const QString& name, QVariant defaultValue);
    Option(const QString& name, QVariant defaultValue, const QString& settingsKey);

    const QString& name() const;
    const QVariant& value() const;
    const QVariant& defaultValue() const;

    void setValue(const QVariant& value);

    QString toString() const;


    Option& addCommandLineString(const QString& commandLineString, bool longFormat = true);
    bool readCommandLine();
    bool readQSettings();

    void read();

    void write();

};

#endif /* OPTION_H_ */
