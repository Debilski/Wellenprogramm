/*
 * barkley_lattice.cpp
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#include "barkley_lattice.h"

BarkleyLattice::BarkleyLattice() : epsilon(
                                       0.06, "epsilon", 0.001, 10.0),
                                   a(0.75, "a", 0.2, 1.0),
                                   b(0.06, "b", 0., 1., 5)
{
    componentInfos[0] = ComponentInfo("Aktivator", "u", -0.1, 1.1);
    componentInfos[1] = ComponentInfo("Inhibitor", "v", -0.1, 1.1);

    setExcitationThreshold(0.5);

    registerParameter(&epsilon);
    registerParameter(&a);
    registerParameter(&b);
}


LATTICE_REGISTER_MODEL2("BarkleyLattice", BarkleyLattice)
