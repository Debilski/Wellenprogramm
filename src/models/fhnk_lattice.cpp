/*
 * fhnk_lattice.cpp
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#include "fhnk_lattice.h"

DEFINE_LATTICE_DRIVER2("FhnKLattice 4er", FhnKLattice4)

DEFINE_LATTICE_DRIVER2("FhnKLattice 8er", FhnKLattice8)

DEFINE_LATTICE_DRIVER2("FhnKLattice global z 4er", FhnKLattice_z4)

DEFINE_LATTICE_DRIVER2("FhnKLattice global z 8er", FhnKLattice_z8)

DEFINE_LATTICE_DRIVER2("FhnKLattice Laplace", FhnKLattice_laplace)

DEFINE_LATTICE_DRIVER2("FhnKLattice Laplace Extended", FhnKLattice_laplace_extended)

DEFINE_LATTICE_DRIVER2("FhnKLattice Layers", FhnKLattice_layers)

REGISTER_PLUGINS_BEGIN
REGISTER_PLUGIN(FhnKLattice4)
REGISTER_PLUGIN(FhnKLattice8)
REGISTER_PLUGIN(FhnKLattice_z4)
REGISTER_PLUGIN(FhnKLattice_z8)
REGISTER_PLUGIN(FhnKLattice_laplace)
REGISTER_PLUGIN(FhnKLattice_laplace_extended)
REGISTER_PLUGIN(FhnKLattice_layers)
REGISTER_PLUGINS_END
