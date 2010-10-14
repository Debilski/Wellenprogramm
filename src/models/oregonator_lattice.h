#ifndef OREGONATOR_LATTICE_H
#define OREGONATOR_LATTICE_H

#include "../lattice/rds_lattice.h"


class OregonatorLattice;
template<>
struct Metainfo< OregonatorLattice > : public MetainfoBase {
    typedef ThreeComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template<int N> struct NoiseMapping {
        enum {
            value = -1
        };
    };
};
class OregonatorLattice : public SIIP_LatticeIntegrator< OregonatorLattice > {
public:

    inline OregonatorLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
        SIIP_LatticeIntegrator< OregonatorLattice > ( sizeX, sizeY, latticeSizeX, latticeSizeY ),
            epsilon( 0.1, "epsilon", 0.0, 100.0 ), epsilon_prime( 0.1, "epsilon'", 0.0, 100.0 ), q(
                0.1, "q", 0.0, 100.0 ), f( 0.1, "f", 0.0, 100.0 )
    {
        componentInfos[ 0 ] = ComponentInfo( "X", "x", -2.2, 2.5 );
        componentInfos[ 1 ] = ComponentInfo( "Y", "y", -2.2, 2.5 );
        componentInfos[ 2 ] = ComponentInfo( "Z", "z", -2.2, 2.5 );

        registerParameter( &epsilon );
        registerParameter( &epsilon_prime );
        registerParameter( &q );
        registerParameter( &f );

    }
    Parameter< double > epsilon;
    Parameter< double > epsilon_prime;
    Parameter< double > q;
    Parameter< double > f;

    inline ThreeComponentSystem fixpoint(long int) const
    {
        return ThreeComponentSystem( fixpointU(), fixpointV(), fixpointW() );
    }
    inline double fixpointU() const
    {
        return -1;
    }
    inline double fixpointV() const
    {
        return -1;
    }
    inline double fixpointW() const
    {
        return -fixpointU();
    }
    inline double step_du(double u, double v, double /*w*/)
    {
        return (1.0 / epsilon()) * (q() * v - u * v + u * (1.0 - u));
    }
    inline double step_dv(double u, double v, double w)
    {
        return (1.0 / epsilon_prime()) * (-q() * v - u * v + f() * w);
    }
    inline double step_dw(double u, double /*v*/, double w)
    {
        return (u - w);
    }
    inline ThreeComponentSystem step_f(ThreeComponentSystem sys, long int)
    {
        return ThreeComponentSystem( step_du( sys.u(), sys.v(), sys.w() ), step_dv(
            sys.u(), sys.v(), sys.w() ), step_dw( sys.u(), sys.v(), sys.w() ) );
    }
    inline int noiseVariable() const
    {
        return 0;
    }

};

#endif
