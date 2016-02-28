/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * noise_generator.cpp
 *
 *  Created on: 24.11.2008
 *      Author: rikebs
 */

#include "noise_generator.h"

NoiseGenerator::NoiseGenerator(int sizeX, int sizeY, int latticeSizeX,
    int latticeSizeY) : LatticeGeometry(sizeX, sizeY, latticeSizeX, latticeSizeY),
                        blitz_normal(0, 1),
                        lattice_(0)
{
    init();
}

NoiseGenerator::NoiseGenerator(const LatticeGeometry& geometry) : LatticeGeometry(geometry), blitz_normal(0, 1), lattice_(0)
{
    init();
}

NoiseGenerator::NoiseGenerator(const LatticeInterface* lattice) : LatticeGeometry(*lattice), blitz_normal(0, 1), lattice_(lattice)
{
    init();
}

NoiseGenerator::~NoiseGenerator()
{
}

void NoiseGenerator::init()
{
    blitz_normal.seed((unsigned int)time(0));
    noiseLattice_.resize(latticeSizeX(), latticeSizeY());
}

std::string NoiseGenerator::getModelName()
{
    return modelName;
}


/**
 * If the NoiseGenerator is connected to a non-abstrace child of LatticeInterface,
 * it can use all of the LatticeInterface’s public functions, including, but not limited
 * to getTimeStep(), time(), etc.
 */
void NoiseGenerator::connectToLattice(const LatticeInterface* lattice)
{
    lattice_ = lattice;
}

void NoiseGenerator::disconnectFromLattice()
{
    lattice_ = 0;
}
