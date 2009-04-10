/*
 * fhn_lattice.cpp
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#include "fhn_lattice.h"



FhnLattice::FhnLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
    SIIP_LatticeIntegrator< FhnLattice > ( sizeX, sizeY, latticeSizeX, latticeSizeY ), epsilon(
        0.1, "epsilon", 0.0, 100.0 ), gamma( 0.1, "gamma", -10.0, 100.0 ),
        gamma_high(0.55, "γ high"),
        gamma_low(0.1, "γ low"),
        gamma_spacing(0.001, "γ spacing"),
        waveSize_threshold(200, "wavesize"),
        var_intensity(2,"intensity",0,100), var_size(3, "size", 0,100), var_speed(1,"speed",0,100)

{
    componentInfos[ 0 ] = ComponentInfo( "Aktivator", "u", -2.2, 2.5 ); // Einheit und Dimension klingt gut
    componentInfos[ 1 ] = ComponentInfo( "Inhibitor", "v", -2.2, 2.5 );

    modelName_ = "FitzHugh-Nagumo";

    registerParameter( &epsilon );
    registerParameter( &gamma );

    registerParameter( &var_intensity );
    registerParameter( &var_size );
    registerParameter( &var_speed );

    registerAdaptationParameter( &gamma_high );
    registerAdaptationParameter( &gamma_low );
    registerAdaptationParameter( &gamma_spacing );
    registerAdaptationParameter( &waveSize_threshold );

}

double FhnLattice::suggestedTimeStep() const
{
    double m = getMax().u();
    //   cout << m << endl;
    //   cout << epsilon() / (m * m) << endl;
    //   cout << "E" << pow( 10, floor( log( epsilon() / (m * m) )
    //           / log( 10 ) ) ) << endl;
    return std::min( 0.01, pow( 10, floor( log( epsilon() / (m * m) ) / log( 10 ) ) ) );
}


void FhnLattice::adaptParameters()
{
    static int j = 0;
    double ws = this->currentWavesize();
    if ( j % 10 == 0 ) {
        if ( ws > waveSize_threshold() ) {
            gamma_low() = gamma_low() + gamma_spacing() * (gamma_high() - gamma_low()) * timeStep();
        } else {
            gamma_high() = gamma_high() - gamma_spacing() * (gamma_high() - gamma_low()) * timeStep();
        }
        //    SurfacePoint po = lattice->centreOfExcitation();
        //    std::cout << "\nNew high: " << high << ", New low: " << low << ", Epsilon: " << lattice->epsilon() << " " << po.x <<"\n" << std::flush;
    }
    if ( ws < waveSize_threshold() ) {
        gamma.set( gamma_low() );
        // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
    } else {
        gamma.set( gamma_high() );
        // std::cout << " g set to " << Model::g << " " << std::endl << std::flush;
    }
    j++;

}

DEFINE_LATTICE_DRIVER2( "FhnLattice", FhnLattice )

DEFINE_LATTICE_DRIVER2( "FhnHeartLattice", FhnHeartLattice )

DEFINE_LATTICE_DRIVER2( "GameofLife", GameOfLife )

REGISTER_PLUGINS_BEGIN
REGISTER_PLUGIN(FhnLattice)
REGISTER_PLUGIN(GameOfLife)
REGISTER_PLUGIN(FhnHeartLattice)
REGISTER_PLUGINS_END
