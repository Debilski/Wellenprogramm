/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * finite_interval_lattice_integrator.h
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef FINITE_INTERVAL_LATTICE_INTEGRATOR_H_
#define FINITE_INTERVAL_LATTICE_INTEGRATOR_H_

#include "lattice_integrator.h"
#include "siip_lattice_integrator.h"

template<typename T_model>
class FiniteIntervalIntegrator : public SIIP_LatticeIntegrator< T_model > {
    typedef SIIP_LatticeIntegrator< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
public:
    FiniteIntervalIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    SIIP_LatticeIntegrator< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
        Base::INTEGRATOR_NAME = std::string( "Semi Implicite Finite Intervall" );
    }
protected:
    void executeDiffusion(typename Base::DiffusionStepWidth stepSize, bool advanceInTime = true);
    inline double my_laplacian(blitz::Array< double, 2 >& a, int x, int y);
    inline double my_laplacian_periodic(blitz::Array< double, 2 >& a, int x, int y);
};



#include "finite_interval_lattice_integrator.cpp"

#endif /* FINITE_INTERVAL_LATTICE_INTEGRATOR_H_ */
