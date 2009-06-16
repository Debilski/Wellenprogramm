/*
 * ginzburg_landau_lattice.cpp
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#include "ginzburg_landau_lattice.h"

GinzburgLandauLattice::GinzburgLandauLattice() : eta(
        0.1, "eta", -100.0, 100.0 ), etaB( 0.1, "eta I", -100.0, 100.0 ), r(
	        0.1, "r", -100.0, 100.0 ), rB( 0.1, "r I", -100.0, 100.0 ), s(
		        0.1, "s", -100.0, 100.0 ), sB( 0.1, "s I", -100.0, 100.0 )

{
    componentInfos[ 0 ] = ComponentInfo( "A reell", "A", -2.2, 2.5 ); // Einheit und Dimension klingt gut
    componentInfos[ 1 ] = ComponentInfo( "A imag", "AI", -2.2, 2.5 );
    componentInfos[ 2 ] = ComponentInfo( "B reell", "B", -2.2, 2.5 );
    componentInfos[ 3 ] = ComponentInfo( "B imag", "BI", -2.2, 2.5 );

    registerParameter( &eta );
    registerParameter( &etaB );
    registerParameter( &r );
    registerParameter( &rB );
    registerParameter( &s );
    registerParameter( &sB );
}

DEFINE_LATTICE_DRIVER2( "GinzburgLandauLattice", GinzburgLandauLattice )

REGISTER_PLUGINS_BEGIN
REGISTER_PLUGIN(GinzburgLandauLattice)
REGISTER_PLUGINS_END
