/*
 * barkley_lattice.h
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#ifndef BARKLEY_LATTICE_H_
#define BARKLEY_LATTICE_H_

#include "../lattice/rds_lattice.h"
#include "../lattice/finite_interval_lattice_integrator.h"

class BarkleyLattice;
template<>
struct Metainfo< BarkleyLattice > : MetainfoBase {
    typedef TwoComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N> struct NoiseMapping {enum {value=-1};};
    static const bool HAS_EXPLICIT_STEP_FUNCTION = true;
};
template <> struct Metainfo<BarkleyLattice>::NoiseMapping< secondComponent >{ enum {value = firstComponent}; };

class BarkleyLattice : public FiniteIntervalIntegrator< BarkleyLattice > {
public:

    BarkleyLattice();

    Parameter< double > epsilon;
    Parameter< double > a;
    Parameter< double > b;

    inline TwoComponentSystem fixpoint(int /*position*/) const
    {
        return TwoComponentSystem( fixpointU(), fixpointV() );
    }
    inline double fixpointU() const
    {
        return 0;//pow( 1.0 / a(), 1. / 3. ) + 0.1;
    }
    inline double fixpointV() const
    {
        return 0 ;//-1.0 / a();
    }

    inline TwoComponentSystem step_explicit( TwoComponentSystem sys, long int , TwoComponentSystem noise ) {
        double u_old = sys.u();
        double u_th = (sys.v() + b()) / a();
        double u = (u_old <= u_th)
                 ? ( u_old / (1 - ( tau/epsilon() )*( 1 - u_old )*( u_old - u_th ) ) )
                 : ( (u_old + (tau/epsilon()) * u_old * (u_old -u_th))/ ( 1 + ( tau/epsilon() )* u_old * ( u_old - u_th )  ) );
        return TwoComponentSystem( u, sys.v() );
    }
    inline double step_u(double u, double v)
    {
        return 0;
        return (1.0 / epsilon()) * u * (1.0 - u) * (u - (v + b()) / a());
    }
    inline double step_v(double u, double v)
    {
        return u*u*u  - v;
    }

    inline TwoComponentSystem step_f(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( step_u( sys.u(), sys.v() ), step_v( sys.u(), sys.v() ) );
    }

    void toInitial(int) {

        lattice[ 0 ] = where( blitz::tensor::i > latticeSizeX() / 2.0, 1, 0 );
        lattice[ 1 ] = where( blitz::tensor::j > latticeSizeY() / 2.0, a()/2, 0 );

    }

};

#endif /* BARKLEY_LATTICE_H_ */
