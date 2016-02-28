/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * ark45ip_lattice_integrator.h
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef ARK45IP_LATTICE_INTEGRATOR_H_
#define ARK45IP_LATTICE_INTEGRATOR_H_

#include "lattice_integrator.h"

/**
 * Adaptive Runge-Kutta-Integration im WW-Bild
 */

template <typename T_model>
class ARK45IP_LatticeIntegrator : public Lattice<T_model>
{
protected:
    typedef Lattice<T_model> Base;
    //typedef typename M::TestComponents TestComponents_;

    typedef typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables ? Base::number_of_Noise_Variables : 1> T_noiseVector;

    //typedef typename _tvec<Base::number_of_Noise_Variables>::TinyVectorWithZero T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;

public:
    //    static int const SIIP_ITERATIONS = Metainfo< T_model >::SIIP_ITERATIONS;

    ARK45IP_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) : Lattice<T_model>(sizeX, sizeY, latticeSizeX, latticeSizeY)
    {
        Base::INTEGRATOR_NAME = std::string("Adaptive 4/5th Order Runge-Kutta Interaction Picture");
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_I[component].resize(latticeSizeX, latticeSizeY);
            lattice_J[component].resize(latticeSizeX, latticeSizeY);
            lattice_K[component].resize(latticeSizeX, latticeSizeY);
            lattice_L[component].resize(latticeSizeX, latticeSizeY);
            lattice_star[component].resize(latticeSizeX, latticeSizeY);

            latticeFft_I[component].resize(latticeSizeX, latticeSizeY / 2 + 1);
            latticeFft_J[component].resize(latticeSizeX, latticeSizeY / 2 + 1);
            latticeFft_K[component].resize(latticeSizeX, latticeSizeY / 2 + 1);
            latticeFft_L[component].resize(latticeSizeX, latticeSizeY / 2 + 1);
            latticeFft_star[component].resize(latticeSizeX, latticeSizeY / 2 + 1);

            forwardPlan_I[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_I[component], latticeFft_I[component]);
            backwardPlan_I[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_I[component], lattice_I[component]);
            forwardPlan_J[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_J[component], latticeFft_J[component]);
            backwardPlan_J[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_J[component], lattice_J[component]);
            forwardPlan_K[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_K[component], latticeFft_K[component]);
            backwardPlan_K[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_K[component], lattice_K[component]);
            forwardPlan_L[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_L[component], latticeFft_L[component]);
            backwardPlan_L[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_L[component], lattice_L[component]);
            forwardPlan_star[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_star[component], latticeFft_star[component]);
            backwardPlan_star[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_star[component], lattice_star[component]);
        }
    }
    ~ARK45IP_LatticeIntegrator()
    {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            fftw_destroy_plan(forwardPlan_K[component]);
            fftw_destroy_plan(backwardPlan_K[component]);
            fftw_destroy_plan(forwardPlan_L[component]);
            fftw_destroy_plan(backwardPlan_L[component]);
            fftw_destroy_plan(forwardPlan_I[component]);
            fftw_destroy_plan(backwardPlan_I[component]);
            fftw_destroy_plan(forwardPlan_J[component]);
            fftw_destroy_plan(backwardPlan_J[component]);
            fftw_destroy_plan(forwardPlan_star[component]);
            fftw_destroy_plan(backwardPlan_star[component]);
        }
    }
    void step(int numberOfSteps)
    {
        for (int i = 0; i < numberOfSteps; ++i)
            step();
    }
    void step();

protected:
    void step_dt(long int latticePoint);
    //Components step_f(Components);
    //Components step_g(Components);


    blitz::Array<double, 2> lattice_I[Components::number_of_Variables];
    blitz::Array<double, 2> lattice_J[Components::number_of_Variables];
    blitz::Array<double, 2> lattice_K[Components::number_of_Variables];
    blitz::Array<double, 2> lattice_L[Components::number_of_Variables];
    blitz::Array<double, 2> lattice_star[Components::number_of_Variables];


    blitz::Array<std::complex<double>, 2> latticeFft_I[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2> latticeFft_J[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2> latticeFft_K[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2> latticeFft_L[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2> latticeFft_star[Components::number_of_Variables];

    fftw_plan forwardPlan_I[Components::number_of_Variables];
    fftw_plan backwardPlan_I[Components::number_of_Variables];
    fftw_plan forwardPlan_J[Components::number_of_Variables];
    fftw_plan backwardPlan_J[Components::number_of_Variables];
    fftw_plan forwardPlan_K[Components::number_of_Variables];
    fftw_plan backwardPlan_K[Components::number_of_Variables];
    fftw_plan forwardPlan_L[Components::number_of_Variables];
    fftw_plan backwardPlan_L[Components::number_of_Variables];
    fftw_plan forwardPlan_star[Components::number_of_Variables];
    fftw_plan backwardPlan_star[Components::number_of_Variables];

    void diffOp(blitz::Array<std::complex<double>, 2>* bz, double a);
};

template <typename T_model>
void ARK45IP_LatticeIntegrator<T_model>::diffOp(blitz::Array<std::complex<double>, 2>* bz,
    double a)
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (Base::diffusion_[component] == 0.0)
            continue;
        blitz::Array<std::complex<double>, 2> diffusionOperator(bz[component].shape());
        int max_x_out = (Base::latticeSizeX());
        int max_y_out = (Base::latticeSizeY()) / 2 + 1;
        static const long double SQRT_M_PIl_POW_3l = pow(sqrt(M_PIl * 2.), 3);
        for (int i = 0; i < max_x_out; ++i) {
            for (int j = 0; j < max_y_out; ++j) {
                long double f_x = (i < Base::latticeSizeX() / 2) ? i : Base::latticeSizeX() - i;
                long double f_y = j;
                f_x = f_x * SQRT_M_PIl_POW_3l / Base::sizeX();
                f_y = f_y * SQRT_M_PIl_POW_3l / Base::sizeY();

                diffusionOperator(i, j) = (exp(-(f_x * f_x + f_y * f_y) * Base::diffusion_[component]
                    * Base::tau * a));
            }
        }
        bz[component] = bz[component] * diffusionOperator;
    }
}

template <typename T_model>
void ARK45IP_LatticeIntegrator<T_model>::step()
{

    // Cash-Karp coefficients
    double _a[7];
    double _b[7][7];
    double _c[7];
    double _cs[7];
    double _f[7];

    _a[0] = 0.0;
    _a[1] = 0.0;
    _a[2] = 1.0 / 5;
    _a[3] = 3.0 / 10;
    _a[4] = 3.0 / 5;
    _a[5] = 1.0;
    _a[6] = 7.0 / 8.0;

    _b[2][1] = 1.0 / 5;
    _b[3][1] = 3.0 / 40;
    _b[3][2] = 9.0 / 40;
    _b[4][1] = 3.0 / 10;
    _b[4][2] = -9.0 / 10;
    _b[4][3] = 6.0 / 5;
    _b[5][1] = -11.0 / 54;
    _b[5][2] = 5.0 / 2;
    _b[5][3] = -70.0 / 27;
    _b[5][4] = 35.0 / 27;
    _b[6][1] = 1631.0 / 55296;
    _b[6][2] = 175.0 / 512;
    _b[6][3] = 575.0 / 13824;
    _b[6][4] = 44275.0 / 110592;
    _b[6][5] = 253.0 / 4096;

    _c[0] = 0.0;
    _c[1] = 37.0 / 378;
    _c[2] = 0.0;
    _c[3] = 250.0 / 621;
    _c[4] = 125.0 / 594;
    _c[5] = 0.0;
    _c[6] = 512.0 / 1771;

    _cs[0] = 0.0;
    _cs[1] = 2825.0 / 27648;
    _cs[2] = 0.0;
    _cs[3] = 18575.0 / 48384;
    _cs[4] = 13525.0 / 55296;
    _cs[5] = 277.0 / 14336;
    _cs[6] = 1.0 / 4;

    double _g = _c[1] * _cs[4] - _cs[1] * _c[4];
    _f[0] = 0.0;
    _f[1] = (_b[6][4] * (_cs[1] - _c[1]) + _b[6][1] * (_c[4] - _cs[4])) / _g + 1.0;
    _f[2] = _b[6][2];
    _f[3] = (_b[6][4] * (_cs[1] * _c[3] - _c[1] * _cs[3]) + _b[6][1] * (_cs[3] * _c[4] - _c[3] * _cs[4])) / _g + _b[6][3];
    _f[4] = (_b[6][1] * _cs[4] - _b[6][4] * _cs[1]) / _g;
    _f[5] = _b[6][5] + _cs[5] * (_b[6][1] * _c[4] - _b[6][4] * _c[1]) / _g;
    _f[6] = (-_b[6][1] * _c[4] + _b[6][4] * _c[1]) / _g;

    // 0. a in Fourier
    LOOP_COMPONENTS
    {
        fftw_execute(Base::forwardPlan[component]);
    }
    // 1. Calc. xi
    // 2. a_k = a
    LOOP_COMPONENTS
    {
        latticeFft_K[component] = Base::latticeFft[component];
    }
    // 3. a = D(a2)a
    diffOp(Base::latticeFft, _a[2]);
    // 4. aI = a
    LOOP_COMPONENTS latticeFft_I[component] = Base::latticeFft[component];
    //5. a∗ = a
    LOOP_COMPONENTS latticeFft_star[component] = Base::latticeFft[component];
    // 6. aK = h F [N (x0 , F −1 [aK ] , ξ)]
    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_K[component]);
        lattice_K[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_K[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_K[component] = Base::tau * latticeFft_K[component];
    }
    // 7. aK = e−a2 hL(x       )·a
    diffOp(latticeFft_K, -_a[2]);
    // 8. a = a + c1 aK
    LOOP_COMPONENTS
    Base::latticeFft[component] = Base::latticeFft[component] + _c[1] * latticeFft_K[component];


    // 9. a∗ = a∗ + c1∗ aK
    LOOP_COMPONENTS
    latticeFft_star[component] = latticeFft_star[component] + _cs[1] * latticeFft_K[component];

    // 10. x0 = x0 + a2 h
    Base::time_ = Base::time_ + _a[2] * Base::tau;
    // 11. aK = aI + b21 aK
    LOOP_COMPONENTS
    latticeFft_K[component] = latticeFft_I[component] + _b[2][1] * latticeFft_K[component];
    // 12. aK = h F [N (x0 , F −1 [aK ] , ξ)]
    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_K[component]);
        lattice_K[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_K[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_K[component] = Base::tau * latticeFft_K[component];
    }


    // 13. aJ = (1 − b31 /c1 )aI + b31 /c1 a + b32 aK
    LOOP_COMPONENTS
    latticeFft_J[component] = (1.0 - _b[3][1] / _c[1]) * latticeFft_I[component] + _b[3][1] / _c[1] * Base::latticeFft[component] + _b[3][2] * latticeFft_K[component];

    // 14. x0 = x0 + (a3 − a2 )h
    Base::time_ = Base::time_ + (_a[3] - _a[2]) * Base::tau;
    // 15.
    diffOp(latticeFft_J, _a[3] - _a[2]);

    //16. aJ = h F [N (x0 , F −1 [aJ ] , ξ)]

    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_J[component]);
        lattice_J[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_J[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_J[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_J[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_J[component] = Base::tau * latticeFft_J[component];
    }

    // 17. aJ = e−(a3 −a2 )hL(x          )·a
    diffOp(latticeFft_J, -(_a[3] - _a[2]));
    // 18.
    LOOP_COMPONENTS
    {
        latticeFft_L[component] = (1.0 - _b[4][1] / _c[1]) * latticeFft_I[component] + _b[4][1] / _c[1] * Base::latticeFft[component] + _b[4][2] * latticeFft_K[component] + _b[4][3] * latticeFft_J[component];
    }

    LOOP_COMPONENTS
    {
        // 19. a = a + c3 aJ
        Base::latticeFft[component] = Base::latticeFft[component] + _c[3] * latticeFft_J[component];
        // 20. a∗ = a∗ + c3∗ aJ
        latticeFft_star[component] = latticeFft_star[component] + _cs[3] * latticeFft_J[component];
    }

    // 21. x0 = x0 + (a4-a3)h
    Base::time_ = Base::time_ + (_a[4] - _a[3]) * Base::tau;
    //22.
    diffOp(latticeFft_L, _a[4] - _a[2]);
    // 23. aL
    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_L[component]);
        lattice_L[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_L[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_L[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_L[component] = Base::tau * latticeFft_L[component];
    }
    // 24.
    diffOp(latticeFft_L, -(_a[4] - _a[2]));
    // 25. 26
    LOOP_COMPONENTS
    {
        Base::latticeFft[component] = Base::latticeFft[component] + _c[4] * latticeFft_L[component];
        latticeFft_star[component] = latticeFft_star[component] + _cs[4] * latticeFft_L[component];
    }
    // 27.
    LOOP_COMPONENTS
    latticeFft_L[component] = (1 - _b[5][1] / _c[1]) * latticeFft_I[component] + _b[5][1] / _c[1] * Base::latticeFft[component] + _b[5][2] * latticeFft_K[component] + (_b[5][3] - _b[5][1] * _c[3] / _c[1]) * latticeFft_J[component] + (_b[5][4] - _b[5][1] * _c[4] / _c[1]) * latticeFft_L[component];

    // 28.
    Base::time_ += Base::tau * (_a[5] - _a[4]);
    // 29.
    diffOp(latticeFft_L, _a[5] - _a[2]);
    // 30.

    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_L[component]);
        lattice_L[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_L[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_L[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_L[component] = Base::tau * latticeFft_L[component];
    }
    // 31.
    diffOp(latticeFft_L, -(_a[5] - _a[2]));
    // 32.
    LOOP_COMPONENTS
    latticeFft_star[component] = latticeFft_star[component] + _cs[5] * latticeFft_L[component];
    // 33.
    LOOP_COMPONENTS
    latticeFft_L[component] = _f[1] * latticeFft_L[component] + _f[2] * latticeFft_K[component]
        + _f[3] * latticeFft_J[component] + _f[4] * Base::latticeFft[component] + _f[5] * latticeFft_L[component] + _f[6] * latticeFft_star[component];
    // 34.
    Base::time_ += Base::tau * (_a[6] - _a[5]);
    // 35.
    diffOp(latticeFft_L, _a[6] - _a[2]);
    // 36.

    LOOP_COMPONENTS
    {
        fftw_execute(backwardPlan_L[component]);
        lattice_L[component] /= static_cast<double>(Base::latticeSize());
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_L[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS
    {
        fftw_execute(forwardPlan_L[component]);
    }
    LOOP_COMPONENTS
    {
        latticeFft_L[component] = Base::tau * latticeFft_L[component];
    }

    // 37.
    diffOp(latticeFft_L, -(_a[6] - _a[2]));
    // 38. 39. // J oder L???
    LOOP_COMPONENTS
    {
        Base::latticeFft[component] = Base::latticeFft[component] + _c[6] * latticeFft_L[component];
        latticeFft_star[component] = latticeFft_star[component] + _cs[6] * latticeFft_L[component];
    }

    // 40.
    Base::time_ = Base::time_ + (_a[6] - _a[5]) * Base::tau;
    // 41. 42.
    diffOp(Base::latticeFft, 1 - _a[2]);
    diffOp(latticeFft_star, 1 - _a[2]);


    LOOP_COMPONENTS
    {
        fftw_execute(Base::backwardPlan[component]);
        Base::lattice[component] /= static_cast<double>(Base::latticeSize());
    }
}


#endif /* ARK45IP_LATTICE_INTEGRATOR_H_ */
