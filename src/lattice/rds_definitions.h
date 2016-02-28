/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * rds_definitions.h
 *
 *  Created on: 25.11.2008
 *      Author: rikebs
 */

#ifndef RDS_DEFINITIONS_H_
#define RDS_DEFINITIONS_H_


typedef unsigned int uint;

/*#ifdef COMPONENTS_AS_ENUM
enum EnumComponents {
    firstComponent = 0,
    secondComponent = 1,
    thirdComponent = 2,
    fourthComponent = 3,
    fifthComponent = 4,
    sixthComponent = 5,
    seventhComponent = 6,
    eigthComponent = 7,
    ninthComponent = 8,
    tenthComponent = 9,
    eleventhComponent = 10
};
typedef EnumComponents T_component;

#else
*/
const int firstComponent = 0;
const int secondComponent = 1;
const int thirdComponent = 2;
const int fourthComponent = 3;
const int fifthComponent = 4;
const int sixthComponent = 5;
const int seventhComponent = 6;
const int eigthComponent = 7;
const int ninthComponent = 8;
const int tenthComponent = 9;
const int eleventhComponent = 10;
typedef uint T_component;

//#endif

/**
 * Definiert Standardwerte für Optimierungsoptionen, welche in der Integrationsroutine verwendet werden können.
 *
 * Standardmäßig sind alle Optimierungen AUS.
 */
struct LatticeOptimisations
{
    //! Schaltet die externe Kraft aus: step_h() und external_force() werden ggf ignoriert.
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = false;
    //! Schaltet das multiplikative Rauschen aus: step_g() wird ggf ignoriert.
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = false;
    //! Schaltet das Ermitteln der Cluster aus
    static const bool OPTIMISE_NO_CLUSTER_COUNT = false;
};

/**
 * Definiert einige Standardkonstanten, welche dann von der Modell-Implementierung überschrieben
 * werden können.
 */
struct MetainfoBase : public LatticeOptimisations
{
    /**
     * Gibt das Gitter an, für welches die Cluster gezählt werden sollen.
     */
    static const int CLUSTER_COUNTER_LATTICE = 0;
    /**
     * Gibt die Zahl der unabhängigen Zufallsarrays an, die erzeugt werden sollen.
     */
    static const int number_of_Noise_Variables = 0;
    /**
     * Gibt die Zahl der Iterationen an, die im Semi-impliziten-Wechselwirkungsbild-Algorithmus
     * verwendet werden sollen.
     */
    static const int SIIP_ITERATIONS = 3;
    /**
     * Gibt an, ob es eine explizite Step-Funktion gibt, die genutzt werden soll
     */
    static const bool HAS_EXPLICIT_STEP_FUNCTION = false;
};


#endif /* RDS_DEFINITIONS_H_ */
