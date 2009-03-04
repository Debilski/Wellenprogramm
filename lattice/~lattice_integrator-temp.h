/*
 * lattice_integrator.h
 *
 *  Created on: 21.11.2008
 *      Author: rikebs
 */

#ifndef LATTICE_INTEGRATOR_H_
#define LATTICE_INTEGRATOR_H_

#ifndef RDS_LATTICE_H_
#error must be included via rds_lattice.h
#endif

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

/**
 * This Class provides the actual integration.
 *
 * The LatticeIntegrator class employs the Curiously Recurring Template Pattern,
 * which means, that it binds to its child class already at compile time.
 * This means that the simple @c dt functions can be inlined. Which greatly reduces
 * execution time compared to a solution with @b virtual call, which pratically can’t
 * be inlined -- the additional cost of a @b virtual could not make it much worse then.
 *
 * \param T_model     The model implementation which subclasses this template
 * \param Components   The component system
 */
template<typename T_model>
class LatticeIntegrator : public Lattice< T_model > {
protected:
    typedef Lattice< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables> T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;
public:
    LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    Lattice< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
    }

    void step(int numberOfSteps = 1);

    void det(int latticePoint);

    Components step_df(Components);
    Components step_dg(Components);
    static void detProxy(
        LatticeIntegrator< T_model>* lattice,
        int loopNum, int loopCount);
    void
    integration(
        typename Lattice< T_model>::DiffusionStepWidth firstStep,
        typename Lattice< T_model>::DiffusionStepWidth lastStep);

    T_model* subModel;
};

template<typename T_model>
class FiniteIntervalIntegrator : public LatticeIntegrator< T_model > {
    typedef LatticeIntegrator< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
public:
    FiniteIntervalIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    LatticeIntegrator< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
    }
    void executeDiffusion(typename Base::DiffusionStepWidth stepSize);
};

template<typename T_model, unsigned int N_steps>
struct LatticeIntegratorLoop {
    typedef blitz::TinyVector< double, T_model::number_of_Noise_Variables > T_noiseVector;
    typedef blitz::TinyVector< double, T_model::number_of_Variables > T_componentsVector;

    static void noiseMapping(T_componentsVector& noiseOut, T_noiseVector& noiseIn,
                             double multiplicator)
    {
        int mapping = Metainfo< T_model >::template NoiseMapping< N_steps - 1 >::value;
        if ( mapping >= 0 )
            noiseOut[ N_steps - 1 ] = multiplicator * noiseIn[ mapping ];
        LatticeIntegratorLoop< T_model, N_steps - 1 >::noiseMapping(
            noiseOut, noiseIn, multiplicator );
    }
};

template<typename T_model>
struct LatticeIntegratorLoop< T_model, 0 > {
    typedef blitz::TinyVector< double, T_model::number_of_Noise_Variables > T_noiseVector;
    typedef blitz::TinyVector< double, T_model::number_of_Variables > T_componentsVector;

    static void noiseMapping(T_componentsVector&, T_noiseVector&, double)
    {
    }
};

/**
 * Reaction method.
 *
 *  This method handles accessing the child class and calls the integration for each and
 *  every latticePoint there. As this
 *  function is not virtually linked, it is possible to be inlined, which makes sense for
 *  integration schemes with only a small overhead.
 *
 *  This special implementation integrates semi-implicitely using an iteration for root-finding.
 *  The iteration stops when the values in the dt-part are smaller than…
 *
 *  Das Rauschen wurde vorab berechnet und in der Funktion über getPrecomputedNoise abgerufen
 *
 * \param latticePoint  Current index on the lattice
 */
template<typename T_model>
inline void LatticeIntegrator< T_model >::det(int latticePoint)
{

    blitz::TinyVector< double, Components::number_of_Variables > initial;
    blitz::TinyVector< double, Components::number_of_Variables > values;
    //double initial[ Components::number_of_Variables ];
    //double values[ Components::number_of_Variables ];

    for (uint component = 0; component < Components::number_of_Variables; ++component) {

        initial[ component ] = Base::latticeDataPointer[ component ][ latticePoint ];
        values[ component ] = initial[ component ];

    }

    /*  for (uint component = 0; component < Components::number_of_Variables; ++component) {
     initial[ component ]
     = static_cast< T_model* > ( this )->lattice[ component ].data()[ latticepoint ];

     values[ component ] = initial[ component ];
     }*/
    //values = initial;

    // noise
    /*blitz::TinyVector< double, Components::number_of_Variables > xi;
     xi = 0;
     xi[1] = sqrt(2. * this->noiseIntensity() * this->tau) * noise;
     */

    // noiseVector
    T_componentsVector xi;


    //double d[ Components::number_of_Variables ];
    xi = 0;

    // noiseVector mapping
    double intensity = Base::noiseIntensity();
    if ( intensity != 0 ) {
std::cout << intensity << std::endl;
        T_noiseVector noise;
        for (uint noiseComponents = 0; noiseComponents < Base::number_of_Noise_Variables; ++noiseComponents)
        {
            noise[ noiseComponents ] = Base::noiseGenerator_[ noiseComponents ]->getPrecomputedNoise(
                latticePoint );
        }

        double mult = sqrt( 2.0 * Base::noiseIntensity() * Base::tau );
        LatticeIntegratorLoop< T_model, Components::number_of_Variables >::noiseMapping(
            xi, noise, mult );
        //        xi[ static_cast< T_model* > ( this )->noiseVariable() ] = sqrt( 2.
        //            * Base::noiseIntensity() * Base::tau ) * noise;
    }

    //xi = 0;

    //if (latticepoint == 1000) cout << endl;
    for (int i = 0; i < 3; ++i) {
        //blitz::TinyVector< double, Components::number_of_Variables > d = static_cast< T_model* > ( this ) -> step_d( values );

        //static_cast< T_model* > ( this ) -> step_d( values, d );

        Components df = static_cast< T_model* > ( this ) -> step_df( Components( values ) );
        Components dg = static_cast< T_model* > ( this ) -> step_dg( Components( values ) );

        //double du = static_cast<T_model*>(this) -> step_du( u, v ) ;
        //double dv = static_cast<T_model*>(this) -> step_dv( u, v );

        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            //    cout << "." << initial[ component ] << endl;
            //               std::cout << d[ component ] << std::endl;
            //    cout << xi[ component ] << endl;
            values[ component ] = initial[ component ] + 1.0 / 2.0 * Base::tau * df[ component ]
                + dg[ component ] * xi[ component ] / 2.0;
        }
        //    cout << "\n"<< values;
        //u = u_I + 1./2. * tau * du;
        //v = v_I + 1./2. * tau * dv + 1./2. * xsi;

        // performance increase if in fp
        //  if (abs(du) < 1E-7 && abs(dv) < 1E-7) {break;}

        //if ( CheckTininess< Components::number_of_Variables >::isTiny( dd ) )
        if ( df.isTiny() )
            break;
        /*
         if ( Components::number_of_Variables == 1 ) {
         if ( abs( d[ 0 ] ) < 1E-7 ) {
         break;
         }
         }
         if ( Components::number_of_Variables == 2 ) {
         if ( abs( d[ 0 ] ) < 1E-7 && abs( d[ 1 ] ) < 1E-7 ) {
         break;
         }
         }
         if ( Components::number_of_Variables == 3 ) {
         if ( abs( d[ 0 ] ) < 1E-7 && abs( d[ 1 ] ) < 1E-7
         && abs( d[ 2 ] ) < 1E-7 )
         {
         break;
         }
         }
         if ( Components::number_of_Variables == 4 ) {
         if ( abs( d[ 0 ] ) < 1E-7 && abs( d[ 1 ] ) < 1E-7
         && abs( d[ 2 ] ) < 1E-7 && abs( d[ 3 ] ) < 1E-7 )
         {
         break;
         }
         }
         */
        //if (latticepoint == 1000) cout << (u - u_I) << " " << flush;
    }
    Components df = static_cast< T_model* > ( this ) -> step_df( Components( values ) );
    Components dg = static_cast< T_model* > ( this ) -> step_dg( Components( values ) );

    //    double d[ Components::number_of_Variables ];
    //static_cast< T_model* > ( this ) -> step_d( values, d );

    //double du = static_cast<T_model*>(this) -> step_du(u, v );
    //double dv = static_cast<T_model*>(this) -> step_dv(u, v );

    //  u_lattice[latticepoint] = u_I + tau * du;
    //  v_lattice[latticepoint] = v_I + tau * dv + xsi;


    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::latticeDataPointer[ component ][ latticePoint ] = initial[ component ] + Base::tau
            * df[ component ] + dg[ component ] * xi[ component ];
    }

    /*  for (uint component = 0; component < Components::number_of_Variables; ++component) {

     static_cast< T_model* > ( this )->lattice[ component ].data()[ latticepoint ]
     = initial[ component ] + static_cast< T_model* > ( this )->tau * d[ component ]
     + xi[ component ];
     }*/
}
template<typename T_model>
inline typename Lattice< T_model>::Components LatticeIntegrator<T_model>::step_df(Components) {
    Components c;
    c.components = 1;
    return c;
}
template<typename T_model>
inline typename Lattice< T_model>::Components LatticeIntegrator<T_model>::step_dg(Components) {
    Components c;
    c.components = 1;
    return c;
}

#include "lattice_integrator.cpp"

#endif /* LATTICE_INTEGRATOR_H_ */
