/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * rk4ip_lattice_integrator.h
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef RK4IP_LATTICE_INTEGRATOR_H_
#define RK4IP_LATTICE_INTEGRATOR_H_

#include "lattice_integrator.h"

/**
 * Runge-Kutta-Integration im WW-Bild
 */

template <typename T_model>
class RK4IP_LatticeIntegrator : public Lattice<T_model>
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

    RK4IP_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) : Lattice<T_model>(sizeX, sizeY, latticeSizeX, latticeSizeY)
    {
        Base::INTEGRATOR_NAME = std::string("4th Order Runge-Kutta Interaction Picture");
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].resize(latticeSizeX, latticeSizeY);
            lattice_I[component].resize(latticeSizeX, latticeSizeY);
            latticeFft_K[component].resize(latticeSizeX, latticeSizeY / 2 + 1);

            forwardPlan_K[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_K[component], latticeFft_K[component]);
            backwardPlan_K[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_K[component], lattice_K[component]);
        }
    }
    ~RK4IP_LatticeIntegrator()
    {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            fftw_destroy_plan(forwardPlan_K[component]);
            fftw_destroy_plan(backwardPlan_K[component]);
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

    blitz::Array<double, 2> lattice_K[Components::number_of_Variables];
    blitz::Array<double, 2> lattice_I[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2> latticeFft_K[Components::number_of_Variables];
    fftw_plan forwardPlan_K[Components::number_of_Variables];
    fftw_plan backwardPlan_K[Components::number_of_Variables];
};


template <typename T_model>
void RK4IP_LatticeIntegrator<T_model>::step()
{
    // 1. Calc. xi
    // 2. a_k = a
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice_K[i] = Base::lattice[i];
    }
    // 3. a = D(1/2)a
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (Base::diffusion_[component] == 0.0)
            continue;
        fftw_execute(Base::forwardPlan[component]);
        Base::latticeFft[component] = Base::latticeFft[component]
            * Base::precomputedDiffusionOperator[component];
        fftw_execute(Base::backwardPlan[component]);
    }
    // 4. a_I = a
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice_I[i] = Base::lattice[i];
    }
    // 5. a_k = h N a_k
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in, i);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }
    // 6. a_k = D(1/2)a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (Base::diffusion_[component] == 0.0)
            continue;
        fftw_execute(forwardPlan_K[component]);
        latticeFft_K[component] = latticeFft_K[component]
            * Base::precomputedDiffusionOperator[component];
        fftw_execute(backwardPlan_K[component]);
    }
    // 7. a = a + 1/6 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[component] = Base::lattice[component] + 1. / 6. * lattice_K[component];
    }
    // 8. time = time + tau/2
    Base::advanceTime(Base::HalfStep);
    // 9. a_k = a_I + 1/2 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[component] = lattice_I[component] + 1. / 2. * lattice_K[component];
    }
    // 10. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in, i);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }
    // 11. a = a + 1/3 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[component] = Base::lattice[component] + 1. / 3. * lattice_K[component];
    }
    // 12. a_K = a_i + 1/2 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[component] = lattice_I[component] + 1. / 2. * lattice_K[component];
    }
    // 13. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in, i);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }
    // 14. a = a + 1/3 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[component] = Base::lattice[component] + 1. / 3. * lattice_K[component];
    }
    // 15. time = time + tau/2
    Base::advanceTime(Base::HalfStep);
    // 16. a_K = a_i +  a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[component] = lattice_I[component] + lattice_K[component];
    }
    // 17. = 6. a_k = D(1/2)a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (Base::diffusion_[component] == 0.0)
            continue;
        fftw_execute(forwardPlan_K[component]);
        latticeFft_K[component] = latticeFft_K[component]
            * Base::precomputedDiffusionOperator[component];
        fftw_execute(backwardPlan_K[component]);
    }
    // 18. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[component] = lattice_K[component].data()[i];
        }
        Components f = static_cast<T_model*>(this)->step_f(in, i);
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }
    // 19. a = D(1/2)a
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (Base::diffusion_[component] == 0.0)
            continue;
        fftw_execute(Base::forwardPlan[component]);
        Base::latticeFft[component] = Base::latticeFft[component]
            * Base::precomputedDiffusionOperator[component];
        fftw_execute(Base::backwardPlan[component]);
    }
    // 20. =  7. a = a + 1/6 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[component] = Base::lattice[component] + 1. / 6. * lattice_K[component];
    }
}


#endif /* RK4IP_LATTICE_INTEGRATOR_H_ */
