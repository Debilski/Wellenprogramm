/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * SI_LatticeIntegrator.h
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef SI_LATTICEINTEGRATOR_H_
#define SI_LATTICEINTEGRATOR_H_

#include "lattice_integrator.h"

template<typename T_model>
class SI_LatticeIntegrator : public LatticeIntegrator< T_model > {
public:
    SI_LatticeIntegrator() :
        LatticeIntegrator< T_model > (  )
    {
    }
};

#endif /* SI_LATTICEINTEGRATOR_H_ */
