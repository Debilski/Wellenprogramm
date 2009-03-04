/*
 * fftw3_wrapper.h
 *
 *  Created on: 18.11.2008
 *      Author: rikebs
 */

#ifndef FFTW3_WRAPPER_H_
#define FFTW3_WRAPPER_H_

#include <complex>
#include <fftw3.h>
#include <blitz/array.h>

/**
 * Singleton class for initialising FFTW and setting up FFTW plans with blitz::Arrays.
 *
 *
 */

class Fftw3Wrapper {
protected:

  static Fftw3Wrapper* instance_;
  static int refCount_;
public:
  static int fftwFlags;
  static const int fftwNumberOfThreads;
  static std::string fftwCacheFileName;

  static fftw_plan blitzFftwPlan_dft_r2c_2d(blitz::Array< double, 2 >& in, blitz::Array< std::complex< double > , 2 >& out);
  static fftw_plan blitzFftwPlan_dft_c2r_2d(blitz::Array< std::complex< double > , 2 >& in, blitz::Array< double, 2 >& out);

  /**
   * Erzeugt eine neue Instanz oder gibt die bereits erzeugte Instanz zurück.
   */
  static Fftw3Wrapper* instance()
  {
    if ( instance_ == 0 ) // if first time ...
    {
      instance_ = new Fftw3Wrapper(); // ... create a new 'solo' instance
    }
    refCount_++; //increase reference counter
    return instance_;
  }

  static void release();
  static void destroy();
  ~Fftw3Wrapper();
  void importWisdom(std::string s = std::string( fftwCacheFileName ));
  void exportWisdom(std::string s = std::string( fftwCacheFileName ));
protected:
  Fftw3Wrapper(); // verhindert, das ein Objekt von außerhalb von N erzeugt wird.
  // protected, wenn man von der Klasse noch erben möchte
  Fftw3Wrapper(const Fftw3Wrapper&); /* verhindert, dass eine weitere Instanz via
   Kopie-Konstruktor erstellt werden kann */
  Fftw3Wrapper& operator=(const Fftw3Wrapper &); //Verhindert weitere Instanz durch Kopie
};

#endif /* FFTW3_WRAPPER_H_ */
