/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * easy_lattice_model.h
 *
 *  Created on: 02.01.2009
 *      Author: rikebs
 */

/**
 * Liefert eine Bequemlichkeitsstruktur, die automatisch ein Modell einrichtet.
 */

#ifndef EASY_LATTICE_INTEGRATOR
#define EASY_LATTICE_INTEGRATOR SIIP_LatticeIntegrator
#endif

#ifndef EASY_LATTICE_NUMBER_OF_NOISE
#define EASY_LATTICE_NUMBER_OF_NOISE 1
#endif

#define _QUOTEME(x) #x
#define QUOTEME(x) _QUOTEME(x)


class EASY_LATTICE_NAME;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<EASY_LATTICE_NAME> : MetainfoBase
{
    typedef EASY_LATTICE_COMPONENTS Components;
    static const int number_of_Noise_Variables = EASY_LATTICE_NUMBER_OF_NOISE;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
};


class EASY_LATTICE_NAME : public EASY_LATTICE_INTEGRATOR<EASY_LATTICE_NAME>
{
public:
    EASY_LATTICE_NAME(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : EASY_LATTICE_INTEGRATOR<EASY_LATTICE_NAME>(sizeX, sizeY, latticeSizeX, latticeSizeY)
    {
        modelName_ = QUOTEME(EASY_LATTICE_NAME);
    }
    inline EASY_LATTICE_COMPONENTS step_f(EASY_LATTICE_COMPONENTS MODEL)
    {
        return EASY_LATTICE_COMPONENTS EASY_LATTICE_REACTION;
    }
    inline EASY_LATTICE_COMPONENTS fixpoint(int /* position */) const
    {
        return EASY_LATTICE_COMPONENTS EASY_LATTICE_FIXPOINT;
    }

    Parameter<double> EASY_LATTICE_PARAMETER;

#define EASY_LATTICE_MODEL_END \
    }                          \
    ;
