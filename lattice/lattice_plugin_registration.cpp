/*
 * lattice_plugin_registration.cpp
 *
 *  Created on: 22.12.2008
 *      Author: rikebs
 */


#include "lattice_plugin_registration.h"
#include "singleton_helper.h"


/**
 * Zerstört beim Beenden LatticePluginRegistration.
 */
SingletonCleaner< LatticePluginRegistration > LatticePluginRegistrationCleanerInst;

//
// static members
//
LatticePluginRegistration* LatticePluginRegistration::instance_ = 0;
int LatticePluginRegistration::refCount_ = 0;


/**
 * Reduziert die Referenzen um eins und zerstört den Wrapper, wenn keine mehr vorhanden sind.
 */
void LatticePluginRegistration::release()
{
  if ( --refCount_ < 1 ) {
    destroy();
  }
}

/**
 * Zerstört den Wrapper
 */
void LatticePluginRegistration::destroy()
{
  if ( instance_ != 0 ) {
    delete (instance_);
    instance_ = 0;
  }
}


LatticePluginRegistration::LatticePluginRegistration()
{
  std::cout << "LatticePluginRegistration Constructor" << std::endl;
}

LatticePluginRegistration::~LatticePluginRegistration()
{
  std::cout << "LatticePluginRegistration Destructor" << std::endl;
}


void LatticePluginRegistration::registerModel(std::string name, LatticeMakerFnc maker, LatticeDestroyerFnc destroyer)
{
    // register the maker with the factory
    factory()[ name ] = LatticeHandlerFnc( maker, destroyer );
    std::cout << "Registered ‘" << name << "’ Model" << std::endl;
    std::cout << "Factory size now: " << factory().size() << std::endl;
}

std::list< std::string > LatticePluginRegistration::models() {
    std::list< std::string > keys;
    for ( T_factory::iterator it = factory().begin(); it != factory().end(); ++it ) {
        keys.push_back( (*it).first );
    }
    return keys;
}

LatticeMakerFnc* LatticePluginRegistration::getMakerByName(std::string name)
{
    T_factory::iterator result = factory().find( name );
    if ( result != factory().end() )
        return result->second.first;
    else
        return 0;
}

LatticeDestroyerFnc* LatticePluginRegistration::getDestroyerByName(std::string name)
{
    T_factory::iterator result = factory().find( name );
    if ( result != factory().end() )
        return result->second.second;
    else
        return 0;
}

