/*
 * fhn_lattice.h
 *
 *  Created on: 21.12.2008
 *      Author: rikebs
 */

#ifndef FHN_LATTICE_H_
#define FHN_LATTICE_H_

#include "../lattice/rds_lattice.h"

/**
 * \class FhnLattice
 * Implementierung eines FitzHugh-Nagumo-Modells mit senkrechter Inhibitor-Nullkline
 *
 * The smallest funtions are to be inlined, so we mention them in the header file.
 */
class FhnLattice;
/**
 * Helper Class for additional but dependend Information
 */
template<>
struct Metainfo< FhnLattice > : MetainfoBase {
    typedef TwoComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template<int N> struct NoiseMapping {
        enum {
            value = -1
        };
    };
    static const int SIIP_ITERATIONS = 3;
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template<> struct Metainfo< FhnLattice >::NoiseMapping< secondComponent > {
    enum {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnLattice : public SIIP_LatticeIntegrator< FhnLattice > {
public:
    typedef TwoComponentSystem TestComponents;
    // TestComponents C;
    FhnLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);

    Parameter< double > epsilon;
    Parameter< double > gamma;

    Parameter< double > gamma_high, gamma_low, gamma_spacing, waveSize_threshold;

    inline double fixpointU() const
    {
        return -gamma.get() - 1.;
    }
    inline double fixpointV() const
    {
        double u0 = fixpointU();
        return 3. * u0 - u0 * u0 * u0;
    }
    inline double step_u(double u, double v)
    {
        //return u - u*u*u - v;
        return (1.0 / epsilon.get()) * (3.0 * u - u * u * u - v);
    }
    inline double step_v(double u, double v)
    {
        //return epsilon()* ( u - 2.0 *v - gamma() );
        return gamma.get() + 1. + u;
    }

    inline TwoComponentSystem step_f(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( step_u( sys.u(), sys.v() ), step_v( sys.u(), sys.v() ) );
    }
    /*inline TwoComponentSystem step_g(TwoComponentSystem sys, long int)
     {
     return TwoComponentSystem( sys.u(), 0 );
     }*/
    Parameter< double > var_intensity, var_size, var_speed;
    inline TwoComponentSystem step_h(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( var_intensity(), 0 );
    }
    inline TwoComponentSystem external_force(long int position)
    {
        SurfacePoint now = indexToSurfacePoint( position );
        SurfacePoint sp = centrePoint();//{ 10., 10. };
        if ( euklidianDistance( now, sp ) < var_size() ) {
            return TwoComponentSystem( sin( time() * var_speed() ), 0 );
        } else
            return TwoComponentSystem( 0, 0 );
    }

    /*inline TwoComponentSystem step_h2(TwoComponentSystem sys, long int) {
     //return TwoComponentSystem(0,1);
     return TwoComponentSystem(0.6,0);
     }
     inline TwoComponentSystem external_force(long int position) {
     //        return  (this->indexToX(position) > 50 ) ? TwoComponentSystem( 0, sin(position / 5.0) ) : TwoComponentSystem(0,0);
     int x = indexToX(position);
     int y = indexToY(position);
     double rad = sqrt(x*x + y*y);
     TwoComponentSystem ret;
     ret.components = sin(rad / 2.0 + time() * 3);
     return  (this->indexToX(position) > 50 ) ? ret : TwoComponentSystem(0,0);
     }
     */
    /*    inline void step_d(const double* in, double* out)
     {
     out[ 0 ] = step_du( in[ 0 ], in[ 1 ] );
     out[ 1 ] = step_dv( in[ 0 ], in[ 1 ] );
     }
     *//*
     inline blitz::TinyVector< double, number_of_Variables > step_d(blitz::TinyVector< double,
     number_of_Variables > sys)
     {
     return blitz::TinyVector< double, number_of_Variables >(
     step_du( sys[ 0 ], sys[ 1 ] ), step_dv( sys[ 0 ], sys[ 1 ] ) );
     }
     */
    //inline void toInitial()
    //{
    /*        blitz::firstIndex i;
     blitz::secondIndex j;
     lattice[ 0 ] = fixpointU() + sin(i * M_PI * 20.0 / latticeSizeX() ); //where( i > latticeSizeX()/2 , 2 , fixpointU() );
     lattice[ 1 ] = fixpointV() + sin(j * M_PI * 20.0 / latticeSizeY() );
     */
    //}

    inline TwoComponentSystem fixpoint(int) const
    {
        return TwoComponentSystem( fixpointU(), fixpointV() );
    }

    double suggestedTimeStep() const;

    void adaptParameters();

    void toInitial(int num)
    {

        if ( num == 2 ) {
            setGeometry(0,0,128,128);

        }
        status( std::cout );
        toFixpoint();
        return;
        if ( num == 2 ) {
            removeDefects();

            Defect< TwoComponentSystem > d;
            d.boundaryCondition = NoReactionBoundary;
            SurfacePoint centre;
            centre.x = sizeX() / 2.0;
            centre.y = sizeY() / 2.0;
            d.centre = centre;
            d.radius = 1.5 * sizeX() / 2.0;
            addDefect( d );

            d.boundaryCondition = PeriodicBoundary;
            d.radius = 11.0;
            for (int i = 0; i < 12; ++i) {
                d.centre.x = 0.4 * sizeX() * sin( i * 2.0 * M_PI / 12.0 ) + sizeX() / 2.0;
                d.centre.y = 0.4 * sizeX() * cos( i * 2.0 * M_PI / 12.0 ) + sizeY() / 2.0;
                addDefect( d );
            }

            setSpotAt(
                sizeX() / 2.0, 0.4 * sizeY() + sizeY() / 2.0, 2.0, TwoComponentSystem( 1, 0 ), true );
            setSpotAt(
                0.4 * sizeX() * sin( 11 * 2.0 * M_PI / 12.0 ) + sizeX() / 2.0, 0.4 * sizeX() * cos(
                    11 * 2.0 * M_PI / 12.0 ) + sizeY() / 2.0, 11.0, TwoComponentSystem( -2, 2 ),
                true );
        }
        if ( num == 3 ) {
            blitz::firstIndex i;
            blitz::secondIndex j;
            lattice[ 0 ] = fixpointU() + sin( i * M_PI * 20.0 / latticeSizeX() ); //where( i > latticeSizeX()/2 , 2 , fixpointU() );
            lattice[ 1 ] = fixpointV() + sin( j * M_PI * 20.0 / latticeSizeY() );
        }

        if ( num == 4 ) {
            removeDefects();

            setSpotAtComponent( 10., 10., 2., 2., 0, true );

            Defect< TwoComponentSystem > d;
            d.boundaryCondition = NoReactionBoundary;

            for (int i = 0; i < 3; ++i) {
                for (int j = 0; j < 3; ++j) {
                    double x = (i * latticeSizeX() / 3);
                    double y = (j * latticeSizeY() / 3);

                    d.centre.x = x * cos( 30. * 2. * M_PI / 360. ) - y * sin( 30. * 2. * M_PI
                        / 360. );
                    d.centre.y = x * sin( 30. * 2. * M_PI / 360. ) + y * cos( 30. * 2. * M_PI
                        / 360. );
                    d.radius = var_size();
                    addDefect( d );
                }
            }
        }
        if ( num == 5 ) {
            toFixpoint();
            removeDefects();
            setSpotAtComponent( sizeX() / 2., sizeY() / 2., sizeX() / 2., 2., 0, true );
            return;
        }
    }
};

class GameOfLife;
/**
 * Helper Class for additional but dependend Information
 */
template<>
struct Metainfo< GameOfLife > : MetainfoBase {
    typedef OneComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template<int N> struct NoiseMapping {
        enum {
            value = -1
        };
    };
};
template<> struct Metainfo< GameOfLife >::NoiseMapping< firstComponent > {
    enum {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class GameOfLife : public Lattice< GameOfLife > {
public:
    Parameter< double > delay;
    enum Rule {
        BearsCell, KillsCell, IgnoresCell
    };
    GameOfLife(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
        Lattice< GameOfLife > ( sizeX, sizeY, latticeSizeX, latticeSizeY ), delay( 0.01, "delay" )
    {
        registerParameter( &delay );
    }
    void step()
    {
        blitz::TinyVector< Rule, 10 > rules;
        rules = KillsCell;
        rules( 2 ) = IgnoresCell;
        rules( 3 ) = BearsCell;
        blitz::Array< Rule, 2 > newMap( lattice[ 0 ].shape() );
        for (int i = 0; i < latticeSize(); ++i) {
            newMap.data()[ i ] = newState( lattice[ 0 ], i, rules, true );
            //if ( indexToLatticePoint(i).x == 0 || indexToLatticePoint(i).y == 0)
            //    newMap.data()[i] = KillsCell;
        }
        for (int i = 0; i < latticeSize(); ++i) {
            if ( newMap.data()[ i ] == IgnoresCell ) {
                continue;
            }
            if ( newMap.data()[ i ] == BearsCell ) {
                lattice[ 0 ].data()[ i ] = 1.0;
                continue;
            }
            lattice[ 0 ].data()[ i ] = -1.0;

        }
        std::stringstream syscall;
        syscall << delay() << std::flush;
        std::string s2;
        syscall >> s2;
        std::string s1;
        s1 = "sleep ";

        system( (s1 + s2).c_str() );
    }
    inline void toInitial(int)
    {
        lattice[ 0 ] = -1;
        lattice[ 0 ]( 2, 3 ) = 1;
        lattice[ 0 ]( 2, 2 ) = 1;
        lattice[ 0 ]( 3, 3 ) = 1;
        lattice[ 0 ]( 3, 2 ) = 1;

        lattice[ 0 ]( 7, 7 ) = 1;
        lattice[ 0 ]( 7, 6 ) = 1;
        lattice[ 0 ]( 7, 5 ) = 1;
    }
    inline OneComponentSystem fixpoint(int) const
    {
        return OneComponentSystem( -1.0 );
    }
protected:
    inline Rule newState(blitz::Array< double, 2 >& a, int position,
                         blitz::TinyVector< Rule, 10 >& rules, bool periodic = true)
    {
        int count = 0;
        int pos;
        if ( periodic ) {
            LatticePoint p = indexToLatticePoint( position );
            pos = indexPeriodic( p.x + 1, p.y + 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            pos = indexPeriodic( p.x + 1, p.y );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            pos = indexPeriodic( p.x + 1, p.y - 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;

            pos = indexPeriodic( p.x, p.y + 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            //            pos = indexPeriodic( p.x, p.y );
            //            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            pos = indexPeriodic( p.x, p.y - 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;

            pos = indexPeriodic( p.x - 1, p.y + 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            pos = indexPeriodic( p.x - 1, p.y );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
            pos = indexPeriodic( p.x - 1, p.y - 1 );
            count += a( indexToX( pos ), indexToY( pos ) ) > 0 ? 1 : 0;
        }
        return rules[ count ];
    }
};

/**
 * \class FhnHeartLattice
 * Implementierung eines FitzHugh-Nagumo-Modells mit senkrechter Inhibitor-Nullkline
 *
 * The smallest funtions are to be inlined, so we mention them in the header file.
 */
class FhnHeartLattice;
/**
 * Helper Class for additional but dependend Information
 */
template<>
struct Metainfo< FhnHeartLattice > : MetainfoBase {
    typedef TwoComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template<int N> struct NoiseMapping {
        enum {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    //    const static bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template<> struct Metainfo< FhnHeartLattice >::NoiseMapping< secondComponent > {
    enum {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnHeartLattice : public SIIP_LatticeIntegrator< FhnHeartLattice > {
public:
    typedef TwoComponentSystem TestComponents;

    Parameter< double > epsilon;
    Parameter< double > gamma;

    Parameter< double > gamma_high, gamma_low, gamma_spacing, waveSize_threshold;

    inline double fixpointU() const
    {
        return -gamma.get() - 1.;
    }
    inline double fixpointV() const
    {
        double u0 = fixpointU();
        return 3. * u0 - u0 * u0 * u0;
    }
    inline double step_u(double u, double v)
    {
        return (1.0 / epsilon.get()) * (3.0 * u - u * u * u - v);
    }
    inline double step_v(double u, double /*v*/)
    {
        return gamma.get() + 1. + u;
    }

    inline TwoComponentSystem step_f(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( step_u( sys.u(), sys.v() ), step_v( sys.u(), sys.v() ) );
    }
    Parameter< double > var_intensity, var_size, var_speed;

    inline TwoComponentSystem fixpoint(int) const
    {
        return TwoComponentSystem( fixpointU(), fixpointV() );
    }

    FhnHeartLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
        SIIP_LatticeIntegrator< FhnHeartLattice > ( sizeX, sizeY, latticeSizeX, latticeSizeY ),
            epsilon( 0.1, "epsilon", 0.0, 100.0 ), gamma( 0.1, "gamma", 0.0, 100.0 ), gamma_high(
                0.55, "γ high" ), gamma_low( 0.1, "γ low" ), gamma_spacing( 0.001, "γ spacing" ),
            waveSize_threshold( 200, "wavesize" ), var_intensity( 0, "intensity", 0, 100 ),
            var_size( 3, "size", 0, 100 ), var_speed( 0, "speed", 0, 100 )

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

    void toInitial(int num)
    {

        toFixpoint();
        removeDefects();
        double x = (sizeX() - 1) / 100.;
        double y = (sizeY() - 1) / 100.;
        if ( true ) {
            Defect< TwoComponentSystem > d;
            SurfacePoint s1 = { 46 * x, 100. * y };
            SurfacePoint s2 = { 46 * x, 93. * y };
            SurfacePoint s3 = { 10 * x, 24 * y };
            SurfacePoint s4 = { 0. * x, 24 * y };
            SurfacePoint s5 = { 0 * x, 100. * y };
            SurfacePoint s6 = { 46 * x, 100. * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.polygon.push_back( s5 );
            d.polygon.push_back( s6 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }
        if ( true ) {
            Defect< TwoComponentSystem > d;
            SurfacePoint s1 = { 54 * x, 100. * y };
            SurfacePoint s2 = { 54 * x, 93. * y };
            SurfacePoint s3 = { 90 * x, 24 * y };
            SurfacePoint s4 = { 100 * x, 24 * y };
            SurfacePoint s5 = { 100 * x, 100. * y };
            SurfacePoint s6 = { 54 * x, 100. * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.polygon.push_back( s5 );
            d.polygon.push_back( s6 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }

        if ( true ) {
            Defect< TwoComponentSystem > d;
            SurfacePoint s1 = { 50 * x, 78 * y };
            SurfacePoint s2 = { 26 * x, 24 * y };
            SurfacePoint s3 = { 74 * x, 24 * y };
            SurfacePoint s4 = { 50 * x, 78 * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }

        if ( true ) {
            Defect< TwoComponentSystem > d;
            SurfacePoint s1 = { 0 * x, 14 * y };
            SurfacePoint s2 = { 100 * x, 14 * y };
            SurfacePoint s3 = { 100 * x, 0 * y };
            SurfacePoint s4 = { 0 * x, 0 * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }
        if ( true && num == 0 ) {
            Defect< TwoComponentSystem > d;
            /*SurfacePoint s1 = { 58 * x, 57. * y };
             SurfacePoint s2 = { 70 * x, 64. * y };
             SurfacePoint s3 = { 70 * x, 50 * y };
             SurfacePoint s4 = { 58 * x, 57. * y };*/
            SurfacePoint s1 = { 61 * x, 52. * y };
            SurfacePoint s2 = { 73 * x, 59. * y };
            SurfacePoint s3 = { 73 * x, 45 * y };
            SurfacePoint s4 = { 61 * x, 52. * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }
        if ( true && num == 1 ) {
            Defect< TwoComponentSystem > d;
            /*SurfacePoint s1 = { 58 * x, 57. * y };
             SurfacePoint s2 = { 70 * x, 64. * y };
             SurfacePoint s3 = { 70 * x, 50 * y };
             SurfacePoint s4 = { 58 * x, 57. * y };*/
            SurfacePoint s1 = { 60 * x, 52. * y };
            SurfacePoint s2 = { 74 * x, 59. * y };
            SurfacePoint s3 = { 74 * x, 45 * y };
            SurfacePoint s4 = { 60 * x, 52. * y };
            d.polygon.push_back( s1 );
            d.polygon.push_back( s2 );
            d.polygon.push_back( s3 );
            d.polygon.push_back( s4 );
            d.boundaryCondition = NoReactionBoundary;
            addDefect( d );
        }
        setSpotAtComponent( 97 * x, 20 * y, 3, 1.0, 0, true );

        /*        removeDefects();

         Defect< TwoComponentSystem > d;
         d.boundaryCondition = NoReactionBoundary;
         SurfacePoint centre;
         centre.x = sizeX() / 2.0;
         centre.y = sizeY() / 2.0;
         d.centre = centre;
         d.radius = 1.5 * sizeX() / 2.0;
         addDefect( d );

         d.boundaryCondition = PeriodicBoundary;
         d.radius = 11.0;
         for (int i=0; i<12; ++i) {
         d.centre.x = 0.4 * sizeX() * sin( i * 2.0 * M_PI / 12.0 ) + sizeX() / 2.0;
         d.centre.y = 0.4 * sizeX() * cos( i * 2.0 * M_PI / 12.0 ) + sizeY() / 2.0;
         addDefect( d );
         }

         setSpotAt( sizeX() / 2.0, 0.4*sizeY() + sizeY() / 2.0, 2.0, TwoComponentSystem(1,0), true );
         setSpotAt(0.4 * sizeX() * sin( 11 * 2.0 * M_PI / 12.0 ) + sizeX() / 2.0, 0.4 * sizeX() * cos( 11 * 2.0 * M_PI / 12.0 ) + sizeY() / 2.0, 11.0, TwoComponentSystem(-2,2), true );
         */
    }

    inline TwoComponentSystem step_h(TwoComponentSystem sys, long int)
    {
        return TwoComponentSystem( var_intensity(), 0 );
    }
    inline TwoComponentSystem external_force(long int position)
    {
        SurfacePoint now = indexToSurfacePoint( position );
        SurfacePoint sp = { 0.97 * sizeX(), 0.2 * sizeY() };
        if ( euklidianDistance( now, sp ) < var_size() ) {
            return TwoComponentSystem( sin( time() * var_speed() ), 0 );
        } else
            return TwoComponentSystem( 0, 0 );
    }
};


#endif /* FHN_LATTICE_H_ */
