/*
 * fhn_original_lattice.h
 *
 *  Created on: 19.01.2009
 *      Author: rikebs
 */

#ifndef FHN_ORIGINAL_LATTICE_H_
#define FHN_ORIGINAL_LATTICE_H_

#include "../lattice/rds_lattice.h"


class FhnOriginalLattice;
template<>
struct Metainfo< FhnOriginalLattice > : MetainfoBase {
    typedef TwoComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N> struct NoiseMapping {enum {value=-1};};
};
template <> struct Metainfo<FhnOriginalLattice>::NoiseMapping< secondComponent >{ enum {value = firstComponent}; };

class FhnOriginalLattice : public SIIP_LatticeIntegrator< FhnOriginalLattice > {
public:

    FhnOriginalLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);


    Parameter< double > a_0, a_1, epsilon;

    inline TwoComponentSystem fixpoint(int /*position*/) const
    {
        return TwoComponentSystem( fixpointU(), fixpointV() );
    }
    inline double fixpointU() const
    {
        return 0;//- sqrt( -1.0/a_1() + 1 );
    }
    inline double fixpointV() const
    {
        return 0;//( fixpointU() - a_0() ) / a_1() ;
    }

    inline double step_u(double u, double v)
    {
        return u - u*u*u - v;
    }
    inline double step_v(double u, double v)
    {
        return epsilon()*( u - a_1() * v - a_0() );
    }

    inline TwoComponentSystem step_f(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( step_u( sys.u(), sys.v() ), step_v( sys.u(), sys.v() ) );
    }

    void toInitial(int) {

        lattice[ 0 ] = where( blitz::tensor::i > latticeSizeX() / 2.0, 1, 0 );
        lattice[ 1 ] = where( blitz::tensor::j > latticeSizeY() / 2.0, a_0()/2, 0 );
    }
};




#endif /* FHN_ORIGINAL_LATTICE_H_ */
