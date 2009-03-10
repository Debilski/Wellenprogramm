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

//TODO Much refactoring is needed


template<int N>
struct _tvec {
    typedef blitz::TinyVector< double, N > TinyVectorWithZero;
};

template<>
struct _tvec< 0 > {
    typedef void TinyVectorWithZero;
};


/**
 * This class is intended to call the needed substeps in the integration process.
 */
template<typename T_model>
class LatticeIntegrator : public Lattice< T_model >{

};


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
class SIIP_LatticeIntegrator : public Lattice< T_model > {
protected:
    typedef Lattice< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables ? Base::number_of_Noise_Variables : 1> T_noiseVector;

    //typedef typename _tvec<Base::number_of_Noise_Variables>::TinyVectorWithZero T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;

public:

    SIIP_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    Lattice< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
        Base::INTEGRATOR_NAME = std::string( "Semi Implicit Interaction Picture" );
    }
    void step() { step(1); }
    void step(int numberOfSteps);
protected:
    static const int SIIP_ITERATIONS = Metainfo< T_model>::SIIP_ITERATIONS;

    void step_dt(long int latticePoint);
    //Components step_f(Components);

    Components step_explicit( Components sys, long int position, Components noise );

    Components step_f(Components components, long int latticePoint);
    Components step_g(Components components, long int latticePoint);
    Components step_h(Components components, long int latticePoint);
    Components external_force(long int latticePoint);

    static void step_dtProxy(
        SIIP_LatticeIntegrator< T_model>* lattice,
        int loopNum, int loopCount);
    void
    integration(
        typename Lattice< T_model>::DiffusionStepWidth firstStep,
        typename Lattice< T_model>::DiffusionStepWidth lastStep);
private:
    static const bool HAS_H = ! Metainfo< T_model>::OPTIMISE_NO_EXTERNAL_FORCE;
    static const bool HAS_G = ! Metainfo< T_model>::OPTIMISE_NO_MULTIPLICATIVE_NOISE;
    static const bool HAS_EXPLICIT = Metainfo< T_model >::HAS_EXPLICIT_STEP_FUNCTION;

    //    T_model* subModel;
};


template<typename T_model>
class FiniteIntervalIntegrator : public SIIP_LatticeIntegrator< T_model > {
    typedef SIIP_LatticeIntegrator< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
public:
    FiniteIntervalIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    SIIP_LatticeIntegrator< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
        Base::INTEGRATOR_NAME = std::string( "Semi Implicite Finite Intervall" );
    }
protected:
    void executeDiffusion(typename Base::DiffusionStepWidth stepSize, bool advanceInTime = true);
    inline double my_laplacian(blitz::Array< double, 2 >& a, int x, int y);
    inline double my_laplacian_periodic(blitz::Array< double, 2 >& a, int x, int y);
};

/**
 * Eine Hilfsklasse, welche es erlaubt, Schleifen zur Übersetzungszeit zu generieren.
 */
template<typename T_model, unsigned int N_steps>
struct LatticeIntegratorLoop {
    typedef blitz::TinyVector< double,
        T_model::number_of_Noise_Variables ? T_model::number_of_Noise_Variables : 1 > T_noiseVector;
    typedef blitz::TinyVector< double, T_model::number_of_Variables > T_componentsVector;

    /**
     * Anhand der Informationen im NoiseMapping-Template wird ein Komponentenvector aus dem Noisevector erzeugt.
     */
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
    // Work-Around, damit es bei Fehlen von Rauschen keine Fehlermeldungen gibt.
    typedef blitz::TinyVector< double,
        T_model::number_of_Noise_Variables ? T_model::number_of_Noise_Variables : 1 > T_noiseVector;
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
inline void SIIP_LatticeIntegrator< T_model >::step_dt(long int latticePoint)
{
    blitz::TinyVector< double, Components::number_of_Variables > initial;
    blitz::TinyVector< double, Components::number_of_Variables > values;

    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        initial[ component ] = Base::latticeDataPointer[ component ][ latticePoint ];
        values[ component ] = initial[ component ];
    }

    // noiseVector: xi = Delta W
    T_componentsVector xi;
    xi = 0;

    // noiseVector mapping
    double intensity = Base::noiseIntensity();
    if ( intensity != 0 ) {
        T_noiseVector noise;
        for (uint noiseComponents = 0; noiseComponents < Base::number_of_Noise_Variables; ++noiseComponents)
        {
            noise[ noiseComponents ]
                = Base::noiseGenerator_[ noiseComponents ]->getPrecomputedNoise( latticePoint );
        }

        double mult = sqrt( 2.0 * Base::noiseIntensity() * Base::tau );
        LatticeIntegratorLoop< T_model, Components::number_of_Variables >::noiseMapping(
            xi, noise, mult );
    }


    // Halbschritt-Iteration
    for (int i = 0; i < SIIP_ITERATIONS; ++i) {
        Components f = static_cast< T_model* > ( this ) -> step_f( Components( values ), latticePoint );
        Components g = HAS_G ? static_cast< T_model* > ( this ) -> step_g( Components( values ), latticePoint ) : Components();
        Components h = HAS_H ? static_cast< T_model* > ( this ) -> step_h( Components( values ), latticePoint ) : Components();
        Components force = HAS_H ? static_cast< T_model* > ( this ) -> external_force( latticePoint ) : Components();

        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            values[ component ] = initial[ component ] + 1.0 / 2.0 * Base::tau * f[ component ]
                + (HAS_G ? g[ component ] : 1 ) * xi[ component ] / 2.0 +
                ( HAS_H ? (1.0 / 2.0 * Base::tau * h[ component ] * force[component]) : 0 );
        }
        // Falls im FP: abkürzen
        if ( intensity == 0 && f.isTiny() )
            break;
    }

    // Prüfe, ob es eine explizit festgelegte Integrationsfunktion gibt und führe sie aus
    if (HAS_EXPLICIT) {
        initial = (static_cast< T_model* > ( this ) -> step_explicit( Components( initial ), latticePoint, Components( xi ) ) ).toTinyVector();
    }
    // Vollschritt
    Components f = static_cast< T_model* > ( this ) -> step_f( Components( values ), latticePoint );
    Components g = HAS_G ? static_cast< T_model* > ( this ) -> step_g( Components( values ), latticePoint ) : Components();
    Components h = HAS_H ? static_cast< T_model* > ( this ) -> step_h( Components( values ), latticePoint ) : Components();
    Components force = HAS_H ? static_cast< T_model* > ( this ) -> external_force( latticePoint ) : Components();

    /*    for (uint component = 0; component < Components::number_of_Variables; ++component) {
     Base::latticeDataPointer[ component ][ latticePoint ] = initial[ component ] + Base::tau
     * f[ component ] + g[ component ] * xi[ component ];
     }*/
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::latticeDataPointer[ component ][ latticePoint ] = initial[ component ] + Base::tau
            * f[ component ] + ( HAS_G ? g[ component ] : 1 ) * xi[ component ] + (HAS_H ? (Base::tau * h[ component ] * force[component]) : 0);
    }
}

/**
 * Beschreibt den Nicht-Berauschten Teil der Reaktionsfunktion
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_f( Components, long int) {
    Components c;
    c.components = 0;
    return c;
}
/*
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_f2(Components c, long int) {
    return static_cast< T_model* > ( this ) -> step_f( c );
}
*/
/**
 * Beschreibt den Multiplikator für das Rauschen
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_g( Components,long int) {
    Components c;
    c.components = 1;
    return c;
}

/**
 * Beschreibt den Multiplikator für eine externe Kraft
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_h(Components,long int) {
    Components c;
    c.components = 0;
    return c;
}

/**
 * Beschreibt eine externe Kraft
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::external_force(long int) {
    Components c;
    c.components = 0;
    return c;
}

/**
 * Führt den Zeitschritt direkt aus
 */
template<typename T_model>
inline typename Lattice< T_model>::Components SIIP_LatticeIntegrator<T_model>::step_explicit( Components sys, long int , Components  ) {
    return sys;
}

/**
 * Runge-Kutta-Integration im WW-Bild
 */

template<typename T_model>
class RK4IP_LatticeIntegrator : public Lattice< T_model > {
protected:
    typedef Lattice< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables ? Base::number_of_Noise_Variables : 1> T_noiseVector;

    //typedef typename _tvec<Base::number_of_Noise_Variables>::TinyVectorWithZero T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;

public:
    //    static int const SIIP_ITERATIONS = Metainfo< T_model >::SIIP_ITERATIONS;

    RK4IP_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    Lattice< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
        Base::INTEGRATOR_NAME = std::string( "4th Order Runge-Kutta Interaction Picture" );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[ component ].resize( latticeSizeX, latticeSizeY );
            lattice_I[ component ].resize( latticeSizeX, latticeSizeY );
            latticeFft_K[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );

            forwardPlan_K[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_K[ component ], latticeFft_K[ component ] );
            backwardPlan_K[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_K[ component ], lattice_K[ component ] );
        }
    }
    ~RK4IP_LatticeIntegrator() {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            fftw_destroy_plan( forwardPlan_K[ component ] );
            fftw_destroy_plan( backwardPlan_K[ component ] );
        }
    }
    void step(int numberOfSteps) {for(int i=0; i<numberOfSteps; ++i) step();}
    void step();
protected:
    void step_dt(long int latticePoint);
    //Components step_f(Components);
    //Components step_g(Components);

    blitz::Array< double, 2> lattice_K[ Components::number_of_Variables ];
    blitz::Array< double, 2> lattice_I[ Components::number_of_Variables ];
    blitz::Array< std::complex< double> , 2> latticeFft_K[ Components::number_of_Variables ];
    fftw_plan forwardPlan_K[ Components::number_of_Variables ];
    fftw_plan backwardPlan_K[ Components::number_of_Variables ];
};


template<typename T_model>
void RK4IP_LatticeIntegrator< T_model >::step()
{
    // 1. Calc. xi
    // 2. a_k = a
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice_K[ i ] = Base::lattice[ i ];
    }
    // 3. a = D(1/2)a
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;
        fftw_execute( Base::forwardPlan[ component ] );
        Base::latticeFft[ component ] = Base::latticeFft[ component ]
            * Base::precomputedDiffusionOperator[ component ];
        fftw_execute( Base::backwardPlan[ component ] );
    }
    // 4. a_I = a
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice_I[ i ] = Base::lattice[ i ];
    }
    // 5. a_k = h N a_k
    for(int i=0; i<Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[component].data()[i];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in, i );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }
    // 6. a_k = D(1/2)a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;
        fftw_execute( forwardPlan_K[ component ] );
        latticeFft_K[ component ] = latticeFft_K[ component ]
            * Base::precomputedDiffusionOperator[ component ];
        fftw_execute( backwardPlan_K[ component ] );
    }
    // 7. a = a + 1/6 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[ component ] = Base::lattice[ component ] + 1. / 6. * lattice_K[ component ];
    }
    // 8. time = time + tau/2
    Base::advanceTime( Base::HalfStep );
    // 9. a_k = a_I + 1/2 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[ component ] = lattice_I[ component ] + 1. / 2. * lattice_K[ component ];
    }
    // 10. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in, i );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }
    // 11. a = a + 1/3 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[ component ] = Base::lattice[ component ] + 1. / 3. * lattice_K[ component ];
    }
    // 12. a_K = a_i + 1/2 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[ component ] = lattice_I[ component ] + 1. / 2. * lattice_K[ component ];
    }
    // 13. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in, i );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }
    // 14. a = a + 1/3 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[ component ] = Base::lattice[ component ] + 1. / 3. * lattice_K[ component ];
    }
    // 15. time = time + tau/2
    Base::advanceTime( Base::HalfStep );
    // 16. a_K = a_i +  a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice_K[ component ] = lattice_I[ component ] + lattice_K[ component ];
    }
    // 17. = 6. a_k = D(1/2)a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;
        fftw_execute( forwardPlan_K[ component ] );
        latticeFft_K[ component ] = latticeFft_K[ component ]
            * Base::precomputedDiffusionOperator[ component ];
        fftw_execute( backwardPlan_K[ component ] );
    }
    // 18. = 5.
    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in, i );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }
    // 19. a = D(1/2)a
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;
        fftw_execute( Base::forwardPlan[ component ] );
        Base::latticeFft[ component ] = Base::latticeFft[ component ]
            * Base::precomputedDiffusionOperator[ component ];
        fftw_execute( Base::backwardPlan[ component ] );
    }
    // 20. =  7. a = a + 1/6 a_k
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        Base::lattice[ component ] = Base::lattice[ component ] + 1. / 6. * lattice_K[ component ];
    }


}



#define LOOP_COMPONENTS for (uint component = 0; component < Components::number_of_Variables; ++component)
#define LOOP_COMPONENTS_ARG( component ) for (uint component = 0; component < Components::number_of_Variables; ++component)


/**
 * Adaptive Runge-Kutta-Integration im WW-Bild
 */

template<typename T_model>
class ARK45IP_LatticeIntegrator : public Lattice< T_model > {
protected:
    typedef Lattice< T_model > Base;
    //typedef typename M::TestComponents TestComponents_;

typedef    typename Base::Components Components;
    typedef blitz::TinyVector<double, Base::number_of_Noise_Variables ? Base::number_of_Noise_Variables : 1> T_noiseVector;

    //typedef typename _tvec<Base::number_of_Noise_Variables>::TinyVectorWithZero T_noiseVector;
    typedef blitz::TinyVector<double, Base::number_of_Variables> T_componentsVector;

public:
    //    static int const SIIP_ITERATIONS = Metainfo< T_model >::SIIP_ITERATIONS;

    ARK45IP_LatticeIntegrator(int sizeX, int sizeY, int latticeSizeX,
        int latticeSizeY) :
    Lattice< T_model> (
        sizeX, sizeY, latticeSizeX, latticeSizeY )
    {
        Base::INTEGRATOR_NAME = std::string( "Adaptive 4/5th Order Runge-Kutta Interaction Picture" );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_I[ component ].resize( latticeSizeX, latticeSizeY );
            lattice_J[ component ].resize( latticeSizeX, latticeSizeY );
            lattice_K[ component ].resize( latticeSizeX, latticeSizeY );
            lattice_L[ component ].resize( latticeSizeX, latticeSizeY );
            lattice_star[ component ].resize( latticeSizeX, latticeSizeY );

            latticeFft_I[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );
            latticeFft_J[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );
            latticeFft_K[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );
            latticeFft_L[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );
            latticeFft_star[ component ].resize( latticeSizeX, latticeSizeY / 2 + 1 );

            forwardPlan_I[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_I[ component ], latticeFft_I[ component ] );
            backwardPlan_I[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_I[ component ], lattice_I[ component ] );
            forwardPlan_J[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_J[ component ], latticeFft_J[ component ] );
            backwardPlan_J[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_J[ component ], lattice_J[ component ] );
            forwardPlan_K[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_K[ component ], latticeFft_K[ component ] );
            backwardPlan_K[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_K[ component ], lattice_K[ component ] );
            forwardPlan_L[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_L[ component ], latticeFft_L[ component ] );
            backwardPlan_L[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_L[ component ], lattice_L[ component ] );
            forwardPlan_star[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
                lattice_star[ component ], latticeFft_star[ component ] );
            backwardPlan_star[ component ] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
                latticeFft_star[ component ], lattice_star[ component ] );
        }
    }
    ~ARK45IP_LatticeIntegrator() {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            fftw_destroy_plan( forwardPlan_K[ component ] );
            fftw_destroy_plan( backwardPlan_K[ component ] );
            fftw_destroy_plan( forwardPlan_L[ component ] );
            fftw_destroy_plan( backwardPlan_L[ component ] );
            fftw_destroy_plan( forwardPlan_I[ component ] );
            fftw_destroy_plan( backwardPlan_I[ component ] );
            fftw_destroy_plan( forwardPlan_J[ component ] );
            fftw_destroy_plan( backwardPlan_J[ component ] );
            fftw_destroy_plan( forwardPlan_star[ component ] );
            fftw_destroy_plan( backwardPlan_star[ component ] );
        }
    }
    void step(int numberOfSteps) {for(int i=0; i<numberOfSteps; ++i) step();}
    void step();
protected:
    void step_dt(long int latticePoint);
    //Components step_f(Components);
    //Components step_g(Components);


    blitz::Array< double, 2> lattice_I[ Components::number_of_Variables ];
    blitz::Array< double, 2> lattice_J[ Components::number_of_Variables ];
    blitz::Array< double, 2> lattice_K[ Components::number_of_Variables ];
    blitz::Array< double, 2> lattice_L[ Components::number_of_Variables ];
    blitz::Array< double, 2> lattice_star[ Components::number_of_Variables ];


    blitz::Array< std::complex< double> , 2> latticeFft_I[ Components::number_of_Variables ];
    blitz::Array< std::complex< double> , 2> latticeFft_J[ Components::number_of_Variables ];
    blitz::Array< std::complex< double> , 2> latticeFft_K[ Components::number_of_Variables ];
    blitz::Array< std::complex< double> , 2> latticeFft_L[ Components::number_of_Variables ];
    blitz::Array< std::complex< double> , 2> latticeFft_star[ Components::number_of_Variables ];

    fftw_plan forwardPlan_I[ Components::number_of_Variables ];
    fftw_plan backwardPlan_I[ Components::number_of_Variables ];
    fftw_plan forwardPlan_J[ Components::number_of_Variables ];
    fftw_plan backwardPlan_J[ Components::number_of_Variables ];
    fftw_plan forwardPlan_K[ Components::number_of_Variables ];
    fftw_plan backwardPlan_K[ Components::number_of_Variables ];
    fftw_plan forwardPlan_L[ Components::number_of_Variables ];
    fftw_plan backwardPlan_L[ Components::number_of_Variables ];
    fftw_plan forwardPlan_star[ Components::number_of_Variables ];
    fftw_plan backwardPlan_star[ Components::number_of_Variables ];

    void diffOp( blitz::Array< std::complex<double>, 2>* bz, double a );
};

template<typename T_model>
void ARK45IP_LatticeIntegrator< T_model >::diffOp(blitz::Array< std::complex< double >, 2 >* bz,
                                                  double a)
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if ( Base::diffusion_[ component ] == 0.0 )
            continue;
        blitz::Array< std::complex< double >, 2 > diffusionOperator( bz[ component ].shape() );
        int max_x_out = (Base::latticeSizeX());
        int max_y_out = (Base::latticeSizeY()) / 2 + 1;
        static const long double SQRT_M_PIl_POW_3l = pow( sqrt( M_PIl * 2. ), 3 );
        for (int i = 0; i < max_x_out; ++i) {
            for (int j = 0; j < max_y_out; ++j) {
                long double f_x = (i < Base::latticeSizeX() / 2) ? i : Base::latticeSizeX() - i;
                long double f_y = j;
                f_x = f_x * SQRT_M_PIl_POW_3l / Base::sizeX();
                f_y = f_y * SQRT_M_PIl_POW_3l / Base::sizeY();

                diffusionOperator( i, j ) = (exp( -(f_x * f_x + f_y * f_y) * Base::diffusion_[ component ]
                    * Base::tau * a ));
            }
        }
        bz[ component ] = bz[ component ] * diffusionOperator;
    }
}

template<typename T_model>
void ARK45IP_LatticeIntegrator< T_model >::step()
{

    // Cash-Karp coefficients
    double _a[7];
    double _b[7][7];
    double _c[7];
    double _cs[7];
    double _f[7];

    _a[0]=0.0;
    _a[1]=0.0;
    _a[2]=1.0/5;
    _a[3]=3.0/10;
    _a[4]=3.0/5;
    _a[5]=1.0;
    _a[6]=7.0/8.0;

    _b[2][1]=1.0/5;
    _b[3][1]=3.0/40;
    _b[3][2]=9.0/40;
    _b[4][1]=3.0/10;
    _b[4][2]=-9.0/10;
    _b[4][3]=6.0/5;
    _b[5][1]=-11.0/54;
    _b[5][2]=5.0/2;
    _b[5][3]=-70.0/27;
    _b[5][4]=35.0/27;
    _b[6][1]=1631.0/55296;
    _b[6][2]=175.0/512;
    _b[6][3]=575.0/13824;
    _b[6][4]=44275.0/110592;
    _b[6][5]=253.0/4096;

    _c[0]=0.0;
    _c[1]=37.0/378;
    _c[2]=0.0;
    _c[3]=250.0/621;
    _c[4]=125.0/594;
    _c[5]=0.0;
    _c[6]=512.0/1771;

    _cs[0]=0.0;
    _cs[1]=2825.0/27648;
    _cs[2]=0.0;
    _cs[3]=18575.0/48384;
    _cs[4]=13525.0/55296;
    _cs[5]=277.0/14336;
    _cs[6]=1.0/4;

    double _g=_c[1]*_cs[4]-_cs[1]*_c[4];
    _f[0]=0.0;
    _f[1]=(_b[6][4]*(_cs[1]-_c[1]) + _b[6][1]*(_c[4]-_cs[4]))/_g + 1.0;
    _f[2]=  _b[6][2];
    _f[3]=(_b[6][4]*(_cs[1]*_c[3] - _c[1]*_cs[3]) + _b[6][1]*(_cs[3]*_c[4] - _c[3]*_cs[4]))/_g + _b[6][3];
    _f[4]=(_b[6][1]*_cs[4]-_b[6][4]*_cs[1])/_g;
    _f[5]=  _b[6][5] + _cs[5]*(_b[6][1]*_c[4]-_b[6][4]*_c[1])/_g;
    _f[6]=(-_b[6][1]*_c[4]+_b[6][4]*_c[1])/_g;

    // 0. a in Fourier
    LOOP_COMPONENTS {
        fftw_execute( Base::forwardPlan[ component ] );
    }
    // 1. Calc. xi
    // 2. a_k = a
    LOOP_COMPONENTS {
        latticeFft_K[ component ] = Base::latticeFft[ component ];
    }
    // 3. a = D(a2)a
    diffOp( Base::latticeFft, _a[2] );
    // 4. aI = a
    LOOP_COMPONENTS latticeFft_I[ component ] = Base::latticeFft[ component ];
    //5. a∗ = a
    LOOP_COMPONENTS latticeFft_star[ component ] = Base::latticeFft[ component ];
    // 6. aK = h F [N (x0 , F −1 [aK ] , ξ)]
    LOOP_COMPONENTS {
            fftw_execute( backwardPlan_K[ component ] );
            lattice_K[component] /= static_cast<double> ( Base::latticeSize() );
        }

    for(int i=0; i<Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[component].data()[i];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_K[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_K[ component ] = Base::tau * latticeFft_K[ component ];
    }
    // 7. aK = e−a2 hL(x       )·a
    diffOp( latticeFft_K, -_a[2] );
    // 8. a = a + c1 aK
    LOOP_COMPONENTS
        Base::latticeFft[ component ] = Base::latticeFft[ component ] + _c[1] * latticeFft_K[ component ];


    // 9. a∗ = a∗ + c1∗ aK
    LOOP_COMPONENTS
        latticeFft_star[ component ] = latticeFft_star[ component ] + _cs[1]
            * latticeFft_K[ component ];

   // 10. x0 = x0 + a2 h
    Base::time_ = Base::time_ + _a[2] * Base::tau;
   // 11. aK = aI + b21 aK
    LOOP_COMPONENTS
    latticeFft_K[ component ] = latticeFft_I[ component ] + _b[2][1]
                * latticeFft_K[ component ];
    // 12. aK = h F [N (x0 , F −1 [aK ] , ξ)]
    LOOP_COMPONENTS {
            fftw_execute( backwardPlan_K[ component ] );
            lattice_K[component] /= static_cast<double> ( Base::latticeSize() );
        }

    for(int i=0; i<Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_K[component].data()[i];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_K[component].data()[i] = f[component] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_K[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_K[ component ] = Base::tau * latticeFft_K[ component ];
    }


   // 13. aJ = (1 − b31 /c1 )aI + b31 /c1 a + b32 aK
    LOOP_COMPONENTS
        latticeFft_J[ component ] = (1.0 - _b[3][1] / _c[1]) * latticeFft_I[ component ] + _b[3][1] / _c[1]
            * Base::latticeFft[ component ] + _b[3][2] * latticeFft_K[ component ];

    // 14. x0 = x0 + (a3 − a2 )h
    Base::time_ = Base::time_ + (_a[3] - _a[2]) * Base::tau;
    // 15.
    diffOp( latticeFft_J, _a[3] - _a[2] );

    //16. aJ = h F [N (x0 , F −1 [aJ ] , ξ)]

    LOOP_COMPONENTS {
        fftw_execute( backwardPlan_J[ component ] );
        lattice_J[component] /= static_cast<double> ( Base::latticeSize() );
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_J[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_J[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_J[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_J[ component ] = Base::tau * latticeFft_J[ component ];
    }

    // 17. aJ = e−(a3 −a2 )hL(x          )·a
    diffOp( latticeFft_J, -(_a[3] - _a[2]) );
    // 18.
    LOOP_COMPONENTS {
        latticeFft_L[ component ] = (1.0 - _b[4][1] / _c[1]) * latticeFft_I[ component ] + _b[4][1] / _c[1]
            * Base::latticeFft[ component ] + _b[4][2] * latticeFft_K[ component ] + _b[4][3]
            * latticeFft_J[ component ];    }

    LOOP_COMPONENTS {
        // 19. a = a + c3 aJ
        Base::latticeFft[ component ] = Base::latticeFft[ component ] + _c[3]
            * latticeFft_J[ component ];
        // 20. a∗ = a∗ + c3∗ aJ
        latticeFft_star[ component ] = latticeFft_star[ component ] + _cs[3]
            * latticeFft_J[ component ];
    }

    // 21. x0 = x0 + (a4-a3)h
    Base::time_ = Base::time_ + (_a[4] - _a[3]) * Base::tau;
    //22.
    diffOp( latticeFft_L, _a[4] - _a[2] );
    // 23. aL
    LOOP_COMPONENTS {
        fftw_execute( backwardPlan_L[ component ] );
        lattice_L[component] /= static_cast<double> ( Base::latticeSize() );
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_L[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_L[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_L[ component ] = Base::tau * latticeFft_L[ component ];
    }
    // 24.
    diffOp( latticeFft_L, -(_a[4] - _a[2]) );
    // 25. 26
    LOOP_COMPONENTS {
        Base::latticeFft[ component ] = Base::latticeFft[ component ] + _c[4]
            * latticeFft_L[ component ];
        latticeFft_star[ component ] = latticeFft_star[ component ] + _cs[4]
            * latticeFft_L[ component ];
    }
    // 27.
    LOOP_COMPONENTS
        latticeFft_L[ component ] = (1 - _b[5][1] / _c[1]) * latticeFft_I[ component ] + _b[5][1] / _c[1]
            * Base::latticeFft[ component ] + _b[5][2] * latticeFft_K[ component ] + (_b[5][3] - _b[5][1] * _c[3]
            / _c[1]) * latticeFft_J[ component ] + (_b[5][4] - _b[5][1] * _c[4] / _c[1]) * latticeFft_L[ component ];

    // 28.
    Base::time_ += Base::tau * (_a[5] - _a[4]);
    // 29.
    diffOp( latticeFft_L, _a[5] - _a[2] );
    // 30.

    LOOP_COMPONENTS {
        fftw_execute( backwardPlan_L[ component ] );
        lattice_L[component] /= static_cast<double> ( Base::latticeSize() );
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_L[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_L[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_L[ component ] = Base::tau * latticeFft_L[ component ];
    }
    // 31.
    diffOp( latticeFft_L, -(_a[5] - _a[2]) );
    // 32.
    LOOP_COMPONENTS
        latticeFft_star[ component ] = latticeFft_star[ component ] + _cs[5]
            * latticeFft_L[ component ];
    // 33.
    LOOP_COMPONENTS
        latticeFft_L[ component ] = _f[1] * latticeFft_L[ component ] + _f[2] * latticeFft_K[ component ]
            + _f[3] * latticeFft_J[ component ] + _f[4] * Base::latticeFft[ component ] + _f[5]
            * latticeFft_L[ component ] + _f[6] * latticeFft_star[ component ];
    // 34.
    Base::time_ += Base::tau * (_a[6] - _a[5]);
    // 35.
    diffOp( latticeFft_L, _a[6] - _a[2] );
    // 36.

    LOOP_COMPONENTS {
        fftw_execute( backwardPlan_L[ component ] );
        lattice_L[component] /= static_cast<double> ( Base::latticeSize() );
    }

    for (int i = 0; i < Base::latticeSize(); ++i) {
        Components in;
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            in[ component ] = lattice_L[ component ].data()[ i ];
        }
        Components f = static_cast< T_model* > ( this ) -> step_f( in );
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice_L[ component ].data()[ i ] = f[ component ] * Base::tau;
        }
    }

    LOOP_COMPONENTS {
        fftw_execute( forwardPlan_L[ component ] );
    }
    LOOP_COMPONENTS {
        latticeFft_L[ component ] = Base::tau * latticeFft_L[ component ];
    }

    // 37.
    diffOp( latticeFft_L, -(_a[6] - _a[2]) );
    // 38. 39. // J oder L???
    LOOP_COMPONENTS {
        Base::latticeFft[ component ] = Base::latticeFft[ component ] + _c[6]
            * latticeFft_L[ component ];
        latticeFft_star[ component ] = latticeFft_star[ component ] + _cs[6]
            * latticeFft_L[ component ];
    }

    // 40.
    Base::time_ = Base::time_ + (_a[6] - _a[5]) * Base::tau;
    // 41. 42.
    diffOp( Base::latticeFft, 1 - _a[2] );
    diffOp( latticeFft_star, 1 - _a[2] );


    LOOP_COMPONENTS {
        fftw_execute( Base::backwardPlan[ component ] );
        Base::lattice[component] /= static_cast<double> ( Base::latticeSize() );
    }

}




#include "lattice_integrator.cpp"

#endif /* LATTICE_INTEGRATOR_H_ */
