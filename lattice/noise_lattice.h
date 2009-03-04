#ifndef NOISE_LATTICE_H
#define NOISE_LATTICE_H

#include "fftw3_wrapper.h"
#include "noise_generator.h"

class NoiseLattice : public NoiseGenerator {
    Fftw3Wrapper* fftw3Wrapper;
    //double* noiseLattice_;

    blitz::Array< double, 2 > spatialCorrelationFunction;

    blitz::Array< std::complex< double >, 2 > noiseLattice_fft;
    blitz::Array< std::complex< double >, 2 >
            spatialCorrelationFunction_fft;

    fftw_plan noiseLattice_forward, noiseLattice_backward;
    fftw_plan spatialCorrelationFunction_forward,
            spatialCorrelationFunction_backward;

public:
    NoiseLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    NoiseLattice(const LatticeGeometry& geometry);
    NoiseLattice(const LatticeInterface* lattice);
    ~NoiseLattice();

    //  double getPrecomputedNoise(int i) const;
    void precomputeNoise(double correlation, double intensity);

    void
            precomputeNoiseSpatialLowPass(double correlation,
                    double intensity);

    void precomputeNoiseSpatialCorr(double correlation, double intensity);
    void precomputeNoiseSpatialBlocks(double correlation, double intensity);
    void precomputeSineWaves(double correlation, double intensity);

private:
    void init();
};

#endif

