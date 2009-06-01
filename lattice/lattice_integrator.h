/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * lattice_integrator.h
 *
 *  Created on: 21.11.2008
 *      Author: rikebs
 */

#ifndef LATTICE_INTEGRATOR_H_
#define LATTICE_INTEGRATOR_H_

#ifndef RDS_LATTICE_H_
//#error must be included via rds_lattice.h
#include "rds_lattice.h"
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

//TODO Much refactoring is needed



template<int N>
struct _tvec {
    typedef blitz::TinyVector< double, N > TinyVectorWithZero;
};

template<>
struct _tvec< 0 > {
    typedef void TinyVectorWithZero;
};


/**
 * This class is intended to call the needed substeps in the integration process.
 */
template<typename T_model>
class LatticeIntegrator : public Lattice< T_model >{
public:
    LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
        Lattice< T_model > ( sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
    }
    /*
     * Erstes Schema:
     * - Abfragen, ob strikt oder ob mehrere steps auf einmal gehen.
     * - Aufrufe:
     *   - Virtual
     *   - CRTP
     *   - boost-signals
     * - Abfragen, ob man variable Zeitschritte hat oder diese fest ist (Game of Life)
     * - Views hinzufügen?
     * - Initial Conditions verbessern
     * - Wie mit allgemeiner Diffusion umgehen?
     * - Implicit
     *
     * mainloop
     *
     * normalise
     * before_integration
     *
     * integrate
     * - step
     *
     * after_integration
     *
     */

};



/**
 * Eine Hilfsklasse, welche es erlaubt, Schleifen zur Übersetzungszeit zu generieren.
 */
template<typename T_model, unsigned int N_steps>
struct LatticeIntegratorLoop {
    typedef blitz::TinyVector< double,
        T_model::number_of_Noise_Variables ? T_model::number_of_Noise_Variables : 1 > T_noiseVector;
    typedef blitz::TinyVector< double, T_model::number_of_Variables > T_componentsVector;

    /**
     * Anhand der Informationen im NoiseMapping-Template wird ein Komponentenvector aus dem Noisevector erzeugt.
     */
    static void noiseMapping(T_componentsVector& noiseOut, T_noiseVector& noiseIn,
                             double multiplicator)
    {
        int mapping = Metainfo< T_model >::template NoiseMapping< N_steps - 1 >::value;
        if ( mapping >= 0 )
            noiseOut[ N_steps - 1 ] = multiplicator * noiseIn[ mapping ];
        LatticeIntegratorLoop< T_model, N_steps - 1 >::noiseMapping(
            noiseOut, noiseIn, multiplicator );
    }
};

template<typename T_model>
struct LatticeIntegratorLoop< T_model, 0 > {
    // Work-Around, damit es bei Fehlen von Rauschen keine Fehlermeldungen gibt.
    typedef blitz::TinyVector< double,
        T_model::number_of_Noise_Variables ? T_model::number_of_Noise_Variables : 1 > T_noiseVector;
    typedef blitz::TinyVector< double, T_model::number_of_Variables > T_componentsVector;

    static void noiseMapping(T_componentsVector&, T_noiseVector&, double)
    {
    }
};


#define LOOP_COMPONENTS for (uint component = 0; component < Components::number_of_Variables; ++component)
#define LOOP_COMPONENTS_ARG( component ) for (uint component = 0; component < Components::number_of_Variables; ++component)




#include "lattice_integrator.cpp"

#endif /* LATTICE_INTEGRATOR_H_ */
