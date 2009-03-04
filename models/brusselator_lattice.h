/*
 * brusselator_lattice.h
 *
 *  Created on: 19.01.2009
 *      Author: rikebs
 */

#ifndef BRUSSELATOR_LATTICE_H_
#define BRUSSELATOR_LATTICE_H_

#include "../lattice/rds_lattice.h"


class BrusselatorLattice;
template<>
struct Metainfo< BrusselatorLattice > : MetainfoBase {
    typedef TwoComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N> struct NoiseMapping {enum {value=-1};};
};
template <> struct Metainfo<BrusselatorLattice>::NoiseMapping< secondComponent >{ enum {value = firstComponent}; };

class BrusselatorLattice : public SIIP_LatticeIntegrator< BrusselatorLattice > {
public:

    BrusselatorLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);


    Parameter< double > a;
    Parameter< double > b;

    inline TwoComponentSystem fixpoint(int /*position*/) const
    {
        return TwoComponentSystem( fixpointU(), fixpointV() );
    }
    inline double fixpointU() const
    {
        return a();
    }
    inline double fixpointV() const
    {
        return b()/a() ;
    }

    inline double step_u(double u, double v)
    {
        return a() - (b() + 1.0 )*u + u*u*v;
    }
    inline double step_v(double u, double v)
    {
        return b() * u - u*u*v;
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




#endif /* BRUSSELATOR_LATTICE_H_ */
