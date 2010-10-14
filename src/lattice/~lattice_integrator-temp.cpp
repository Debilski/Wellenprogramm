/*
 * lattice_integrator.cpp
 *
 *  Created on: 21.11.2008
 *      Author: rikebs
 */

#ifndef LATTICE_INTEGRATOR_CPP
#define LATTICE_INTEGRATOR_CPP

#include "lattice_integrator.h"

/**
 * Static proxy method for the reaction.
 *
 * \c detProxy splits the loop into \a loopCount parts and then calls \c det on the part indexed with \a loopNum.
 */
template<typename T_model>
void LatticeIntegrator< T_model >::detProxy(
                                            LatticeIntegrator< T_model >* lattice,
                                            int loopNum, int loopCount)
{
    int start = lattice->latticeSize() * (loopNum - 1) / loopCount;
    int end = lattice->latticeSize() * loopNum / loopCount;
    for (int i = start; i < end; i++) {
        lattice->det( i );
    }
}

/**
 *
 */
template<typename T_model>
void LatticeIntegrator< T_model >::integration(
                                               typename Lattice< T_model >::DiffusionStepWidth firstStep,
                                               typename Lattice< T_model >::DiffusionStepWidth lastStep)
{
    if ( Base::noiseIntensity() != 0 ) {

        boost::thread noiseThread( boost::bind(
            &Base::precomputeNoise, this ) );
        Base::setBoundary();
        Base::applyDefects();
        executeDiffusion( firstStep );
        noiseThread.join();

        for (uint component = 0; component
            < Components::number_of_Variables; ++component)
        {
            Base::latticeDataPointer[ component ]
                = Base::lattice[ component ].data();
        }

#ifndef BOOST_THREADS
# pragma omp parallel for
        for (int i = 0; i < Base::latticeSize(); ++i) {
            if ( Base::latticePointHasReaction_.data()[ i ] )
                det( i );
        }

#else
        boost::thread_group tgr;
        for (int i=0; i< BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &detProxy, this, i+1, BOOST_THREADS) );
        }
        tgr.join_all();
#endif

    } else {
        Base::setBoundary();
        Base::applyDefects();
        executeDiffusion( firstStep );

        for (uint component = 0; component
            < Components::number_of_Variables; ++component)
        {
            Base::latticeDataPointer[ component ]
                = Base::lattice[ component ].data();
        }

#ifndef BOOST_THREADS
# pragma omp parallel for
        for (int i = 0; i < Base::latticeSize(); ++i) {
            if ( Base::latticePointHasReaction_.data()[ i ] )
                det( i );
        }
#else
        boost::thread_group tgr;
        for (int i=0; i < BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &detProxy, this, i+1, BOOST_THREADS) );
        }
        tgr.join_all();
#endif
    }
    if ( lastStep != Base::IgnoreStep ) {
        Base::setBoundary();
        Base::applyDefects();
        executeDiffusion( lastStep );
    }
}

/**
 * Integrates the Lattice for \a numberOfSteps steps. If \f$\text{numberOfSteps}\neq 1 \f$, for example this spares
 * the FFT back calculation.
 */
template<typename T_model>
void LatticeIntegrator< T_model >::step(int numberOfSteps /* = 1 */)
{
    if ( numberOfSteps > 1 ) {

        integration( Base::HalfStep, Base::IgnoreStep );

        for (int i = 2; i < numberOfSteps; ++i) {
            integration( Base::WholeStep, Base::IgnoreStep );
        }

        integration( Base::WholeStep, Base::HalfStep );

    } else {
        integration( Base::HalfStep, Base::HalfStep );
    }
    // integration();
    // Zeit time wird in diffusion() erhöht.
    Base::numSteps_ += numberOfSteps;

}


/*
 * Testweise für Finite-Intervall-Diffusion eingebaut.
 *
 */


#include <blitz/array.h>
inline double my_laplacian(blitz::Array< double, 2 >& a, int x, int y)
{
    //return - 4 * a( x, y ) + a( x - 1, y ) + a( x + 1, y ) + a( x, y - 1 )
    //    + a( x, y + 1 );

    return -20.0 * a( x, y ) + 4.0 * (a( x - 1, y ) + a( x + 1, y ) + a(
        x, y - 1 ) + a( x, y + 1 )) + a( x + 1, y + 1 ) + a( x - 1, y + 1 )
        + a( x + 1, y - 1 ) + a( x - 1, y - 1 );
}

/*
 return (sigmax == 0) ? 0 : sigmax / (h * h) *
 ((lattice[index(x + 1, y + 1)].getX() +
 lattice[index(x + 1, y - 1)].getX() +
 lattice[index(x - 1, y + 1)].getX() +
 lattice[index(x - 1, y - 1)].getX()) +
 4. * (lattice[index(x, y + 1)].getX() +
 lattice[index(x, y - 1)].getX() +
 lattice[index(x + 1, y)].getX() +
 lattice[index(x - 1, y)].getX()) -
 20. * lattice[index(x, y)].getX());
 */
/**
 * Führt die Diffusion um eine Schrittweite von \a stepSize durch. Außerdem wird die Zeit
 *  \stepSize entsprechend verändert.
 */
template<typename T_model>
void FiniteIntervalIntegrator<T_model >::executeDiffusion(
                                                    typename Base::DiffusionStepWidth stepSize)
{
    std::cout << "Finite Intervall";
    Base::advanceTime( stepSize );

    for (uint component = 0; component < Components::number_of_Variables; ++component)
    {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;

        //diffMatrix = blitz::Laplacian2D4n( Base::lattice[ component ] );
        double scaleFactor = 0;

        if ( stepSize == Base::HalfStep ) {
            scaleFactor = Base::diffusion_[ component ] / Base::scaleX()
                / Base::scaleY() * Base::tau / 2.;
        } else if ( stepSize == Base::WholeStep ) {
            scaleFactor = Base::diffusion_[ component ] / Base::scaleX()
                / Base::scaleY() * Base::tau;

        }
        for (int x = 0 + 1; x < Base::latticeSizeX() - 1; ++x) {
            for (int y = 0 + 1; y < Base::latticeSizeY() - 1; ++y) {
                Base::lattice[ component ]( x, y ) += my_laplacian(
                    Base::lattice[ component ], x, y ) * scaleFactor;
            }
        }
    }
}

#endif
