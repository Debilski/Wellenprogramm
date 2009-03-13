/*
 * singleton_helper.h
 *
 *  Created on: 14.03.2009
 *      Author: rikebs
 */

#ifndef SINGLETON_HELPER_H_
#define SINGLETON_HELPER_H_

//
// this is a yabo(r) trick to ensure
// the singleton destruction on
// program termination
//


/**
 * Erzeugt eine Hilfsklasse, deren einzige Aufgabe es ist, dass sie beim
 * Beenden des Programms Zerstören auch den Singleton zerstört.
 */

template<typename T_singleton>
class SingletonCleaner {
public:
    ~SingletonCleaner()
    {
        T_singleton::destroy();
    }
};

#endif /* SINGLETON_HELPER_H_ */
