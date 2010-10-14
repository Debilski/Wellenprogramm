/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * ou_noise_generator.cpp
 *
 *  Created on: 24.11.2008
 *      Author: rikebs
 */

#include "noise_lattice.h"

#include <iostream>



OuNoiseGenerator::OuNoiseGenerator(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
: NoiseGenerator(sizeX, sizeY, latticeSizeX, latticeSizeY)
{
    init();
}

OuNoiseGenerator::OuNoiseGenerator(const LatticeGeometry& geometry)
: NoiseGenerator(geometry)
{
    init();
}

void OuNoiseGenerator::init()
{

  spatialCorrelationFunction.resize(latticeSizeX(), latticeSizeY());

  std::cout << "Make plans (noise)... " << std::flush;

  fftw3Wrapper = Fftw3Wrapper::instance();
  fftw3Wrapper->importWisdom();

  std::cout << "noise\n" << std::flush;
  noiseLattice_fft.resize(latticeSizeX(), latticeSizeY() / 2 + 1 );
  std::cout << "lat\n" << std::flush;
  spatialCorrelationFunction_fft.resize(latticeSizeX(), latticeSizeY() / 2 + 1 );
  std::cout << "spatial\n" << std::flush;
//  noiseLattice_forward = fftw_plan_dft_r2c_2d(latticeSizeX(), latticeSizeY(), noiseLattice_.data(), noiseLattice_fft,  FFTW_PATIENT | FFTW_DESTROY_INPUT);

  noiseLattice_forward = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(noiseLattice_, noiseLattice_fft);

  std::cout << "noisefor\n" << std::flush;
//  noiseLattice_backward = fftw_plan_dft_c2r_2d(latticeSizeX(), latticeSizeY(), noiseLattice_fft, noiseLattice_.data(),  FFTW_PATIENT | FFTW_DESTROY_INPUT);
  noiseLattice_backward= Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(noiseLattice_fft, noiseLattice_);

  std::cout << "noisebacl\n" << std::flush;

  spatialCorrelationFunction_forward =  Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d( spatialCorrelationFunction, spatialCorrelationFunction_fft);
  std::cout << "spatialfor\n" << std::flush;
  spatialCorrelationFunction_backward =  Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d( spatialCorrelationFunction_fft, spatialCorrelationFunction);
  std::cout << "spatialback\n" << std::flush;

  fftw3Wrapper->exportWisdom();

  std::cout << "done\n" << std::flush;


  ouNoiseLattice_.resize( latticeSizeX(), latticeSizeY() );
  ouNoiseLattice_ = 0;

}

OuNoiseGenerator::~OuNoiseGenerator()
{
  std::cout << "Destroy NoiseLattice... " << std::flush;
  //fftw_free( noiseLattice_ );

  fftw_destroy_plan( noiseLattice_forward );
  fftw_destroy_plan( noiseLattice_backward );
  fftw_destroy_plan( spatialCorrelationFunction_forward );
  fftw_destroy_plan( spatialCorrelationFunction_backward );

  fftw3Wrapper->destroy();
  //fftw_cleanup_threads();
  std::cout << "done\n" << std::flush;
}

void OuNoiseGenerator::precomputeNoiseSpatiotemporal(double correlation, double intensity)
{
  double lambda = correlation;
  double tau = 0.001;
  double sigma = 1.;
  for (int i=0; i<latticeSize(); ++i) {
    ouNoiseLattice_[i] = ouNoiseLattice_[i]*exp(-lambda * tau) + sigma*sqrt( (1 - exp(-2 * lambda * tau ) )/(2*lambda) ) * blitz_normal.random();
  }

  for (int i=0; i<latticeSize(); ++i) {
    noiseLattice_[i] = ouNoiseLattice_[i];
  }

  for (int i=0; i<latticeSize(); ++i) {
    spatialCorrelationFunction[i] = M_PI / 2. / correlation * exp( -2./correlation * sqrt(indexToX(i)*indexToX(i) + indexToY(i)+indexToY(i) ) * scaleX() * scaleY() );
  }

  fftw_execute( noiseLattice_forward );
  fftw_execute( spatialCorrelationFunction_forward );

  int max_x_out = (latticeSizeX());
  int max_y_out = (latticeSizeY())/2 +1;

  for (int i=0; i< max_x_out; ++i) {
    for (int j=0; j < max_y_out; ++j) {
      noiseLattice_fft[j + max_y_out * i][0] *= spatialCorrelationFunction_fft[j + max_y_out * i][0]  / latticeSize();
      noiseLattice_fft[j + max_y_out * i][1] *= spatialCorrelationFunction_fft[j + max_y_out * i][1]  / latticeSize();
    }
  }
  fftw_execute( noiseLattice_backward );

}
