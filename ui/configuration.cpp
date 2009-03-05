/*
 * configuration.cpp
 *
 *  Created on: 05.03.2009
 *      Author: rikebs
 */

#include "configuration.h"

template <typename T_singleton>
class SingletonCleaner {
public:
    ~SingletonCleaner()
    {
        T_singleton::destroy();
    }
};

/*
class ConfigurationCleaner {
public:
  ~ConfigurationCleaner()
  {
    Configuration::destroy();
  }
} ConfigurationCleanerInst;
*/

SingletonCleaner< Configuration > ConfigurationCleaner;

// static members
//
Configuration* Configuration::instance_ = 0;
int Configuration::refCount_ = 0;


/**
 * Reduziert die Referenzen um eins und zerstört den Singleton, wenn keine mehr vorhanden sind.
 */
void Configuration::release()
{
  if ( --refCount_ < 1 ) {
    destroy();
  }
}

/**
 * Zerstört den Singleton
 */
void Configuration::destroy()
{
  if ( instance_ != 0 ) {
    delete (instance_);
    instance_ = 0;
  }
}
