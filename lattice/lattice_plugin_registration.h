/*
 * lattice_plugin_registration.h
 *
 *  Created on: 22.12.2008
 *      Author: rikebs
 */

#ifndef LATTICE_PLUGIN_REGISTRATION_H_
#define LATTICE_PLUGIN_REGISTRATION_H_

#include <list>
#include "lattice_interface.h"



#define LATTICE_REGISTER_MODEL( model_class ) LATTICE_REGISTER_MODEL( model_class, model_class )

#define LATTICE_REGISTER_MODEL2( model_name, model_class )                                          \
    extern "C" {                                                                                    \
    LatticeInterface* model_class##_maker(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) \
    {                                                                                               \
        return new model_class( sizeX, sizeY, latticeSizeX, latticeSizeY );                         \
    }                                                                                               \
    void model_class##_destroyer( LatticeInterface* l )                                             \
    {                                                                                               \
        delete l;                                                                                   \
    }                                                                                               \
    class model_class##_proxy {                                                                     \
        LatticePluginRegistration* i;                                                               \
    public:                                                                                         \
        model_class##_proxy()                                                                       \
        {               std::cout << "hi from " << #model_class << "_proxy !" << std::endl;         \
            i = LatticePluginRegistration::instance();                                              \
            i->registerModel( model_name , model_class##_maker, model_class##_destroyer );          \
        }                                                                                           \
    };                                                                                              \
    static model_class##_proxy model_class##_p;                                                     \
    }




typedef LatticeInterface* LatticeMakerFnc(int, int, int, int);
typedef void LatticeDestroyerFnc( LatticeInterface* );

/**
 * Singleton-Klasse zur Verwaltung der Lattice-Plugins.
 *
 * Todo: Klasse muss noch ausgebaut werden, um Modelle zu deleten. Wenn dies im aufrufenden
 * Code geschieht, kann unter Umständen ein falsches delete benutzt werden.
 *
 * Außerdem Möglichkeit zur De-Registrierung bieten und automatische Verweiszähler?
 */
class LatticePluginRegistration {
public:
    typedef std::pair< LatticeMakerFnc*, LatticeDestroyerFnc* > LatticeHandlerFnc;
    typedef std::map< std::string, LatticeHandlerFnc, std::less< std::string > > T_factory;

    static LatticePluginRegistration* instance()
    {
        std::cout << "Instance " << instance_ << std::endl;
        if ( instance_ == 0 ) // if first time ...
        {
            instance_ = new LatticePluginRegistration(); // ... create a new 'solo' instance
        }
        refCount_++; //increase reference counter
        return instance_;
    }

    void registerModel(std::string name, LatticeMakerFnc maker, LatticeDestroyerFnc destroyer);
    std::list< std::string > models();
    LatticeMakerFnc* getMakerByName(std::string name);
    LatticeDestroyerFnc* getDestroyerByName(std::string name);

    static void release();
    static void destroy();
    ~LatticePluginRegistration();
protected:
    /**
     * Hält die Maker-Funktionen in einer std::map.
     *
     * Wegen des Problems der statischen Initialisierung als statische Methode implementiert.
     */
    static T_factory& factory() {
        static LatticePluginRegistration::T_factory f = LatticePluginRegistration::T_factory();
        return f;
    }

    static LatticePluginRegistration* instance_;
    static int refCount_;
    LatticePluginRegistration(); // verhindert, das ein Objekt von außerhalb von N erzeugt wird.
    // protected, wenn man von der Klasse noch erben möchte
    LatticePluginRegistration(const LatticePluginRegistration&); /* verhindert, dass eine weitere Instanz via
     Kopie-Konstruktor erstellt werden kann */
    LatticePluginRegistration& operator=(const LatticePluginRegistration &); //Verhindert weitere Instanz durch Kopie

};

#endif /* LATTICE_PLUGIN_REGISTRATION_H_ */
