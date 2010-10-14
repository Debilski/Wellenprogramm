/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * configuration.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <qhash.h>
#include <qstring.h>

#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>


#include "option.h"

template<typename T>
struct singleton : private boost::noncopyable {
public:
    static T & instance()
    {
        boost::call_once( call_me_once, once_ );
        return instance_helper();
    }
protected:
    static boost::once_flag once_;
    static void call_me_once()
    {
        instance_helper();
    }

    static T & instance_helper()
    {
        static T t;
        return t;
    }
    singleton()
    {
    }
    ~singleton()
    {
    }
};
template<typename T>
boost::once_flag singleton< T >::once_ = BOOST_ONCE_INIT;

class Configuration : /*public QObject, */ public singleton< Configuration > {

public:

    Configuration();
    ~Configuration()
    {
    }
public:

    void debug();
    void read();
    void write();

    Option& addOption(const QString& name, const QVariant& defaultValue);
    Option
        & addOption(const QString& name, const QVariant& defaultValue, const QString& settingsKey);

    Option& operator()(const QString& name);
    Option option(const QString& name) const;

public:
    Option emptyOption;
private:
    Configuration(const Configuration&);
    QHash< QString, Option > optionList;
    typedef QHash< QString, Option >::iterator optionListIterator;
};

#define config Configuration::instance()

#endif /* CONFIGURATION_H_ */
