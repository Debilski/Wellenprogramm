/*
 * fftw3_wrapper.cpp
 *
 *  Created on: 18.11.2008
 *      Author: rikebs
 */

#include <iostream>

#include "fftw3_wrapper.h"



#include "singleton_helper.h"


/**
 * Zerstört beim Beenden Fftw3Wrapper.
 */
SingletonCleaner< Fftw3Wrapper > Fftw3WrapperCleanerInst;

//
// static members
//
Fftw3Wrapper* Fftw3Wrapper::instance_ = 0;
int Fftw3Wrapper::refCount_ = 0;

/**
 * Reduziert die Referenzen um eins und zerstört den Wrapper, wenn keine mehr vorhanden sind.
 */
void Fftw3Wrapper::release()
{
  if ( --refCount_ < 1 ) {
    destroy();
  }
}

/**
 * Zerstört den Wrapper
 */
void Fftw3Wrapper::destroy()
{
  if ( instance_ != 0 ) {
    delete (instance_);
    instance_ = 0;
  }
}

/**
 * Initialisiert die Threads für FFTW, falls \c fftwNumberOfThreads > 1.
 */
Fftw3Wrapper::Fftw3Wrapper()
{
  std::cout << "FFTW Constructor" << std::endl;
  if( fftwNumberOfThreads > 1) {
    fftw_init_threads();
    fftw_plan_with_nthreads( fftwNumberOfThreads );
  }
}

/**
 * Räumt die FFTW-Threads auf.
 */
Fftw3Wrapper::~Fftw3Wrapper()
{
  std::cout << "FFTW Destructor" << std::endl;
  if( fftwNumberOfThreads > 1) {
    fftw_cleanup_threads();
  }
}

/**
 * Importiert Wisdom aus einer Datei. Sollte man vor Planerstellung aufrufen.
 */
void Fftw3Wrapper::importWisdom(const std::string s)
{
  FILE *fp;

  fp = fopen( s.c_str(), "r" );
  if ( fp != NULL ) {
    fftw_import_wisdom_from_file( fp );
    fclose( fp );
  }

}

/**
 * Speichert das gewonnene Wisdom wieder ab.
 */
void Fftw3Wrapper::exportWisdom(const std::string s)
{
  FILE *fp;
  fp = fopen( s.c_str(), "w" );
  if ( fp != NULL ) {
    fftw_export_wisdom_to_file( fp );
    fclose( fp );
  } else {
      std::cerr << "Fehler beim Schreiben von Wisdom. Möglicherweise keine Schreibrechte." << std::endl;
  }
}


/**
 * Erstellt einen Plan für eine zweidimensionale reale DF-Trafo.
 *
 * \attention Sollten die Dimensionen der übergebenen Arrays nicht korrekt sein, so wird eine
 * Assertion ausgelöst. Diese Methode wird kein @c resize ausführen, wenn es falsch ist.
 *
 * \attention Es muss gelten: out.Nx = in.Nx, out.Ny = in.Ny / 2 + 1
 *
 * \param in Das reale Eingangsarray
 * \param out Das komplexe Ausgabearray
 */
fftw_plan Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(blitz::Array< double, 2 >& in, blitz::Array<
    std::complex< double >, 2 >& out)
{
  int x = in.extent( blitz::firstDim );
  int y = in.extent( blitz::secondDim );
  assert( out.extent( blitz::firstDim ) == in.extent( blitz::firstDim ) );
  assert( out.extent( blitz::secondDim ) == in.extent( blitz::secondDim ) / 2 + 1 );
  return fftw_plan_dft_r2c_2d(
      x, y, in.data(), reinterpret_cast< fftw_complex* > ( out.data() ), fftwFlags );
}

/**
 * Erstellt einen Plan für eine zweidimensionale reale DF-Rücktrafo.
 *
 * \attention Sollten die Dimensionen der übergebenen Arrays nicht korrekt sein, so wird eine
 * Assertion ausgelöst. Diese Methode wird kein @c resize ausführen, wenn es falsch ist.
 *
 * \attention Es muss gelten: in.Nx = out.Nx, in.Ny = out.Ny / 2 + 1
 *
 * \param in Das reale Eingangsarray
 * \param out Das komplexe Ausgabearray
 */
fftw_plan Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(blitz::Array< std::complex< double >, 2 >& in,
    blitz::Array< double, 2 >& out)
{
  int x = out.extent( blitz::firstDim );
  int y = out.extent( blitz::secondDim );
  assert( in.extent( blitz::firstDim ) == out.extent( blitz::firstDim ) );
  assert( in.extent( blitz::secondDim ) == out.extent( blitz::secondDim ) / 2 + 1 );
  return fftw_plan_dft_c2r_2d(
      x, y, reinterpret_cast< fftw_complex* > ( in.data() ), out.data(), fftwFlags );

}

int Fftw3Wrapper::fftwFlags = FFTW_PATIENT;// | FFTW_DESTROY_INPUT;

const int Fftw3Wrapper::fftwNumberOfThreads = 2;

std::string Fftw3Wrapper::fftwCacheFileName = "fftw_wisdom";
