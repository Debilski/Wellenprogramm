#ifndef LATTICE_INTERFACE_H
#define LATTICE_INTERFACE_H

#include <list>
#include <vector>
#include <map>
#include <blitz/array.h>

#include "lattice_geometry.h"
#include "rds_helper.h"

class LatticeInterface;

#include "noise_generator.h"
typedef unsigned int uint;

/**
 * Allgemeines System mit number_of_Variables Komponenten.
 */
struct GeneralComponentSystem {
    blitz::Array< double, 1 > components;
    uint number_of_Variables;

    //  GeneralComponentSystem( const GeneralComponentSystem& generalComponentSystem ) : components( generalComponentSystem ) {}

    GeneralComponentSystem(blitz::Array< double, 1 > vector) :
        components( vector ), number_of_Variables( components.size() )
    {
    }
    GeneralComponentSystem(const std::vector< double >& vector) :
        components( vector.size() ), number_of_Variables( components.size() )
    {
        for (uint i = 0; i < vector.size(); ++i)
            components( i ) = vector[ i ];
    }
    GeneralComponentSystem(int size) :
        components( size ), number_of_Variables( components.size() )
    {
        components = 0;
    }
    GeneralComponentSystem() :
        components( 1 ), number_of_Variables( components.size() )
    {
        components = 0;
    }
    void resize(int size)
    {
        components.resize( size );
        number_of_Variables = components.size();
    }
    std::vector< double > toVector() const
    {
        std::vector< double > vector;
        for (uint i = 0; i < number_of_Variables; ++i)
            vector.push_back( components( i ) );
        return vector;
    }
    const double& operator[](unsigned i) const
    {
        return components( i );
    }

    double& operator[](unsigned i)
    {
        return components( i );
    }

    friend std::ostream& operator<<(std::ostream& o, const GeneralComponentSystem& sys)
    {
        return o << sys.components;
    }
};

/**
 * ComponentSystem ist ein Wrapper um blitz::TinyVector.
 *
 * Der Grund für Nutzung von blitz::TinyVector ist unter anderem darin begründet, dass
 * TinyVector bereits für bis zu 11 Argumente vordefinierte Konstruktoren mitliefert,
 * welche das Implementieren eigener Componenten-Typen wesentlich übersichtlicher macht.
 * Da TinyVector seinerseits nur ein sehr sehr schlanker Wrapper um ein Array fester Größe ist,
 * sollten bei inline-Setzung keine nennenswerten Verluste auftreten.
 *
 * In der Benutzung würde man eine Subklasse zu ComponentSystem<N> definieren, und einige
 * hilfreiche Funktionen für den Zugriff auf einzelne Komponenten.
 *
 * \param nVars Die Anzahl der Parameter für eine Komponente
 */
template<uint nVars>
struct ComponentSystem {
    typedef blitz::TinyVector< double, nVars > T_vector;

    T_vector components;
    //double components[nVars];

    static const uint number_of_Variables = nVars;

    ComponentSystem() : components()    {}
    ComponentSystem(T_vector vector) :
        components( vector )
    {
//        for (uint i=0; i<vector.length(); ++i)
//            components[i] = vector[i];
    }
    ComponentSystem(const GeneralComponentSystem& generalComponentSystem)
    {
        for (uint i = 0; i < number_of_Variables; ++i) {
            if ( i < generalComponentSystem.number_of_Variables ) {
                components[ i ] = generalComponentSystem[ i ];
            } else {
                components[ i ] = 0;
            }
        }
    }
    const T_vector& toTinyVector() const { return components; }
    T_vector& toTinyVector() { return components; }
    const double& operator[](unsigned i) const
    {
        return components[ i ];
    }
    double& operator[](unsigned i)
    {
        return components[ i ];
    }
    friend std::ostream& operator<<(std::ostream& o, const ComponentSystem& sys)
    {
        return o << sys.components;
    }
    /**
     * Prüft, ob alle Komponenten kleiner sind als ein bestimmter Wert.
     */
    bool isTiny() {
        //return true;
        //return blitz::all( blitz::fabs(components) < 1.0E-7 );
        return CheckTininess< number_of_Variables >::isTiny( components.data() );
    }
};

/**
 * Basis-Variablen zur Darstellung eines Hodgkin-Huxley-Modells.
 */
struct HodgkinHuxleyComponentSystem : ComponentSystem< 4 > {
    HodgkinHuxleyComponentSystem() :
        ComponentSystem< 4 > ( T_vector( 0, 0, 0, 0 ) )
    {
    }
    HodgkinHuxleyComponentSystem(double V, double n, double m, double h) :
        ComponentSystem< 4 > ( T_vector( V, n, m, h ) )
    {
    }
    HodgkinHuxleyComponentSystem(T_vector vector) :
        ComponentSystem< 4 > ( vector )
    {
    }
    HodgkinHuxleyComponentSystem(const GeneralComponentSystem& generalComponentSystem) :
        ComponentSystem< 4 > ( generalComponentSystem )
    {
    }

    const double& V() const
    {
        return components[ 0 ];
    }
    const double& n() const
    {
        return components[ 1 ];
    }
    const double& m() const
    {
        return components[ 2 ];
    }
    const double& h() const
    {
        return components[ 3 ];
    }
    double& V()
    {
        return components[ 0 ];
    }
    double& n()
    {
        return components[ 1 ];
    }
    double& m()
    {
        return components[ 2 ];
    }
    double& h()
    {
        return components[ 3 ];
    }
};

struct ThreeComponentSystem : ComponentSystem< 3 > {
    ThreeComponentSystem() :
        ComponentSystem< 3 > ( T_vector( 0, 0, 0 ) )
    {
    }
    ThreeComponentSystem(double u, double v, double w) :
        ComponentSystem< 3 > ( T_vector( u, v, w ) )
    {
    }
    ThreeComponentSystem(T_vector vector) :
        ComponentSystem< 3 > ( vector )
    {
    }
    ThreeComponentSystem(const GeneralComponentSystem& generalComponentSystem) :
        ComponentSystem< 3 > ( generalComponentSystem )
    {
    }

    const double& u() const
    {
        return components[ 0 ];
    }
    const double& v() const
    {
        return components[ 1 ];
    }
    const double& w() const
    {
        return components[ 2 ];
    }
    double& u()
    {
        return components[ 0 ];
    }
    double& v()
    {
        return components[ 1 ];
    }
    double& w()
    {
        return components[ 2 ];
    }
};

/**
 * System with two Components named @c u and @c v.
 */
struct TwoComponentSystem : ComponentSystem< 2 > {
    TwoComponentSystem() :
        ComponentSystem< 2 > ( T_vector( 0, 0 ) )
    {
    }
    TwoComponentSystem(double u, double v) :
        ComponentSystem< 2 > ( T_vector( u, v ) )
    {
    }
    TwoComponentSystem(T_vector vector) :
        ComponentSystem< 2 > ( vector )
    {
    }
    TwoComponentSystem(const GeneralComponentSystem& generalComponentSystem) :
        ComponentSystem< 2 > ( generalComponentSystem )
    {
    }

    const double& u() const
    {
        return components[ 0 ];
    }
    const double& v() const
    {
        return components[ 1 ];
    }
    double& u()
    {
        return components[ 0 ];
    }
    double& v()
    {
        return components[ 1 ];
    }
};

struct OneComponentSystem : ComponentSystem< 1 > {
    OneComponentSystem(double u = 0) :
        ComponentSystem< 1 > ( T_vector( u ) )
    {
    }
    OneComponentSystem(T_vector vector) :
        ComponentSystem< 1 > ( vector )
    {
    }
    OneComponentSystem(const GeneralComponentSystem& generalComponentSystem) :
        ComponentSystem< 1 > ( generalComponentSystem )
    {
    }
    const double& u() const
    {
        return components[ 0 ];
    }
    double& u()
    {
        return components[ 0 ];
    }
};



#define _COMPONENT_SYSTEM_CONSTRUCTORS(system_number, system_name)                          \
    system_name( ) : ComponentSystem< system_number >( ) {}                             \
    system_name( T_vector v ) : ComponentSystem< system_number >( v ) {}                \
    system_name( const GeneralComponentSystem& generalComponentSystem) :                \
        ComponentSystem< system_number > ( generalComponentSystem ) {}

#define _COMPONENT_SYSTEM_BEGIN(system_number, system_name)                                 \
    struct system_name : ComponentSystem< system_number > {                                 \
    _COMPONENT_SYSTEM_CONSTRUCTORS(system_number, system_name)

#define _COMPONENT_SYSTEM_ACCESSOR( num, var_name )                                         \
    const double& var_name () const { return components[ num ]; }                           \
          double& var_name ()       { return components[ num ]; }

#define _COMPONENT_SYSTEM_END                                                               \
    };

#define ONE_COMPONENT_SYSTEM(system_name, var_name_0)                                       \
    _COMPONENT_SYSTEM_BEGIN( (1), system_name )                                             \
    system_name( double var_name_0 ) : ComponentSystem< 1 >( T_vector( var_name_0 ) ) {}    \
    _COMPONENT_SYSTEM_ACCESSOR( 0, var_name_0 )                                             \
    _COMPONENT_SYSTEM_END

#define TWO_COMPONENT_SYSTEM(system_name, var_name_0, var_name_1)                                       \
    _COMPONENT_SYSTEM_BEGIN( (2), system_name )                                             \
    system_name( double var_name_0, double var_name_1 ) : ComponentSystem< 2 >( T_vector( var_name_0, var_name_1 ) ) {}    \
    _COMPONENT_SYSTEM_ACCESSOR( 0, var_name_0 )                                             \
    _COMPONENT_SYSTEM_ACCESSOR( 1, var_name_1 )                                             \
    _COMPONENT_SYSTEM_END

#define THREE_COMPONENT_SYSTEM(system_name, var_name_0, var_name_1, var_name_2)                                       \
    _COMPONENT_SYSTEM_BEGIN( (3), system_name )                                             \
    system_name( double var_name_0, double var_name_1, double var_name_2 ) : ComponentSystem< 3 >( T_vector( var_name_0, var_name_1, var_name_2 ) ) {}    \
    _COMPONENT_SYSTEM_ACCESSOR( 0, var_name_0 )                                             \
    _COMPONENT_SYSTEM_ACCESSOR( 1, var_name_1 )                                             \
    _COMPONENT_SYSTEM_ACCESSOR( 2, var_name_2 )                                             \
    _COMPONENT_SYSTEM_END


ONE_COMPONENT_SYSTEM(fhn, u)
TWO_COMPONENT_SYSTEM(fhn2, u, v)


/**
 * Templateklasse für einen Parameter.
 *
 * Der Vorteil der Klasse ist, dass man den Parameter nun durch ein externes Programm lesen kann, ohne
 * dass das Programm (und der Programmierer) von der Existenz des Parameters zur Übersetzungszeit wissen muss.
 *
 */
template<class T>
struct Parameter {
    Parameter(T initial = 0, const std::string& name = std::string(), T min = 0, T max = 500,
              int decimals = 3) :
        name( name ), value_( initial ), min_( min ), max_( max ), stepSizeHint_( 0.05 ),
            decimals_( decimals )
    {
    }
    Parameter(Parameter const& P) :
        name( P.name ), value_( P.value_ ), min_( P.min_ ), max_( P.max_ ), stepSizeHint_(
            P.stepSizeHint_ ), decimals_( P.decimals_ )
    {
    }
    ~Parameter()
    {
    }
    inline operator T()
    {
        return value_;
    }
    inline Parameter& operator=(T value)
    {
        this.value_ = value;
        return *this;
    }
    inline Parameter& operator=(const Parameter& p)
    {
        this.value_ = p.value_;
        return *this;
    }
    std::string name;
    inline void set(T param)
    {
        value_ = param;
    }
    inline const T& operator()() const
    {
        return value_;
    }
    inline T& operator()()
    {
        return value_;
    }
    inline T get() const
    {
        return value_;
    }
    inline T max() const
    {
        return max_;
    }
    inline T min() const
    {
        return min_;
    }
    inline T stepSizeHint() const
    {
        return stepSizeHint_;
    }
    inline int decimals() const
    {
        return decimals_;
    }
private:
    T value_;
    T min_;
    T max_;
    T stepSizeHint_;
    int decimals_;
    std::string physicalUnit_;
    std::string physicalDimension_;
};

/**
 * Enum für mögliche Randbedingungen.
 *
 * Was nicht geht ist, zwei Componenten unterschiedlich zu behandeln.
 */
enum BoundaryCondition {
    FixedBoundary,
    PeriodicBoundary /* = No Boundary */,
    NoFluxBoundary,
    FixedValueBoundary,
    NoReactionBoundary
};

template<typename Components>
struct Defect {
    SurfacePoint centre;
    double radius;
    Components value;
    BoundaryCondition boundaryCondition;
    std::vector< SurfacePoint > polygon;

    Defect()
    {
    }
    Defect(const Defect< GeneralComponentSystem >& d) :
        centre( d.centre ), radius( d.radius ), value( Components( d.value ) ), boundaryCondition(
            d.boundaryCondition )
    {
    }
};

/**
 * Beinhaltet Informationen über die Komponenten.
 *
 * Das Ausschalten der Diffusion ist an dieser Stelle noch nicht implementiert.
 */
class ComponentInfo {
public:
    /**
     * Legt neue Informationen für eine Komponente an
     * \param name Der lange Bezeichner der Komponente
     * \param shortName Abkürzung
     * \param assumedMin Liefert einen Minimalwert, der bei der graphischen Ausgabe benutzt werden kann.
     * \param assumedMax    Liefert einen Maximalwert, der bei der graphischen Ausgabe benutzt werden kann.
     * \param canHaveDiffusion  Soll angeben, ob für die Komponente eine FFT angelegt werden soll. (Noch nicht implementiert.)
     */
    ComponentInfo( std::string name = std::string(), std::string shortName = std::string(), double assumedMin = 0, double assumedMax = 0 )
    : componentName_(name), componentNameShort_(shortName), assumedMin_(assumedMin), assumedMax_(assumedMax), canHaveDiffusion_(true)
    {}
    inline std::string name() const {return componentName_;}
    inline std::string shortName() const {return componentNameShort_;}
    inline std::string physicalUnitSymbol() const {return physicalUnitSymbol_;}
    inline ComponentInfo& setPhysicalUnitSymbol(std::string name) { physicalUnitSymbol_ = name; return *this; }
    inline std::string physicalQuantity() const {return physicalQuantity_;}
    inline ComponentInfo& setPhysicalQuantity(std::string name) { physicalQuantity_ = name; return *this; }
    inline double assumedMin() const { return assumedMin_;}
    inline double assumedMax() const { return assumedMax_;}
private:
    std::string componentName_;
    std::string componentNameShort_;
    std::string physicalUnitSymbol_;
    std::string physicalQuantity_;
    double assumedMin_;
    double assumedMax_;
    bool canHaveDiffusion_;
};

typedef std::vector< ComponentInfo > ComponentInfos;

/**
 * Simple struct of a cluster.
 *
 * Provides an ID for the Cluster as well as its size and position on the surface.
 */
struct Cluster {
    long int clusterId;
    double size;
    SurfacePoint position;
};


/**
 * Describes a cluster with time information.
 *
 * Has yet to be used by the program, but could provide a cleaner solution to historical cluster data.
 */
struct HistroricCluster : public Cluster {
    double time;
};
/*
template < T >
class LinearOperatorDifferential {
  void operator=( double value );
};

class LinearOperatorClass {
    LinearOperatorDifferential< int >& dx(int);
    LinearOperatorDifferential< int >& dy(int);

};

class FractionalLinearOperatorClass {
    LinearOperatorDifferential< double >& dx(double);
    LinearOperatorDifferential< double >& dy(double);
};
*/






/**
 * LatticeInterface allows for virtual access to lattice methods,
 * without knowing anything about the number of parameters at compile time.
 *
 * Das Interface des Programmes bietet in erster Linie die virtuellen Methoden an,
 * die notwendig sind, um das Programm auch ohne die Informationen aus dem genauen Modell zur Compile-Zeit zu kennen
 * nutzen zu können.
 * Es ist eine abstrakte Klasse und kann somit nicht direkt instanziiert werden.
 *
 * Memory-Usage ist etwa sizeof(double)*Nx*Ny*(NumberOfComp*2(normal,fft) + 1(noise) + 1(cluster)) + X
 * Besser wäre es, eine Option einzubauen, die steuert, ob auch unnötige FFT schon zu Anfang initialisiert
 * werden sollen, oder ob die Felder erst bei Bedarf erstellt werden sollen.
 */
class LatticeInterface : public LatticeGeometry {
public:

    LatticeInterface(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);
    virtual ~LatticeInterface() = 0;

    /**
     * Liefert die Komponente an einem Gitterpunkt.
     *
     * \param component Der Index der Komponente
     * \param x X-Koordinate auf dem Gitter
     * \param y Y-Koordinate auf dem Gitter
     */
    virtual double getComponentAt(uint component, int x, int y) const = 0;

    virtual std::complex<double> getFftComponentAt(uint component, int x, int y) const = 0;

    /**
     * Liefert die Komponente an einem Gitterpunkt.
     *
     * Überladene Bequemlichkeitsfunktion für virtual double getComponentAt(uint component, int x, int y) const
     * \param component Der Index der Komponente
     * \param pos Position auf dem Gitter
     */
    double getComponentAt(uint component, LatticePoint pos) const
    {
        return getComponentAt( component, pos.x, pos.y );
    }

    /**
     * Setzt einen Gitterpunkt auf einen Wert.
     *
     * \param component Der Index der Komponente
     * \param x X-Koordinate auf dem Gitter
     * \param y Y-Koordinate auf dem Gitter
     */
    virtual void setComponentAt(uint component, double value, int x, int y) = 0;

    void setComponentAt(uint component, double value, LatticePoint pos)
    {
        return setComponentAt( component, value, pos.x, pos.y );
    }

    virtual uint numberOfVariables() const = 0;
    virtual void step(int numberOfSteps = 1) = 0;

    virtual void clear() = 0;
    /**
     *  Lädt den voreingestellten Initalwert i
     */
    virtual void toInitial(int num = 0) = 0;

    /**
     * Liefert das aktuelle Maximum für eine Komponente
     */
    virtual double getMax(uint component) const = 0;

    /**
     * Liefert das aktuelle Minimum für eine Komponente
     */
    virtual double getMin(uint component) const = 0;

    virtual double numberLargerThanThreshold(double threshold) const = 0;
    virtual double currentWavesize() const = 0;

    virtual void setSpotAtComponent(double x, double y, double size, double value, uint component,
                                    bool round) = 0;
    virtual double setSpotAt(double x, double y, double size, const GeneralComponentSystem& values,
                           bool round) = 0;

    virtual double setSpotAt(double x, double y, double radius, BoundaryCondition bc) = 0;

    virtual BoundaryCondition boundaryCondition() const = 0;
    virtual void setBoundaryCondition(BoundaryCondition bc) = 0;

    /**
     * Liefert die abgelaufene Simulationszeit
     */
    virtual double time() const = 0;

    /**
     * Setzt die Simulationszeit auf einen bestimmten Wert
     */
    virtual void setTime(double t) = 0;

    /**
     * Liefert den Simulationszeitschritt
     */
    virtual double timeStep() const = 0;

    /**
     * Stellt den Simulationszeitschritt neu ein.
     *
     * \attention Die Funktion wird keine Prüfung des Wertes durchführen; insbesondere
     * werden auch negative Zeitschritte zugelassen (falls es einmal sinnvoll sein sollte).
     * Da in \b suggestedTimeStep() jedoch negative Werte für Fehler oder unimplementiertes
     * Verhalten stehen, so ist hier Vorsicht geboten.
     *
     * \param tau Zeitschritt
     */
    virtual void setTimeStep(double tau) = 0;

    /**
     * Liefert einen Richtwert für den Zeitschritt.
     *
     * Diese Methode wird durch das Modell implementiert und gibt in der Regel
     * nur einen groben Anhaltspunkt, z.B. bestimmt durch Minumum und Maximum aktueller
     * Simulationswerte. Wurde diese Methode in einem Modell nicht
     * implementiert, oder ist es unmöglich, eine Aussage zu treffen, so wird ein negativer Wert
     * geliefert. Der Nutzer hat selbst dafür zu sorgen, dass solche Werte verworfen werden.
     *
     * \return Richtwert
     */
    virtual double suggestedTimeStep() const = 0;

    /**
     * Gibt ein neues blitz::Array<double> zurück, welches die angegebene Komponente enthält.
     *
     * Das Array wird hierfür neu erzeugt und teilt nach der Rückgabe keine Daten mit den
     * Arrays des Systems. Der Anwender hat selbst dafür zu sorgen, dass die verweisende
     * Variable ihren Gültigkeitsbereich verlässt oder sich anderweitig um die Zerstörung
     * des Objekts zu kümmern.
     *
     * \param Component Die auszugebende Komponente
     */
    virtual blitz::Array< double, 2 > getRawComponent(uint component) const = 0;

    /**
     * Ersetzt eine Komponente komplett durch ein durch den Nutzer gegebenes blitz::Array<double>.
     *
     * Dies kann hilfreich sein, wenn spezielle Anfangsbedingungen gegeben werden sollen und es nicht
     * zweckmäßig erscheint NxN Aufrufe von virtual setComponentAt durchzuführen.
     * Eine reine Übergabe des Arrays als Referenz ist zur Zeit wegen der damit verbundenen Schwierigkeiten
     * nicht implementiert. Dies könnte bei unsachgemäßer Verwendung – oder auch bei sachgemäßer Verwendung –
     * dazu führen, dass der Datenbereich des Arrays heimlich verschoben würde, wodurch die Pläne der
     * FFTW-Bibliothek undefiniertes Verhalten zeigen würden.
     *
     * Sollten diverse Operationen einen ausgedehnteren Zugriff auf die Arrays benötigen, als Beispiel seien
     * hier beispielsweise weitere Fourier-Transformationen oder fortlaufende statistische Auswertung genannt,
     * so ist empfohlen, dies in einer eigenen Integration-Klasse abzuleiten und alles Nötige dort auszuführen.
     * Zugriff auf die generierten Daten könnte man dann beispielsweise mit befreundeten Klassen erhalten,
     * ähnlich wie dies auch mit dem ClusterCounter funktioniert.
     *
     * \attention Aus Sicherheitsgründen wird diese Funktion den Plan für die Fourier-Transformation auf dieser
     * Komponente tatsächlich neu initialisieren. Je nach den gegebenen Umständen \i kann das nochmals eine gewisse
     * Zeit dauern; im Idealfall sollten allerdings die alten Pläne zwischengespeichert und brauchbar sein.
     *
     * \param Component Die Komponete, welche ersetzt werden soll.
     * \param raw Das zweidimensionale blitz++-Array mit den Input-Daten
     * \return Bei nicht übereinstimmenden Dimensionen wird \b false zurück gegeben, ansonsten \b true.
     */
    virtual bool setRawComponent(uint component, const blitz::Array< double, 2 >& raw) = 0;

    /**
     * Gibt Statusinformationen über das System aus.
     */
    virtual void status(std::ostream& o = std::cout) = 0;

    virtual void setDiffusion(uint component, double value) = 0;
    virtual double getDiffusion(uint component) const = 0;

    virtual double getFixpoint(uint component) const = 0;
    virtual int numberOfClusters() = 0;
    virtual std::vector< Cluster > getClusters() = 0;

    //virtual double noiseIntensity() = 0;
    virtual int noiseCorrelation() = 0;
    virtual void setNoiseIntensity(double d) = 0;
    virtual void setNoiseCorrelation(int i) = 0;

    virtual void dump(std::ostream& stream, int every = 1, bool isBinary = false) const = 0;
    virtual void undump(std::istream& stream, int every = 1, bool isBinary = false) = 0;
    virtual void save(bool append = false, std::string fileName = "tmpsave.bin") const = 0;
    virtual void recall(std::string fileName = "tmpsave.bin") = 0;

    /**
     * Gibt die Matrizen in einem von Matlab lesbaren Format aus.
     */
    //virtual void saveToMatlab(int accuracy) const = 0;
    /**
     * Lädt aus dem Matlab-Format
     */
    //virtual void loadFromMatlab(std::string fileName) = 0;

    virtual void applyDefect(Defect< GeneralComponentSystem > defect) = 0;
    //virtual void applyDefect( Defect<Components> defect );
    virtual void applyDefects() = 0;

    virtual void addDefect(Defect< GeneralComponentSystem > defect) = 0;
    //virtual void addDefect( Defect<Components> defect );
    virtual void removeDefects() = 0;

    virtual void setNoiseGenerator(uint noiseComponent, NoiseGenerator* ng) = 0;
    /*
     double midpointSize()
     {
     return 0.;
     }
     void midpointSize(double&)
     {
     }
     */

    /**
     * Gibt eine Liste mit Zeigern der registrierten Parameter zurück.
     */
    virtual std::list< Parameter< double >* > parameters() = 0;

    /**
     * Gibt eine Liste mit Zeigern auf die Parameter für den Adaptionsmodus zurück.
     */
    virtual std::list< Parameter< double >* > adaptationParameters() = 0;


    /**
     * Gibt einen Zeiger auf einen Parameter anhand des Namens zurück.
     *
     * \return Zeiger auf Parameter oder 0 im Fehlerfall.
     */
    virtual Parameter< double >* parameter(std::string name) = 0;
    //Parameter< double > gamma;

    virtual std::string modelName() const = 0;

    virtual SurfacePoint centreOfMass(uint component) const = 0;
    virtual SurfacePoint centreOfExcitation(uint component) const = 0;

    virtual bool isAlive() const = 0;

    virtual void setExcitationThreshold(double threshold) = 0;
    virtual double excitationThreshold() const = 0;

    virtual bool copyFromOtherReal(const LatticeInterface& other) = 0;
    virtual bool copyFromOtherLattice(const LatticeInterface& other) = 0;

    virtual bool insertOtherAt(const LatticeInterface& other, int x, int y) = 0;
    virtual bool copyFromOther(const LatticeInterface& other, int x, int y) = 0;

    virtual std::vector< SurfacePoint > getSpiralTips() = 0;

    virtual void adaptParameters() = 0;
    /*virtual*/ ComponentInfos componentInfos;
};

/**
 * Konstruktor des Interfaces.
 * Die Parameter werden ohne Verarbeitung an LatticeGeometry weiter gegeben.
 *
 * \param sizeX Reale Ausdehnung des Systems in x-Richtung
 * \param sizeY Reale Ausdehnung des Systems in y-Richtung
 * \param latticeSizeX Ausdehnung des zu Grunde liegenden Gitters in x-Richtung
 * \param latticeSizeY Ausdehnung des zu Grunde liegenden Gitters in y-Richtung
 */
inline LatticeInterface::LatticeInterface(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) :
    LatticeGeometry( sizeX, sizeY, latticeSizeX, latticeSizeY )
{
}

/**
 * Leere Definition des Virtuellen Destruktors des Interfaces.
 */
inline LatticeInterface::~LatticeInterface()
{
}




#endif
