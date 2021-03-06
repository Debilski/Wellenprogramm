/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * fftw3_wrapper.h
 *
 *  Created on: 18.11.2008
 *      Author: rikebs
 */

#ifndef FFTW3_WRAPPER_H_
#define FFTW3_WRAPPER_H_

#include <vector>
#include <complex>

#include <fftw3.h>
#include <blitz/array.h>


/**
 * Singleton class for initialising FFTW and setting up FFTW plans with blitz::Arrays.
 *
 *
 */

class Fftw3Wrapper
{
protected:
    static Fftw3Wrapper* instance_;
    static int refCount_;

public:
    static int fftwFlags;
    static const int fftwNumberOfThreads;
    static std::string fftwCacheFileName;

    static fftw_plan blitzFftwPlan_dft_c2c_forward_2d(blitz::Array<std::complex<double>, 2>& in, blitz::Array<std::complex<double>, 2>& out);
    static fftw_plan blitzFftwPlan_dft_c2c_backward_2d(blitz::Array<std::complex<double>, 2>& in, blitz::Array<std::complex<double>, 2>& out);
    static fftw_plan blitzFftwPlan_dft_r2c_2d(blitz::Array<double, 2>& in, blitz::Array<std::complex<double>, 2>& out);
    static fftw_plan blitzFftwPlan_dft_c2r_2d(blitz::Array<std::complex<double>, 2>& in,
        blitz::Array<double, 2>& out);

    /**
     * Erzeugt eine neue Instanz oder gibt die bereits erzeugte Instanz zurück.
     */
    static Fftw3Wrapper* instance()
    {
        if (instance_ == 0)  // if first time ...
        {
            instance_ = new Fftw3Wrapper();  // ... create a new 'solo' instance
        }
        refCount_++;  //increase reference counter
        return instance_;
    }

    static void release();
    static void destroy();
    ~Fftw3Wrapper();
    void importWisdom(std::string s = std::string(fftwCacheFileName));
    void exportWisdom(std::string s = std::string(fftwCacheFileName));

protected:
    Fftw3Wrapper();  // verhindert, das ein Objekt von außerhalb von N erzeugt wird.
    // protected, wenn man von der Klasse noch erben möchte
    Fftw3Wrapper(const Fftw3Wrapper&); /* verhindert, dass eine weitere Instanz via
     Kopie-Konstruktor erstellt werden kann */
    Fftw3Wrapper& operator=(const Fftw3Wrapper&);  //Verhindert weitere Instanz durch Kopie
};


/**
 * Hält einen Plan für Komplexe oder Reelle FFT fest.
 */
class FastFTPlan
{

public:
    FastFTPlan(fftw_plan fftwPlan, std::complex<double>* in, std::complex<double>* out,
        int sizeIn, int sizeOut, int dimensions) : fftType_(ComplexComplexFft), fftwPlan_(fftwPlan), complex_in_(in), complex_out_(out), real_in_(0), real_out_(0), size_in_(sizeIn), size_out_(sizeOut), dimensions_(dimensions)
    {
    }

    FastFTPlan(fftw_plan fftwPlan, double* in, std::complex<double>* out, int sizeIn,
        int sizeOut, int dimensions) : fftType_(RealComplexFft), fftwPlan_(fftwPlan), complex_in_(0), complex_out_(out), real_in_(in), real_out_(0), size_in_(sizeIn), size_out_(sizeOut), dimensions_(dimensions)
    {
    }

    FastFTPlan(fftw_plan fftwPlan, std::complex<double>* in, double* out, int sizeIn,
        int sizeOut, int dimensions) : fftType_(ComplexRealFft), fftwPlan_(fftwPlan), complex_in_(in), complex_out_(0), real_in_(0), real_out_(out), size_in_(sizeIn), size_out_(sizeOut), dimensions_(dimensions)
    {
    }

    inline bool check(std::complex<double>* in, std::complex<double>* out, int sizeIn,
        int sizeOut, int dimensions) const;
    inline bool check(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
        int dimensions) const;
    inline bool check(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
        int dimensions) const;

    inline fftw_plan plan() const;
    inline void destroyPlan();

    ~FastFTPlan()
    {
        if (fftwPlan_ != 0)
            destroyPlan();
    }

private:
    /**
     * Enum für den Typ der Transformation.
     */
    enum FftType
    {
        ComplexComplexFft,
        RealComplexFft,
        ComplexRealFft
    };


    inline bool check(FftType type, std::complex<double>* complex_in,
        std::complex<double>* complex_out, double* real_in, double* real_out,
        int sizeIn, int sizeOut, int dimensions) const;

    /**
     * Speichert den Typ der Transformation
     */
    FftType fftType_;

    /**
     * Speichert den Plan
     */

    fftw_plan fftwPlan_;
    /*
     * Speicherpositionen
     */
    std::complex<double>* complex_in_;
    std::complex<double>* complex_out_;
    double* real_in_;
    double* real_out_;
    /*
     * Länge der Arrays (in sizeof()-Einheiten)
     */
    int size_in_;
    int size_out_;

    /**
     * Speichert die Zahl der Dimensionen der Transformation
     */
    int dimensions_;
};

bool FastFTPlan::check(FftType type, std::complex<double>* complex_in,
    std::complex<double>* complex_out, double* real_in, double* real_out,
    int sizeIn, int sizeOut, int dimensions) const
{
    return fftType_ == type && complex_in_ == complex_in && complex_out_ == complex_out && real_in_ == real_in && real_out_ == real_out && size_in_ == sizeIn && size_out_ == sizeOut
        && dimensions_ == dimensions;
}

bool FastFTPlan::check(std::complex<double>* in, std::complex<double>* out, int sizeIn,
    int sizeOut, int dimensions) const
{
    return check(ComplexComplexFft, in, out, 0, 0, sizeIn, sizeOut, dimensions);
}

bool FastFTPlan::check(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
    int dimensions) const
{
    return check(RealComplexFft, 0, out, in, 0, sizeIn, sizeOut, dimensions);
}

bool FastFTPlan::check(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
    int dimensions) const
{
    return check(ComplexRealFft, in, 0, 0, out, sizeIn, sizeOut, dimensions);
}

fftw_plan FastFTPlan::plan() const
{
    return fftwPlan_;
}

void FastFTPlan::destroyPlan()
{
    fftw_destroy_plan(fftwPlan_);
    fftwPlan_ = 0;
}

/**
 * Basisklasse für FFT-Transformierungen
 *
 * \attention Höchstwahrscheinlich nicht Thread-Safe!
 * \TODO Mutex in die execute-Funktion einbauen, so dass nicht während des Durchsuchens ein
 * Plan erzeugt wird, der die Ordnung verfälscht. Muss man irgendwie testen...
 *
 */
class FastFT
{
public:
    typedef blitz::Array<double, 2> bl_double_2d;
    typedef blitz::Array<std::complex<double>, 2> bl_complex_2d;

    inline void execute(bl_complex_2d& from, bl_complex_2d& to);
    inline void execute(bl_double_2d& from, bl_complex_2d& to);
    inline void execute(bl_complex_2d& from, bl_double_2d& to);

private:
    typedef std::vector<FastFTPlan*> T_plan_vec;
    typedef std::vector<FastFTPlan*>::const_iterator T_plan_iter;
    typedef std::vector<FastFTPlan*>::iterator T_plan_mutable_iter;

    inline T_plan_iter createPlan(std::complex<double>* in, std::complex<double>* out,
        int sizeIn, int sizeOut, int dimensions);
    inline T_plan_iter createPlan(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
        int dimensions);
    inline T_plan_iter createPlan(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
        int dimensions);

    inline void destroyAllPlans();

    inline T_plan_iter find(std::complex<double>* in, std::complex<double>* out, int sizeIn,
        int sizeOut, int dimensions);
    inline T_plan_iter find(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
        int dimensions);
    inline T_plan_iter find(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
        int dimensions);

    T_plan_vec plans_;
};

void FastFT::execute(bl_complex_2d& from, bl_complex_2d& to)
{
    T_plan_iter p_plan = find(from.data(), to.data(), from.size(), to.size(), 2);
    if (p_plan == plans_.end()) {
        p_plan = createPlan(from.data(), to.data(), from.size(), to.size(), 2);
    }
    fftw_execute((*p_plan)->plan());
}

void FastFT::execute(bl_double_2d& from, bl_complex_2d& to)
{
    T_plan_iter p_plan = find(from.data(), to.data(), from.size(), to.size(), 2);
    if (p_plan == plans_.end()) {
        p_plan = createPlan(from.data(), to.data(), from.size(), to.size(), 2);
    }
    fftw_execute((*p_plan)->plan());
}

void FastFT::execute(bl_complex_2d& from, bl_double_2d& to)
{
    T_plan_iter p_plan = find(from.data(), to.data(), from.size(), to.size(), 2);
    if (p_plan == plans_.end()) {
        p_plan = createPlan(from.data(), to.data(), from.size(), to.size(), 2);
    }
    fftw_execute((*p_plan)->plan());
}

FastFT::T_plan_iter FastFT::createPlan(std::complex<double>* in, std::complex<double>* out, int sizeIn,
    int sizeOut, int dimensions)
{
    fftw_plan p = fftw_plan_dft_2d(sizeIn, sizeOut, reinterpret_cast<fftw_complex*>(in), reinterpret_cast<fftw_complex*>(out), FFTW_FORWARD, Fftw3Wrapper::fftwFlags);
    FastFTPlan* plan = new FastFTPlan(p, in, out, sizeIn, sizeOut, dimensions);
    return plans_.insert(plans_.end(), plan);
}

FastFT::T_plan_iter FastFT::createPlan(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
    int dimensions)
{
    fftw_plan p = fftw_plan_dft_r2c_2d(sizeIn, sizeOut, in, reinterpret_cast<fftw_complex*>(out), Fftw3Wrapper::fftwFlags);
    FastFTPlan* plan = new FastFTPlan(p, in, out, sizeIn, sizeOut, dimensions);
    return plans_.insert(plans_.end(), plan);
}

FastFT::T_plan_iter FastFT::createPlan(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
    int dimensions)
{
    fftw_plan p = fftw_plan_dft_c2r_2d(sizeIn, sizeOut, reinterpret_cast<fftw_complex*>(in), out, Fftw3Wrapper::fftwFlags);
    FastFTPlan* plan = new FastFTPlan(p, in, out, sizeIn, sizeOut, dimensions);
    return plans_.insert(plans_.end(), plan);
}

void FastFT::destroyAllPlans()
{
    // Explizite Iteration
    T_plan_iter it;
    for (it = plans_.begin(); it != plans_.end(); ++it) {
        (*it)->destroyPlan();
    }
    plans_.clear();
}

FastFT::T_plan_iter FastFT::find(std::complex<double>* in, std::complex<double>* out, int sizeIn,
    int sizeOut, int dimensions)
{
    // Explizite Iteration
    T_plan_iter it;
    for (it = plans_.begin(); it != plans_.end(); ++it) {
        if ((*it)->check(in, out, sizeIn, sizeOut, dimensions))
            break;
    }
    return it;
}

FastFT::T_plan_iter FastFT::find(double* in, std::complex<double>* out, int sizeIn, int sizeOut,
    int dimensions)
{
    // Explizite Iteration
    T_plan_iter it;
    for (it = plans_.begin(); it != plans_.end(); ++it) {
        if ((*it)->check(in, out, sizeIn, sizeOut, dimensions))
            break;
    }
    return it;
}

FastFT::T_plan_iter FastFT::find(std::complex<double>* in, double* out, int sizeIn, int sizeOut,
    int dimensions)
{
    // Explizite Iteration
    T_plan_iter it;
    for (it = plans_.begin(); it != plans_.end(); ++it) {
        if ((*it)->check(in, out, sizeIn, sizeOut, dimensions))
            break;
    }
    return it;
}

#endif /* FFTW3_WRAPPER_H_ */
