/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * noise_generator.h
 *
 *  Created on: 24.11.2008
 *      Author: rikebs
 */

#ifndef NOISE_GENERATOR_H_
#define NOISE_GENERATOR_H_

#include <random/normal.h>
#include <blitz/array.h>

#if 0

struct zig{
static unsigned long
x = 123456789,
y = 362436069,
z = 521288629,
w = 88675123,
v = 886756453;
unsigned long xorshift(void)
{
unsigned long t;
t = x ^ (x>>7);
x = y;
y = z;
z = w;
w = v;
v = (v ^ (v << 6)) ^ (t ^ (t << 13));
return (y + y + 1) * v;
}



void    RanNormalSetSeedZig(int *piSeed, int cSeed);
double  DRanNormalZig(void);
void    RanNormalSetSeedZigVec(int *piSeed, int cSeed);
double  DRanNormalZigVec(void);
void    RanNormalSetSeedZig32(int *piSeed, int cSeed);
double  DRanNormalZig32(void);
void    RanNormalSetSeedZig32Vec(int *piSeed, int cSeed);
double  DRanNormalZig32Vec(void);

double  DRanQuanNormalZig(void);
double  DRanQuanNormalZigVec(void);
double  DRanQuanNormalZig32(void);
double  DRanQuanNormalZig32Vec(void);


#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef __LP64__
    typedef unsigned long UINT64;
    typedef          long INT64;
#define LIT_UINT64(c) (c##ul)
#define LIT_INT64(c) (c##l)
#elif defined(_MSC_VER)
    typedef unsigned __int64 UINT64;
    typedef          __int64 INT64;
#define LIT_UINT64(c) (c##ui64)
#define LIT_INT64(c) (c##i64)
#else
    typedef unsigned long long UINT64;
    typedef          long long INT64;
#define LIT_UINT64(c) (c##ull)
#define LIT_INT64(c) (c##ll)
#endif

#define M_RAN_INVM30 9.31322574615478515625e-010 /* 1.0 / 2^30 */
#define M_RAN_INVM32 2.32830643653869628906e-010 /* 1.0 / 2^32 */
#define M_RAN_INVM48 3.55271367880050092936e-015 /* 1.0 / 2^48 */
#define M_RAN_INVM52 2.22044604925031308085e-016 /* 1.0 / 2^52 */
#define M_RAN_INVM64 5.42101086242752217004e-020 /* 1.0 / 2^64 */

#define RANDBL_32old(iRan1) \
    ((unsigned int)(iRan1)*M_RAN_INVM32)
#define RANDBL_48old(iRan1, iRan2) \
    ((unsigned int)(iRan1) + (unsigned int)((iRan2) << 16) * M_RAN_INVM32) * M_RAN_INVM32
#define RANDBL_52old(iRan1, iRan2) \
    ((unsigned int)(iRan1) + (unsigned int)((iRan2) << 12) * M_RAN_INVM32) * M_RAN_INVM32

#define RANDBL_32new(iRan1) \
    ((int)(iRan1)*M_RAN_INVM32 + (0.5 + M_RAN_INVM32 / 2))
#define RANDBL_48new(iRan1, iRan2) \
    ((int)(iRan1)*M_RAN_INVM32 + (0.5 + M_RAN_INVM48 / 2) + (int)((iRan2)&0x0000FFFF) * M_RAN_INVM48)
#define RANDBL_52new(iRan1, iRan2) \
    ((int)(iRan1)*M_RAN_INVM32 + (0.5 + M_RAN_INVM52 / 2) + (int)((iRan2)&0x000FFFFF) * M_RAN_INVM52)

void    GetInitialSeeds(unsigned int auiSeed[], int cSeed,
    unsigned int uiSeed, unsigned int uiMin);

/* MWC8222 George Marsaglia */
void RanSetSeed_MWC8222(int *piSeed, int cSeed);
unsigned int IRan_MWC8222(void);
double DRan_MWC8222(void);
void VecIRan_MWC8222(unsigned int *auiRan, int cRan);
void VecDRan_MWC8222(double *adRan, int cRan);

/* plug-in RNG */
typedef double      ( * DRANFUN)(void);
typedef unsigned int( * IRANFUN)(void);
typedef void        ( * IVECRANFUN)(unsigned int *, int);
typedef void        ( * DVECRANFUN)(double *, int);
typedef void        ( * RANSETSEEDFUN)(int *, int);

void    RanSetRan(const char *sRan);
void    RanSetRanExt(DRANFUN DRanFun, IRANFUN IRanFun, IVECRANFUN IVecRanFun,
    DVECRANFUN DVecRanFun, RANSETSEEDFUN RanSetSeedFun);
double  DRanU(void);
unsigned int IRanU(void);
void    RanVecIntU(unsigned int *auiRan, int cRan);
void    RanVecU(double *adRan, int cRan);
void    RanSetSeed(int *piSeed, int cSeed);

/* normal probabilities */
double  DProbNormal(double x);

/* polar standard normal RNG */
double  FRanNormalPolar(void);
double  DRanNormalPolar(void);
double  FRanQuanNormal(void);
double  DRanQuanNormal(void);


/*---------------------------- GetInitialSeeds -----------------------------*/
void GetInitialSeeds(unsigned int auiSeed[], int cSeed,
    unsigned int uiSeed, unsigned int uiMin)
{
    int i;
    unsigned int s = uiSeed;                                    /* may be 0 */

    for (i = 0; i < cSeed; )
    {   /* see Knuth p.106, Table 1(16) and Numerical Recipes p.284 (ranqd1)*/
        s = 1664525 * s + 1013904223;
        if (s <= uiMin)
            continue;
        auiSeed[i] = s;
        ++i;
    }
}
/*-------------------------- END GetInitialSeeds ---------------------------*/


/*------------------------ George Marsaglia MWC ----------------------------*/
#define MWC_R 256
#define MWC_A LIT_UINT64(809430660)
#define MWC_AI 809430660
#define MWC_C 362436
static unsigned int s_uiStateMWC = MWC_R - 1;
static unsigned int s_uiCarryMWC = MWC_C;
static unsigned int s_auiStateMWC[MWC_R];

void RanSetSeed_MWC8222(int *piSeed, int cSeed)
{
    s_uiStateMWC = MWC_R - 1;
    s_uiCarryMWC = MWC_C;

    if (cSeed == MWC_R)
    {
        int i;
        for (i = 0; i < MWC_R; ++i)
        {
            s_auiStateMWC[i] = (unsigned int)piSeed[i];
        }
    }
    else
    {
        GetInitialSeeds(s_auiStateMWC, MWC_R, piSeed && cSeed ? piSeed[0] : 0, 0);
    }
}
unsigned int IRan_MWC8222(void)
{
    UINT64 t;

    s_uiStateMWC = (s_uiStateMWC + 1) & (MWC_R - 1);
    t = MWC_A * s_auiStateMWC[s_uiStateMWC] + s_uiCarryMWC;
    s_uiCarryMWC = (unsigned int)(t >> 32);
    s_auiStateMWC[s_uiStateMWC] = (unsigned int)t;
    return (unsigned int)t;
}
double DRan_MWC8222(void)
{
    UINT64 t;

    s_uiStateMWC = (s_uiStateMWC + 1) & (MWC_R - 1);
    t = MWC_A * s_auiStateMWC[s_uiStateMWC] + s_uiCarryMWC;
    s_uiCarryMWC = (unsigned int)(t >> 32);
    s_auiStateMWC[s_uiStateMWC] = (unsigned int)t;
    return RANDBL_32new(t);
}
void VecIRan_MWC8222(unsigned int *auiRan, int cRan)
{
    UINT64 t;
    unsigned int carry = s_uiCarryMWC, status = s_uiStateMWC;

    for (; cRan > 0; --cRan, ++auiRan)
    {
        status = (status + 1) & (MWC_R - 1);
        t = MWC_A * s_auiStateMWC[status] + carry;
        *auiRan = s_auiStateMWC[status] = (unsigned int)t;
        carry = (unsigned int)(t >> 32);
    }
    s_uiCarryMWC = carry;
    s_uiStateMWC = status;
}
void VecDRan_MWC8222(double *adRan, int cRan)
{
    UINT64 t;
    unsigned int carry = s_uiCarryMWC, status = s_uiStateMWC;

    for (; cRan > 0; --cRan, ++adRan)
    {
        status = (status + 1) & (MWC_R - 1);
        t = MWC_A * s_auiStateMWC[status] + carry;
        s_auiStateMWC[status] = (unsigned int)t;
        *adRan = RANDBL_32new(t);
        carry = (unsigned int)(t >> 32);
    }
    s_uiCarryMWC = carry;
    s_uiStateMWC = status;
}
/*----------------------- END George Marsaglia MWC -------------------------*/


/*------------------- normal random number generators ----------------------*/
static int s_cNormalInStore = 0;             /* > 0 if a normal is in store */

static DRANFUN s_fnDRanu = DRan_MWC8222;
static IRANFUN s_fnIRanu = IRan_MWC8222;
static IVECRANFUN s_fnVecIRanu = VecIRan_MWC8222;
static DVECRANFUN s_fnVecDRanu = VecDRan_MWC8222;
static RANSETSEEDFUN s_fnRanSetSeed = RanSetSeed_MWC8222;

double  DRanU(void)
{
    return (*s_fnDRanu)();
}
unsigned int IRanU(void)
{
    return (*s_fnIRanu)();
}
void RanVecIntU(unsigned int *auiRan, int cRan)
{
    (*s_fnVecIRanu)(auiRan, cRan);
}
void RanVecU(double *adRan, int cRan)
{
    (*s_fnVecDRanu)(adRan, cRan);
}
//void RanVecU(double *adRan, int cRan)
//{
//  int i, j, c, airan[256];
//
//  for (; cRan > 0; cRan -= 256)
//  {
//      c = min(cRan, 256);
//      (*s_fnVecIRanu)(airan, c);
//      for (j = 0; j < c; ++j)
//          *adRan = RANDBL_32new(airan[j]);
//  }
//}
void    RanSetSeed(int *piSeed, int cSeed)
{
    s_cNormalInStore = 0;
    (*s_fnRanSetSeed)(piSeed, cSeed);
}
void    RanSetRan(const char *sRan)
{
    s_cNormalInStore = 0;
    if (strcmp(sRan, "MWC8222") == 0)
    {
        s_fnDRanu = DRan_MWC8222;
        s_fnIRanu = IRan_MWC8222;
        s_fnVecIRanu = VecIRan_MWC8222;
        s_fnRanSetSeed = RanSetSeed_MWC8222;
    }
    else
    {
        s_fnDRanu = NULL;
        s_fnIRanu = NULL;
        s_fnVecIRanu = NULL;
        s_fnRanSetSeed = NULL;
    }
}
static unsigned int IRanUfromDRanU(void)
{
    return (unsigned int)(UINT_MAX * (*s_fnDRanu)());
}
static double DRanUfromIRanU(void)
{
    return RANDBL_32new( (*s_fnIRanu)() );
}
void    RanSetRanExt(DRANFUN DRanFun, IRANFUN IRanFun, IVECRANFUN IVecRanFun,
    DVECRANFUN DVecRanFun, RANSETSEEDFUN RanSetSeedFun)
{
    s_fnDRanu = DRanFun ? DRanFun : DRanUfromIRanU;
    s_fnIRanu = IRanFun ? IRanFun : IRanUfromDRanU;
    s_fnVecIRanu = IVecRanFun;
    s_fnVecDRanu = DVecRanFun;
    s_fnRanSetSeed = RanSetSeedFun;
}
/*---------------- END uniform random number generators --------------------*/


/*----------------------------- Polar normal RNG ---------------------------*/
#define POLARBLOCK(u1, u2, d)      \
    do {                           \
        u1 = (*s_fnDRanu)();       \
        u1 = 2 * u1 - 1;           \
        u2 = (*s_fnDRanu)();       \
        u2 = 2 * u2 - 1;           \
        d = u1 * u1 + u2 * u2;     \
    } while (d >= 1);              \
    d = sqrt((-2.0 / d) * log(d)); \
    u1 *= d;                       \
    u2 *= d

static double s_dNormalInStore;

double  DRanNormalPolar(void)                         /* Polar Marsaglia */
{
    double d, u1;

    if (s_cNormalInStore)
        u1 = s_dNormalInStore, s_cNormalInStore = 0;
    else
    {
        POLARBLOCK(u1, s_dNormalInStore, d);
        s_cNormalInStore = 1;
    }

return u1;
}

};

#endif

class NoiseGenerator;

#include "lattice_geometry.h"
#include "lattice_interface.h"

/**
 * Base class for implementing noise
 */
class NoiseGenerator : public LatticeGeometry
{
public:
    NoiseGenerator(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    NoiseGenerator(const LatticeGeometry& geometry);
    NoiseGenerator(const LatticeInterface* lattice);
    virtual ~NoiseGenerator();
    /**
     * This method will be called by the integrator before doing the actual integration.
     *
     * Additional parameters will have to be given with other methods.
     */
    virtual void precomputeNoise(double correlation, double intensity) = 0;
    double getPrecomputedNoise(int i) const;

    void connectToLattice(const LatticeInterface* lattice);
    void disconnectFromLattice();

    std::string getModelName();

protected:
    blitz::Array<double, 2> noiseLattice_;
    ranlib::Normal<double> blitz_normal;
    double DRanNormalZig32() { return DRanNormalZig32(); }
private:
    void init();
    const LatticeInterface* lattice_;
    std::string modelName;
};

/**
 * This method will be called by the integrator when it has to insert the noise.
 */
inline double NoiseGenerator::getPrecomputedNoise(int i) const
{

    return noiseLattice_.data()[i];
}

#endif /* NOISE_GENERATOR_H_ */
