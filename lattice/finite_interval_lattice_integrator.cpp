/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * finite_interval_lattice_integrator.cpp
 *
 *  Created on: 13.04.2009
 *      Author: rikebs
 */

#ifndef FINITE_INTERVAL_LATTICE_INTEGRATOR_CPP_
#define FINITE_INTERVAL_LATTICE_INTEGRATOR_CPP_

#include "finite_interval_lattice_integrator.h"

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
