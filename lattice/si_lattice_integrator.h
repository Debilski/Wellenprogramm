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
    SI_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
        LatticeIntegrator< T_model > ( sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
    }
};

#endif /* SI_LATTICEINTEGRATOR_H_ */
