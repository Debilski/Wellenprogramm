/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * ou_noise_generator.h
 *
 *  Created on: 24.11.2008
 *      Author: rikebs
 */

#ifndef OU_NOISE_GENERATOR_H_
#define OU_NOISE_GENERATOR_H_


#include "fftw3_wrapper.h"
#include "noise_generator.h"

class OuNoiseGenerator : public NoiseGenerator
{
    Fftw3Wrapper* fftw3Wrapper;

    blitz::Array<double, 2> ouNoiseLattice_;
    blitz::Array<double, 2> spatialCorrelationFunction;

    blitz::Array<std::complex<double>, 2> noiseLattice_fft;
    blitz::Array<std::complex<double>, 2>
        spatialCorrelationFunction_fft;

    fftw_plan noiseLattice_forward, noiseLattice_backward;
    fftw_plan spatialCorrelationFunction_forward,
        spatialCorrelationFunction_backward;

public:
    OuNoiseGenerator(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    OuNoiseGenerator(const LatticeGeometry& geometry);
    ~OuNoiseGenerator();

    //  double getPrecomputedNoise(int i) const;
    void precomputeNoise(double correlation, double intensity);

    void
    precomputeNoiseSpatiotemporal(double correlation,
        double intensity);

private:
    void init();
};


#endif /* OU_NOISE_GENERATOR_H_ */
