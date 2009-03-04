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
void SIIP_LatticeIntegrator< T_model >::step_dtProxy(
                                            SIIP_LatticeIntegrator< T_model >* lattice,
                                            int loopNum, int loopCount)
{
    int start = lattice->latticeSize() * (loopNum - 1) / loopCount;
    int end = lattice->latticeSize() * loopNum / loopCount;
    for (int i = start; i < end; i++) {
        //if ( Base::latticePointHasReaction_.data()[ i ] )
        // Todo: Reaktionspunkt irgendwie auslagern
        lattice->step_dt( i );
    }
}

/**
 *
 */
template<typename T_model>
void SIIP_LatticeIntegrator< T_model >::integration(
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
                step_dt( i );
        }
#else
        boost::thread_group tgr;
        for (int i=0; i< BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &step_dtProxy, this, i+1, BOOST_THREADS) );
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
                step_dt( i );
        }
#else
        boost::thread_group tgr;
        for (int i=0; i < BOOST_THREADS; ++i) {
            tgr.create_thread( boost::bind( &step_dtProxy, this, i+1, BOOST_THREADS) );
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
void SIIP_LatticeIntegrator< T_model >::step(int numberOfSteps /* = 1 */)
{
    if ( numberOfSteps > 1 ) {
        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::HalfStep, Base::IgnoreStep );
        static_cast< T_model* > (this) -> doAfterStep();

        for (int i = 2; i < numberOfSteps; ++i) {
            static_cast< T_model* > (this) -> doBeforeStep();
            integration( Base::WholeStep, Base::IgnoreStep );
            static_cast< T_model* > (this) -> doAfterStep();
        }

        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::WholeStep, Base::HalfStep );
        static_cast< T_model* > (this) -> doAfterStep();

    } else {
        static_cast< T_model* > (this) -> doBeforeStep();
        integration( Base::HalfStep, Base::HalfStep );
        static_cast< T_model* > (this) -> doAfterStep();
    }

    // Zeit time wird in diffusion() erhöht.
    Base::numSteps_ += numberOfSteps;
}


/*
 * Testweise für Finite-Intervall-Diffusion eingebaut.
 *
 */


template<typename T_model>
inline double FiniteIntervalIntegrator< T_model >::my_laplacian(blitz::Array< double, 2 >& a,
                                                                int x, int y)
{
    //return - 4 * a( x, y ) + a( x - 1, y ) + a( x + 1, y ) + a( x, y - 1 )
    //    + a( x, y + 1 );

    return -20.0 * a( x, y ) + 4.0
        * (a( x - 1, y ) + a( x + 1, y ) + a( x, y - 1 ) + a( x, y + 1 )) + a( x + 1, y + 1 ) + a(
        x - 1, y + 1 ) + a( x + 1, y - 1 ) + a( x - 1, y - 1 );
}

template<typename T_model>
inline double FiniteIntervalIntegrator<T_model >::my_laplacian_periodic(blitz::Array< double, 2 >& a, int x, int y)
{
    //return - 4 * a( x, y ) + a( x - 1, y ) + a( x + 1, y ) + a( x, y - 1 )
    //    + a( x, y + 1 );
    return -20.0 * a( Base::indexToX( Base::indexPeriodic( x, y ) ), Base::indexToY( Base::indexPeriodic( x, y ) ) ) + 4.0
        * (a( Base::indexToX( Base::indexPeriodic( x - 1, y ) ), Base::indexToY( Base::indexPeriodic( x - 1, y ) ) ) + a(
            Base::indexToX( Base::indexPeriodic( x + 1, y ) ), Base::indexToY( Base::indexPeriodic( x + 1, y ) ) ) + a(
            Base::indexToX( Base::indexPeriodic( x, y - 1 ) ), Base::indexToY( Base::indexPeriodic( x, y - 1 ) ) ) + a(
            Base::indexToX( Base::indexPeriodic( x, y + 1 ) ), Base::indexToY( Base::indexPeriodic( x, y + 1 ) ) ))
            + 1.0 * ( a(
        Base::indexToX( Base::indexPeriodic( x + 1, y + 1 ) ), Base::indexToY( Base::indexPeriodic( x + 1, y + 1 ) ) ) + a(
        Base::indexToX( Base::indexPeriodic( x - 1, y + 1 ) ), Base::indexToY( Base::indexPeriodic( x - 1, y + 1 ) ) ) + a(
        Base::indexToX( Base::indexPeriodic( x + 1, y - 1 ) ), Base::indexToY( Base::indexPeriodic( x + 1, y - 1 ) ) ) + a(
        Base::indexToX( Base::indexPeriodic( x - 1, y - 1 ) ), Base::indexToY( Base::indexPeriodic( x - 1, y - 1 ) ) ) );
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
                                                    typename Base::DiffusionStepWidth stepSize, bool advanceInTime)
{
    //std::cout << "Finite Intervall";
    advanceTime( stepSize );

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
        blitz::Array< double, 2 > diffMatrix( Base::lattice[ component ].shape() );
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                if ( x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y
                    == (Base::latticeSizeY() - 1) )
                {

                    if ( Base::boundaryCondition_ == PeriodicBoundary ) {
                        diffMatrix( x, y ) += my_laplacian_periodic(
                            Base::lattice[ component ], x, y ) * scaleFactor;
                    }
                } else {

                    //      if ( abs( Base::lattice[ component ]( x, y ) - Base::fixpoint()[component] ) < 0.1 )
                    //            return;
                    //double diffScale = sqrt( pow( ( 2.0 * x / Base::latticeSizeX() - 1 ), 2 ) + pow( ( 2.0 * y / Base::latticeSizeY() - 1 ), 2 ) );
                    diffMatrix( x, y ) += my_laplacian( Base::lattice[ component ], x, y )
                        * scaleFactor;

                    /*           scaleFactor = scaleFactor * Base::lattice[ component ]( x, y );


                     diffMatrix( x, y ) += (-20.0 * scaleFactor);
                     diffMatrix( x - 1, y ) += 4.0 * scaleFactor;
                     diffMatrix( x + 1, y ) += 4.0 * scaleFactor;
                     diffMatrix( x, y - 1 ) += 4.0 * scaleFactor;
                     diffMatrix( x, y + 1 ) += 4.0 * scaleFactor;
                     diffMatrix( x + 1, y + 1 ) += scaleFactor;
                     diffMatrix( x - 1, y + 1 ) += scaleFactor;
                     diffMatrix( x + 1, y - 1 ) += scaleFactor;
                     diffMatrix( x - 1, y - 1 ) += scaleFactor;
                     */}
            }
        }

        Base::lattice[ component ] += diffMatrix;
    }
}

#endif
