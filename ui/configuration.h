/*
 * configuration.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <QtCore>

#include <boost/noncopyable.hpp>
#include <boost/thread/once.hpp>

#include <iostream>

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

    //Q_OBJECT

public:

    Configuration()
    {
        QSettings settings;
        addOption( "libraryDirectory", "builds/darwin/models", "global/libraryDirectory" ). addCommandLineString(
            "libDir" );

        addOption( "libraryPattern", "*lattice.dylib", "global/libraryPattern" ). addCommandLineString(
            "libPattern" );

    }
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

    QHash< QString, Option > optionList;
    typedef QHash< QString, Option >::iterator optionListIterator;

public:
    Option emptyOption;
private:
    Configuration(const Configuration&);
};

#define config Configuration::instance()

#endif /* CONFIGURATION_H_ */
