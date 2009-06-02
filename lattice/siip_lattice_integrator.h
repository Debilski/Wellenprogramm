/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * SIIP_LatticeIntegrator.h
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef SIIP_LATTICEINTEGRATOR_H_
#define SIIP_LATTICEINTEGRATOR_H_

#include "si_lattice_integrator.h"

/**
 * This Class provides the actual integration.
 *
 * The LatticeIntegrator class employs the Curiously Recurring Template Pattern,
 * which means, that it binds to its child class already at compile time.
 * This means that the simple @c dt functions can be inlined. Which greatly reduces
 * execution time compared to a solution with @b virtual call, which pratically can’t
 * be inlined -- the additional cost of a @b virtual could not make it much worse then.
 *
 * \param T_model     The model implementation which subclasses this template
 * \param Components   The component system
 */
template<typename T_model>
class SIIP_LatticeIntegrator : public SI_LatticeIntegrator< T_model > {
protected:
    typedef Lattice< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables ? Base::number_of_Noise_Variables : 1> T_noiseVector;

    //typedef typename _tvec<Base::number_of_Noise_Variables>::TinyVectorWithZero T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;

public:

    SIIP_LatticeIntegrator() :
            SI_LatticeIntegrator< T_model> ()
    {
        Base::INTEGRATOR_NAME = std::string( "Semi Implicit Interaction Picture" );
    }
    void step() { step(1); }
    void step(int numberOfSteps);
protected:
    static const int SIIP_ITERATIONS = Metainfo< T_model>::SIIP_ITERATIONS;

    void step_dt(long int latticePoint);
    //Components step_f(Components);

    Components step_explicit( Components sys, long int position, Components noise );

    Components step_f(Components components, long int latticePoint);
    Components step_g(Components components, long int latticePoint);
    Components step_h(Components components, long int latticePoint);
    Components external_force(long int latticePoint);

    static void step_dtProxy(
        SIIP_LatticeIntegrator< T_model>* lattice,
        int loopNum, int loopCount);
    void
    integration(
        typename Lattice< T_model>::DiffusionStepWidth firstStep,
        typename Lattice< T_model>::DiffusionStepWidth lastStep);
private:
    static const bool HAS_H = ! Metainfo< T_model>::OPTIMISE_NO_EXTERNAL_FORCE;
    static const bool HAS_G = ! Metainfo< T_model>::OPTIMISE_NO_MULTIPLICATIVE_NOISE;
    static const bool HAS_EXPLICIT = Metainfo< T_model >::HAS_EXPLICIT_STEP_FUNCTION;

    //    T_model* subModel;
};

#include "siip_lattice_integrator.cpp"

#endif /* SIIP_LATTICEINTEGRATOR_H_ */
