/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * siip_lattice_integrator.cpp
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */


#ifndef SIIP_LATTICE_INTEGRATOR_CPP
#define SIIP_LATTICE_INTEGRATOR_CPP

#include "siip_lattice_integrator.h"


/**
 * Reaction method.
 *
 *  This method handles accessing the child class and calls the integration for each and
 *  every latticePoint there. As this
 *  function is not virtually linked, it is possible to be inlined, which makes sense for
 *  integration schemes with only a small overhead.
 *
 *  This special implementation integrates semi-implicitely using an iteration for root-finding.
 *  The iteration stops when the values in the dt-part are smaller than…
 *
 *  Das Rauschen wurde vorab berechnet und in der Funktion über getPrecomputedNoise abgerufen
 *
 * \param latticePoint  Current index on the lattice
 */
template<typename T_model>
inline void SIIP_LatticeIntegrator< T_model >::step_dt(long int latticePoint)
{
    blitz::TinyVector< double, Components::number_of_Variables > initial;
    blitz::TinyVector< double, Components::number_of_Variables > values;

    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        initial[ component ] = Base::latticeDataPointer[ component ][ latticePoint ];
        values[ component ] = initial[ component ];
    }

    // noiseVector: xi = Delta W
    T_componentsVector xi;
    xi = 0;

    // noiseVector mapping
    double intensity = Base::noiseIntensity();
    if ( intensity != 0 ) {
        T_noiseVector noise;
        for (uint noiseComponents = 0; noiseComponents < Base::number_of_Noise_Variables; ++noiseComponents)
        {
            noise[ noiseComponents ]
                = Base::noiseGenerator_[ noiseComponents ]->getPrecomputedNoise( latticePoint );
        }

        double mult = sqrt( 2.0 * Base::noiseIntensity() * Base::tau );
        LatticeIntegratorLoop< T_model, Components::number_of_Variables >::noiseMapping(
            xi, noise, mult );
    }


    // Halbschritt-Iteration
    for (int i = 0; i < SIIP_ITERATIONS; ++i) {
        Components f = static_cast< T_model* > ( this ) -> step_f( Components( values ), latticePoint );
        Components g = HAS_G ? static_cast< T_model* > ( this ) -> step_g( Components( values ), latticePoint ) : Components();
        Components h = HAS_H ? static_cast< T_model* > ( this ) -> step_h( Components( values ), latticePoint ) : Components();
        Components force = HAS_H ? static_cast< T_model* > ( this ) -> external_force( latticePoint ) : Components();

        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            values[ component ] = initial[ component ] + 1.0 / 2.0 * Base::tau * f[ component ]
                + (HAS_G ? g[ component ] : 1 ) * xi[ component ] / 2.0 +
                ( HAS_H ? (1.0 / 2.0 * Base::tau * h[ component ] * force[component]) : 0 );
        }
        // Falls im FP: abkürzen
        if ( intensity == 0 && f.isTiny() )
            break;
    }

    // Prüfe, ob es eine explizit festgelegte Integrationsfunktion gibt und führe sie aus
    if (HAS_EXPLICIT) {
        initial = (static_cast< T_model* > ( this ) -> step_explicit( Components( initial ), latticePoint, Components( xi ) ) ).toTinyVector();
    }
    // Vollschritt
    Components f = static_cast< T_model* > ( this ) -> step_f( Components( values ), latticePoint );
    Components g = HAS_G ? static_cast< T_model* > ( this ) -> step_g( Components( values ), latticePoint ) : Components();
    Components h = HAS_H ? static_cast< T_model* > ( this ) -> step_h( Components( values ), latticePoint ) : Components();
    Components force = HAS_H ? static_cast< T_model* > ( this ) -> external_force( latticePoint ) : Components();

    /*    for (uint component = 0; component < Components::number_of_Variables; ++component) {
     Base::latticeDataPointer[ component ][ latticePoint ] = initial[ component ] + Base::tau
     * f[ component ] + g[ component ] * xi[ component ];
     }*/
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::latticeDataPointer[ component ][ latticePoint ] = initial[ component ] + Base::tau
            * f[ component ] + ( HAS_G ? g[ component ] : 1 ) * xi[ component ] + (HAS_H ? (Base::tau * h[ component ] * force[component]) : 0);
    }
}

/**
 * Beschreibt den Nicht-Berauschten Teil der Reaktionsfunktion
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_f( Components, long int) {
    Components c;
    c.components = 0;
    return c;
}
/*
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_f2(Components c, long int) {
    return static_cast< T_model* > ( this ) -> step_f( c );
}
*/
/**
 * Beschreibt den Multiplikator für das Rauschen
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_g( Components,long int) {
    Components c;
    c.components = 1;
    return c;
}

/**
 * Beschreibt den Multiplikator für eine externe Kraft
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_h(Components,long int) {
    Components c;
    c.components = 0;
    return c;
}

/**
 * Beschreibt eine externe Kraft
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::external_force(long int) {
    Components c;
    c.components = 0;
    return c;
}

/**
 * Führt den Zeitschritt direkt aus
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_explicit( Components sys, long int , Components  ) {
    return sys;
}


/**
 * Static proxy method for the reaction.
 *
 * \c detProxy splits the loop into \a loopCount parts and then calls \c det on the part indexed with \a loopNum.
 */
template<typename T_model>
void SIIP_LatticeIntegrator< T_model >::step_dtProxy(
                                            SIIP_LatticeIntegrator< T_model >* lattice,
                                            int loopNum, int loopCount)
{
    int start = lattice->latticeSize() * (loopNum - 1) / loopCount;
    int end = lattice->latticeSize() * loopNum / loopCount;
    for (int i = start; i < end; i++) {
        //if ( Base::latticePointHasReaction_.data()[ i ] )
        // Todo: Reaktionspunkt irgendwie auslagern
        lattice->step_dt( i );
    }
}

/**
 *
 */
template<typename T_model>
void SIIP_LatticeIntegrator< T_model >::integration(
                                               typename Lattice< T_model >::DiffusionStepWidth firstStep,
                                               typename Lattice< T_model >::DiffusionStepWidth lastStep)
{
    if ( Base::noiseIntensity() != 0 ) {

        boost::thread noiseThread( boost::bind(
            &Base::precomputeNoise, this ) );
        Base::setBoundary();
        Base::applyDefects();
        this->executeDiffusion( firstStep );
        noiseThread.join();

        for (uint component = 0; component
            < Components::number_of_Variables; ++component)
        {
            Base::latticeDataPointer[ component ]
                = Base::lattice[ component ].data();
        }

#ifndef BOOST_THREADS
# pragma omp parallel for
        for (int i = 0; i < Base::latticeSize(); ++i) {
            if ( Base::latticePointHasReaction_.data()[ i ] )
                step_dt( i );
        }
#else
        boost::thread_group tgr;
        for (int i=0; i< BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &step_dtProxy, this, i+1, BOOST_THREADS) );
        }
        tgr.join_all();
#endif

    } else {
        Base::setBoundary();
        Base::applyDefects();
        this->executeDiffusion( firstStep );

        for (uint component = 0; component
            < Components::number_of_Variables; ++component)
        {
            Base::latticeDataPointer[ component ]
                = Base::lattice[ component ].data();
        }

#ifndef BOOST_THREADS
# pragma omp parallel for
        for (int i = 0; i < Base::latticeSize(); ++i) {
            if ( Base::latticePointHasReaction_.data()[ i ] )
                step_dt( i );
        }
#else
        boost::thread_group tgr;
        for (int i=0; i < BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &step_dtProxy, this, i+1, BOOST_THREADS) );
        }
        tgr.join_all();
#endif
    }
    if ( lastStep != Base::IgnoreStep ) {
        Base::setBoundary();
        Base::applyDefects();
        this->executeDiffusion( lastStep );
    }
}

/**
 * Integrates the Lattice for \a numberOfSteps steps. If \f$\text{numberOfSteps}\neq 1 \f$, for example this spares
 * the FFT back calculation.
 */
template<typename T_model>
void SIIP_LatticeIntegrator< T_model >::step(int numberOfSteps /* = 1 */)
{
    if ( numberOfSteps > 1 ) {
        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::HalfStep, Base::IgnoreStep );
        static_cast< T_model* > (this) -> doAfterStep();

        for (int i = 2; i < numberOfSteps; ++i) {
            static_cast< T_model* > (this) -> doBeforeStep();
            integration( Base::WholeStep, Base::IgnoreStep );
            static_cast< T_model* > (this) -> doAfterStep();
        }

        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::WholeStep, Base::HalfStep );
        static_cast< T_model* > (this) -> doAfterStep();

    } else {
        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::HalfStep, Base::HalfStep );
        static_cast< T_model* > (this) -> doAfterStep();
    }

    // Zeit time wird in diffusion() erhöht.
    Base::numSteps_ += numberOfSteps;
}

#endif

