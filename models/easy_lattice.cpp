/*
 * easy_lattice.cpp
 *
 *  Created on: 02.01.2009
 *      Author: rikebs
 */

#include "rds_lattice.h"




typedef ComponentSystem< 2 > TR;


#define EASY_LATTICE_NAME easy_lattice2
#define EASY_LATTICE_COMPONENTS TR
#define EASY_LATTICE_PARAMETER epsilon, delta
//#define EASY_LATTICE_REACTION ( -epsilon()*MODEL.u() , -MODEL.u() * MODEL.u() * MODEL.u() )
#define EASY_LATTICE_REACTION ( )//( -MODEL.u() * MODEL.u() * MODEL.u() + MODEL.v() ) *1.0/epsilon(), 1.1-MODEL.u() )
#define EASY_LATTICE_FIXPOINT ( )//0,0 )
#include "../lattice/easy_lattice_model.h"
EASY_LATTICE_MODEL_END

/*


TWO_COMPONENT_SYSTEM(TS, u, v)

#define EASY_LATTICE_NAME easy_lattice1
#define EASY_LATTICE_COMPONENTS TS
#define EASY_LATTICE_PARAMETER epsilon, delta
//#define EASY_LATTICE_REACTION ( -epsilon()*MODEL.u() , -MODEL.u() * MODEL.u() * MODEL.u() )
#define EASY_LATTICE_REACTION ( ( -MODEL.u() * MODEL.u() * MODEL.u() + MODEL.v() ) *1.0/epsilon(), 1.1-MODEL.u() )
#define EASY_LATTICE_FIXPOINT ( 0,0 )
#include "../lattice/easy_lattice_model.h"
EASY_LATTICE_MODEL_END




#define EASY_LATTICE_NAME easy_lattice
#define EASY_LATTICE_COMPONENTS TwoComponentSystem
#define EASY_LATTICE_PARAMETER epsilon, delta
//#define EASY_LATTICE_REACTION ( -epsilon()*MODEL.u() , -MODEL.u() * MODEL.u() * MODEL.u() )
#define EASY_LATTICE_REACTION ( ( -MODEL.u() * MODEL.u() * MODEL.u() + MODEL.v() ) *1.0/epsilon(), 1.1-MODEL.u() )
#define EASY_LATTICE_FIXPOINT ( 0,0 )



#include "../lattice/easy_lattice_model.h"



EASY_LATTICE_MODEL_END

EASY_LATTICE_NOISE_MAPPING(firstComponent, secondComponent)

LATTICE_REGISTER_MODEL2( "easy_lattice", easy_lattice )


*/
