/*
 * fhn_original_lattice.cpp
 *
 *  Created on: 19.01.2009
 *      Author: rikebs
 */

#include "fhn_original_lattice.h"


FhnOriginalLattice::FhnOriginalLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<FhnOriginalLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY), a_0(0, "a_0", 0., 5.0), a_1(2., "a_1", 0., 20., 5), epsilon(2., "epsilon", 0., 20., 5)
{
    componentInfos[0] = ComponentInfo("Aktivator", "u", -1.1, 1.1);
    componentInfos[1] = ComponentInfo("Inhibitor", "v", -1.1, 1.1);

    setExcitationThreshold(0.5);

    registerParameter(&a_0);
    registerParameter(&a_1);
    registerParameter(&epsilon);
}


LATTICE_REGISTER_MODEL2("FhnOriginalLattice", FhnOriginalLattice)
