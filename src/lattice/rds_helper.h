/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * rds_helper.h
 *
 *  Created on: 01.12.2008
 *      Author: rikebs
 */

#ifndef RDS_HELPER_H_
#define RDS_HELPER_H_

#include <cmath>


#ifndef M_PIl
/** The constant Pi in high precision */
#define M_PIl 3.1415926535897932384626433832795029L
#endif
#ifndef M_GAMMAl
/** Euler's constant in high precision */
#define M_GAMMAl 0.5772156649015328606065120900824024L
#endif
#ifndef M_LN2l
/** the natural logarithm of 2 in high precision */
#define M_LN2l 0.6931471805599453094172321214581766L
#endif


/**
 * Template for automatical checking, whether all array’s components are small enough.
 *
 * \param N   Compile time size of the Array
 */
template <unsigned int N>
struct CheckTininess
{
    /**
     * Checks if the Array @a array is smaller than 1E-7.
     * \param array Array of doubles with compile time size @a N
     */
    static inline bool isTiny(const double* array)
    {
        return (std::fabs(array[N - 1]) < 1E-7)
            && CheckTininess<N - 1>::isTiny(array);
    }
};

/*template<>
 struct CheckTininess< 2 > {
 static inline bool isTiny(const double* array)
 {
 return (abs( array[ 0 ] ) < 1E-7) && (abs( array[ 1 ] ) < 1E-7);
 }
 };
 */

/**
 * Template specialisation for N=1
 */
template <>
struct CheckTininess<1>
{
    static bool isTiny(const double* array)
    {
        return (std::fabs(array[0]) < 1E-7);
    }
};

/**
 * Template specialisation for N=0. Returns the neutral element.
 */
template <>
struct CheckTininess<0>
{
    static bool isTiny(const double* /*array*/)
    {
        // Should never be executed.
        return true;
    }
};


/**
 * Kleine Behelfsfunktion, die prüft, ob @b i gerade ist.
 */
inline bool isEven(int i)
{
    return i % 2 == 0;
}

/**
 * Bestimmt das kgV von @b a und @b b.
 */
unsigned long leastCommonMultiple(int a, int b);


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


#define EASY_LATTICE_NOISE_MAPPING(component, noiseComponent)   \
    template <>                                                 \
    struct Metainfo<EASY_LATTICE_NAME>::NoiseMapping<component> \
    {                                                           \
        enum                                                    \
        {                                                       \
            value = noiseComponent                              \
        };                                                      \
    };


#endif /* RDS_HELPER_H_ */
