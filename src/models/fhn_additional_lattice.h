/*
 * fhn_additional_lattice.h
 *
 *  Created on: 29.12.2008
 *      Author: rikebs
 */

#ifndef FHN_ADDITIONAL_LATTICE_H_
#define FHN_ADDITIONAL_LATTICE_H_

#include "../lattice/rds_lattice.h"

/**
 * \class FhnAdditionalLattice
 * Implementierung eines FitzHugh-Nagumo-Modells mit senkrechter Inhibitor-Nullkline
 *
 * The smallest funtions are to be inlined, so we mention them in the header file.
 */
class FhnAdditionalLattice;
/**
 * Helper Class for additional but dependent Information
 */
template <>
struct Metainfo<FhnAdditionalLattice> : MetainfoBase
{
    typedef ThreeComponentSystem Components;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const int number_of_Noise_Variables = 1;
};
template <>
struct Metainfo<FhnAdditionalLattice>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};

class FhnAdditionalLattice : public SIIP_LatticeIntegrator<FhnAdditionalLattice>
{
public:
    typedef TwoComponentSystem TestComponents;
    inline FhnAdditionalLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<FhnAdditionalLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY)
                                                                                            //, lambda(2, "lambda"), k3(8.5, "k3"), k1(-6.92, "k1", -100, 100), tau(48, "tau"), theta(1, "theta")
                                                                                            ,
                                                                                            alph(1.0, "alpha"),
                                                                                            eps(0.1, "epsilon"),
                                                                                            thet(0.01, "theta"),
                                                                                            gamm(0.1, "gamma")
    {
        componentInfos[0] = ComponentInfo("Aktivator", "u", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "v", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Feedback", "w", -2.2, 2.5);  // Diff 0.5

        modelName_ = "FitzHugh-Nagumo w/ Feedback";
        /*
        registerParameter( &lambda );
        registerParameter( &k3 );
        registerParameter( &k1 );
        registerParameter( &tau );
        registerParameter( &theta );
*/
        registerParameter(&alph);
        registerParameter(&eps);
        registerParameter(&thet);
        registerParameter(&gamm);
    }
    Parameter<double> alph, eps, thet, gamm;

    /*    inline double epsilon() const {return epsilon_;}
     inline double gamma() const {return gamma_;}

     inline void epsilon(double& epsilon) { epsilon_ = epsilon; }
     inline void gamma(double& gamma) { gamma_ = gamma; }
     */
    inline double fixpointU() const
    {
        return -1.0 - gamm();
    }
    inline double fixpointV() const
    {
        double u0 = fixpointU();
        return 3. * u0 - u0 * u0 * u0;
    }
    inline double fixpointW() const
    {
        return 0;
    }
    inline double step_du(double u, double v, double /*w*/)
    {
        return (3.0 * u - u * u * u - v) / eps;  // - k3*w + k1;
    }
    inline double step_dv(double u, double /*v*/, double w)
    {
        //        return (u - v) / tau;
        return 1. + this->gamm() + u + this->alph() * w;
    }
    inline double step_dw(double u, double /*v*/, double w)
    {
        return (u - w - fixpointU()) / this->thet();
    }
    inline ThreeComponentSystem step_f(ThreeComponentSystem sys, long int)
    {
        return ThreeComponentSystem(step_du(sys.u(), sys.v(), sys.w()), step_dv(sys.u(), sys.v(), sys.w()), step_dw(sys.u(), sys.v(), sys.w()));
    }
    inline ThreeComponentSystem fixpoint(long int) const
    {
        return ThreeComponentSystem(fixpointU(), fixpointV(), fixpointW());
    }
};


#endif /* FHN_ADDITIONAL_LATTICE_H_ */
