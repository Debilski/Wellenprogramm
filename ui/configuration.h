/*
 * configuration.h
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <QtCore>

#include "option.h"

class Configuration : public QObject {

    Q_OBJECT
public:
    /**
     * Erzeugt eine neue Instanz oder gibt die bereits erzeugte Instanz zurück.
     */
    static Configuration* instance()
    {
      if ( instance_ == 0 ) // if first time ...
      {
        instance_ = new Configuration(); // ... create a new 'solo' instance
      }
      refCount_++; //increase reference counter
      return instance_;
    }

    static void release();
    static void destroy();
    ~Configuration() {}
    void read() {
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


    }
    void write() {}
protected:

    Configuration() :
    libraryDirectory( QString("libDir"), QString(".") ),
    libraryPattern(QString( "libDir"), QString(".") )
    {
        QSettings settings;
    }

    Configuration(const Configuration&); /* verhindert, dass eine weitere Instanz via
   Kopie-Konstruktor erstellt werden kann */
    Configuration& operator=(const Configuration &); //Verhindert weitere Instanz durch Kopie
    static Configuration* instance_;
    static int refCount_;


public:
    Option<QString> libraryDirectory;
    Option<QString> libraryPattern;
};

#endif /* CONFIGURATION_H_ */
