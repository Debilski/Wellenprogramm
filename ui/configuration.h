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
    void read() {}
    void write() {}
protected:

    Configuration(); // verhindert, das ein Objekt von außerhalb von N erzeugt wird.
  // protected, wenn man von der Klasse noch erben möchte
    Configuration(const Configuration&); /* verhindert, dass eine weitere Instanz via
   Kopie-Konstruktor erstellt werden kann */
    Configuration& operator=(const Configuration &); //Verhindert weitere Instanz durch Kopie
    static Configuration* instance_;
    static int refCount_;
};

#endif /* CONFIGURATION_H_ */
