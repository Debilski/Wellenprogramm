/*
 * ginzburg_landau_lattice.h
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#ifndef GINZBURG_LANDAU_LATTICE_H_
#define GINZBURG_LANDAU_LATTICE_H_

#include "../lattice/siip_lattice_integrator.h"

FOUR_COMPONENT_SYSTEM( GLComponentSystem, A, AI, B, BI)

class GinzburgLandauLattice;
/**
 * Helper Class for additional but dependend Information
 */
template<>
struct Metainfo< GinzburgLandauLattice > : MetainfoBase {
    typedef GLComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template<int N> struct NoiseMapping {
        enum {
            value = -1
        };
    };
    static const int SIIP_ITERATIONS = 3;
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template<> struct Metainfo< GinzburgLandauLattice >::NoiseMapping< secondComponent > {
    enum {
        value = firstComponent
    };
};

class GinzburgLandauLattice : public SIIP_LatticeIntegrator< GinzburgLandauLattice > {
public:
    GinzburgLandauLattice();
    Parameter< double > eta, r, s, etaB, rB, sB;

    inline double step_A(double A, double AI, double B, double BI)
    {
		return eta * A + r * A * AI * A + s * A * AI * B;
    }

    inline double step_AI(double A, double AI, double B, double BI)
    {
		return eta * AI + r * A * AI * AI + s * A * AI * BI;
    }

    inline double step_B(double A, double AI, double B, double BI)
    {
		return etaB * B + rB * B * BI * B + s * B * BI * A;
    }

    inline double step_BI(double A, double AI, double B, double BI)
    {
		return etaB * BI + rB * B * BI * BI + sB * B * BI * AI;
    }

    inline GLComponentSystem step_f(GLComponentSystem s, long int)
    {
        return GLComponentSystem( step_A(s.A(), s.AI(), s.B(), s.BI()), step_AI(s.A(), s.AI(), s.B(),
        s.BI()), step_B(s.A(), s.AI(), s.B(), s.BI()), step_BI(s.A(), s.AI(), s.B(), s.BI()));
    }

    inline GLComponentSystem fixpoint(int) const
    {
        return GLComponentSystem( 0, 0, 0, 0 );
    }
};
#endif /* FHN_LATTICE_H_ */
