/*
 * option.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef OPTION_H_
#define OPTION_H_

#include <QString>

template <typename T>
class Option {

private:
    QString name_;
    QString description_;
    QString helpText_;
    T value_;
    T defaultValue_;

public:
    Option( const QString& name, T value, const QString& description ) :
        name_(name), value_(value), description_(description)
    {

    }

    const QString& name() { return name_; }
    const T& value() { return value_; }
    void setValue( const T& value ) { value_ = value; }
    void operator=( const T& value ) { value_ = value; }

};

#endif /* OPTION_H_ */
