/*
 * brusselator_lattice.cpp
 *
 *  Created on: 19.01.2009
 *      Author: rikebs
 */

#include "brusselator_lattice.h"


BrusselatorLattice::BrusselatorLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<BrusselatorLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY), a(2, "a", 0., 5.0), b(2., "b", 0., 20., 5)
{
    componentInfos[0] = ComponentInfo("Aktivator", "u", -0.1, 1.1);
    componentInfos[1] = ComponentInfo("Inhibitor", "v", -0.1, 1.1);

    setExcitationThreshold(0.5);

    registerParameter(&a);
    registerParameter(&b);
}


LATTICE_REGISTER_MODEL2("BrusselatorLattice", BrusselatorLattice)
