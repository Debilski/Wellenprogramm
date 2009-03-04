#include "noise_lattice.h"

#include <iostream>



NoiseLattice::NoiseLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
: NoiseGenerator(sizeX, sizeY, latticeSizeX, latticeSizeY)
{
    init();
}

NoiseLattice::NoiseLattice(const LatticeGeometry& geometry)
: NoiseGenerator(geometry)
{
    init();
}

NoiseLattice::NoiseLattice(const LatticeInterface* lattice)
: NoiseGenerator(lattice)
{
    init();
}


void NoiseLattice::init()
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
}

NoiseLattice::~NoiseLattice()
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



void NoiseLattice::precomputeNoise(double correlation, double intensity)
{
  //precomputeNoiseSpatialLowPass(correlation, intensity);
  precomputeNoiseSpatialCorr(correlation, intensity);
  //precomputeNoiseSpatialBlocks(correlation, intensity);

  //precomputeSineWaves(correlation, intensity);

  /*
  double mean = 0;
  double corr = 0;
  for (int i=0; i<latticeSize(); ++i) {
    mean += noiseLattice_[i] * scaleX() * scaleY();
    corr += noiseLattice_[i] * noiseLattice_[i] * scaleX() * scaleY();
  }
  std::cout << "Mean = " << mean/sizeX/sizeY << " Corr = " << corr/sizeX/sizeY << std::endl << std::flush;
  */
}

void NoiseLattice::precomputeSineWaves(double correlation, double intensity)
{
  if (intensity == 0) return;

  for (int i=0; i<latticeSize(); ++i) {
      noiseLattice_[i] = intensity * sin(indexToX(i) * correlation * 2*M_PI / latticeSizeX()) *  sin(indexToY(i) * correlation * 2*M_PI / latticeSizeY());
  }
  return;
}

void NoiseLattice::precomputeNoiseSpatialLowPass(double correlation, double intensity)
{
  if (intensity == 0) return;

  /*

  double lambda = 1;
  double sigma = 1;
  double* _OU_noise;
  for (int i=0; i<latticeSize(); ++i) {
 _OU_noise[i] = _OU_noise[i]*exp(-lambda * tau) + sigma*sqrt( (1 - exp(-2 * lambda * tau ) )/(2*lambda) ) * blitz_normal.random();

}

for (int i=0; i<latticeSize(); ++i) {
 double convol = 0;
 for (int j=0; j< 2 * latticeSize(); ++j) {

 int diffx = indexToX(i) - indexToX(j);
 int diffy = indexToY(i) - indexToY(j);
 double r = sqrt( scaleX() * scaleY() * (diffx*diffx + diffy*diffy) );
 convol += _OU_noise[i] * M_PI / (2. * lambda) * exp(- 2*r/lambda );
}

_precomputed_noise[i] = (1./(2. * latticeSize())) * (1./(2. * latticeSize())) *  convol * 1.E7;
_precomputed_noise[i];
}




return;
*/
  //  double mean = 0;
  //  double m_sq = 0;
  //cout << "precompute noise ..." << flush;
  double size = 0;
  if ( correlation < sizeX() && correlation >= 0 ) {
    //cout << "precompute noise ..." << flush;
  //  for (int i=0; i<latticeSize(); ++i) {
      noiseLattice_ = blitz_normal.random() / latticeSize();

      //_precomputed_noise[i] = helper::noise() / latticeSize();
      //      mean += _precomputed_noise[i] * latticeSize() / latticeSize();
      //      m_sq += _precomputed_noise[i] * _precomputed_noise[i] * latticeSize() * latticeSize() / latticeSize();
  //  }

    //cout << "fourier ..." << flush;
    fftw_execute(noiseLattice_forward);
    int max_x_out = (latticeSizeX());
    int max_y_out = (latticeSizeY())/2 +1;

    for (int i=0; i< max_x_out; ++i) {
      for (int j=0; j < max_y_out; ++j) {
        double f_x = (i < latticeSizeX()/2) ? i : latticeSizeX() - i;
        //cout << f_x << " ";
        double f_y = j;
        if ( sqrt(f_x*f_x + f_y*f_y) > correlation ) {
          noiseLattice_fft[j + max_y_out * i][0] = 0;
          noiseLattice_fft[j + max_y_out * i][1] = 0;
        } else {
          ++size;
        }
      }
    }
    fftw_execute(noiseLattice_backward);
  } else {
    //cout << "precompute noise ..." << flush;
    for (int i=0; i<latticeSize(); ++i) {
      noiseLattice_.data()[i] = blitz_normal.random();
      //      mean += _precomputed_noise[i] / latticeSize();
      //      m_sq += _precomputed_noise[i] * _precomputed_noise[i] / latticeSize();
    }
  }

  //  cout << ": " << mean << " " << sqrt(m_sq) << " -- ";
  if (size != 0) {
    // for (int i=0; i<latticeSize(); ++i) {
      noiseLattice_ =  noiseLattice_ * sqrt( (double) latticeSize() )  / sqrt( 2. * size  );
      //if (i == 20) cout << _precomputed_noise[i]*latticeSize() << " " << flush;
    // }
  }
  /*  mean = 0;
  m_sq = 0;
  for (int i=0; i<latticeSize(); ++i) {
 mean += _precomputed_noise[i] / latticeSize();
 m_sq += _precomputed_noise[i] * _precomputed_noise[i] / latticeSize();
 //if (i == 20) cout << _precomputed_noise[i]*latticeSize() << " " << flush;
}
*/
//  cout << noise_correlation() << ": " << mean << " " << sqrt(m_sq) << " -> " ;
//  cout << sqrt( 2*size   / latticeSize())<< endl << flush;
//cout << "done" << endl << flush;


}


void NoiseLattice::precomputeNoiseSpatialCorr(double correlation, double intensity)
{
  if (intensity == 0) return;

    for (int i=0; i<latticeSize(); ++i) {
      noiseLattice_.data()[i] = blitz_normal.random();
    }

  if (correlation <= 0) return;
  for (int i=0; i<latticeSize(); ++i) {
    double rx = ( indexToX(i) < latticeSizeX() / 2 ) ? indexToX(i) : (indexToX(i) - latticeSizeX() );
    double ry = ( indexToY(i) < latticeSizeY() / 2 ) ? indexToY(i) : (indexToY(i) - latticeSizeY() );
    spatialCorrelationFunction.data()[i] = M_PI / 2. / correlation * exp( -2./correlation * sqrt(rx*rx + ry*ry ) * scaleX() * scaleY() );
  }

  fftw_execute( noiseLattice_forward );
  fftw_execute( spatialCorrelationFunction_forward );

  /*
  int max_x_out = (latticeSizeX());
  int max_y_out = (latticeSizeY())/2 +1;

  for (int i=0; i< max_x_out; ++i) {
    for (int j=0; j < max_y_out; ++j) {
      noiseLattice_fft.data()[j + max_y_out * i].real() *= spatialCorrelationFunction_fft.data()[j + max_y_out * i].real()  / latticeSize();
      noiseLattice_fft.data()[j + max_y_out * i].imag() *= spatialCorrelationFunction_fft.data()[j + max_y_out * i].imag()  / latticeSize();
    }
  }*/

  noiseLattice_fft = noiseLattice_fft * spatialCorrelationFunction_fft / (double)latticeSize();
  fftw_execute( noiseLattice_backward );


}

void NoiseLattice::precomputeNoiseSpatialBlocks(double correlation, double intensity)
{
  if (intensity == 0) return;
  if (correlation < 0) precomputeNoiseSpatialLowPass(correlation, intensity);
  if (correlation == 0) return;
  double blockSizeX = correlation / scaleX();
  double blockSizeY = correlation / scaleY();
  for (int i=0; i<sizeX()/ correlation; ++i) {
    for (int j=0; j<sizeY()/ correlation; ++j) {
      double blockRand = blitz_normal.random();// * sqrt( scaleX() * scaleY() );
      for (int ki=0; ki < blockSizeX; ++ki) {
        for (int kj=0; kj < blockSizeY; ++kj) {
          noiseLattice_.data()[index(i*blockSizeX + ki, j*blockSizeY + kj)] = blockRand;
        }
      }
    }
  }
}

