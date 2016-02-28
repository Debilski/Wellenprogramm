#ifndef LATTICE_MODELS_H
#define LATTICE_MODELS_H

#include "rds_lattice.h"


/**
 * Helper Macro that can be used for automatically creating the Metainfo<> Template
 */
#define META(model, components)        \
    class model;                       \
    template <>                        \
    struct Metainfo<model>             \
    {                                  \
        typedef components Components; \
    };

#if 0


ONE_COMPONENT_SYSTEM( OCS, v )

//template class Lattice< TwoComponentSystem >;
//extern template class LatticeIntegrator< BistableLattice, OneComponentSystem >;
//extern template class LatticeIntegrator< FhnLattice, TwoComponentSystem >;

// Fehlen:
// - Stückweise linear
// - Barkley
// - Scott
// - Oregonator


#ifndef HODGKIN_HUXLEY
#endif

#endif
#endif
