/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

#ifndef LATTICE_H
#define LATTICE_H

#ifndef RDS_LATTICE_H_
#error must be included via rds_lattice.h
#endif

/**
 * Helper Class for additional but dependent Information
 */
template <typename T_model>
struct Metainfo;

/**
 *  Template der das Gitter bereitstellenden Klasse.
 *  \param Components  Die Klasse, welche die Komponenten bereitstellt.
 *
 */
template <typename T_model>
class Lattice : public LatticeInterface
{
public:
    /**
     * Die Komponenten des Modells.
     */
    typedef typename Metainfo<T_model>::Components Components;

    /**
     * Enum, welches den Zeitschritt bei der Diffusion angibt.
     *
     * Wenn in einer Implementation ein anderer Zeitschritt benötigt wird oder die Zeitschritte
     * in der Diffusion generell anders verteilt werden sollen, so empfiehlt es sich, \c diffusion
     * oder \c setDiffusion abzuleiten und neu zu implementieren. Generisch ist das schwierig zu lösen,
     * da Unterteilungen einen gewissen Overhead bei der Ausführung der Diffusion mit sich bringen.
     * Eine weitere Unterteilung könnte dort nur durch Wurzelbildung angewendet werden (oder durch
     * Quadrierung), was jedoch ebenfalls den Overhead nicht verringert.
     *
     */
    enum DiffusionStepWidth
    {
        /// Führt die Diffusion um einen halben Zeitschritt durch: \f$t \rightarrow t + \tau / 2\f$
        HalfStep,
        /// Führt die Diffusion um einen ganzen Zeitschritt durch: \f$t \rightarrow t + \tau \f$
        WholeStep,
        /// Ignoriert die Diffusion in diesem Schritt. Die Zeit wird nicht verändert.
        IgnoreStep
    };
    /* Diffusion wird nicht ausgeführt; Zeit bleibt stehen */
public:
    //typedef Components ComponentSystem;

    static const uint number_of_Variables = Components::number_of_Variables;
    static const uint number_of_Noise_Variables = Metainfo<T_model>::number_of_Noise_Variables;

    static const int CLUSTER_COUNTER_LATTICE = Metainfo<T_model>::CLUSTER_COUNTER_LATTICE;

    // Muss man sich noch was besseres für ausdenken
    std::string INTEGRATOR_NAME;

    virtual uint numberOfVariables() const
    {
        return number_of_Variables;
    }

    Lattice();
    virtual ~Lattice();

    virtual void init(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY);

    Components getAt(int x, int y) const;
    double getComponentAt(uint component, int x, int y) const;
    std::complex<double> getFftComponentAt(uint component, int x, int y) const;

    double getFixpoint(uint component) const
    {
        return fixpoint()[component];
    }
    Components fixpoint(long int position = -1) const;

    Components latticeComponents(
        GeneralComponentSystem generalComponentSystem) const
    {
        Components c(generalComponentSystem);
        return c;
    }

    void clear();
    void toZero();
    void toFixpoint();
    virtual void toInitial(int num = 0);

    void setAt(const Components& m, int x, int y);
    void setAtReal(const Components& m, int x, int y);

    blitz::Array<double, 2> getRawComponent(uint component) const;
    bool setRawComponent(uint component, const blitz::Array<double, 2>& raw);

    Components getMax() const;
    double getMax(uint component) const;
    Components getMin() const;
    double getMin(uint component) const;

    double numberLargerThanThreshold(double threshold) const;
    double currentWavesize() const;

    Components sumAll() const;
    // double recommendedTau();

    void setComponentAt(uint component, double value, int x, int y);

    void doNormalize();

    double time() const;
    void setTime(double t);
    double timeStep() const
    {
        return tau;
    }
    void setTimeStep(double t)
    {
        if (tau != t) {
            tau = t;
            computeDiffusionOperator();
        } else {
            tau = t;
        }
    }

    void setDiffusion(uint component, double value);
    void setDiffusion(const Components& diff_components);
    double getDiffusion(uint component) const;
    Components getDiffusion() const;

    void computeDiffusionOperator();
    virtual void computeDiffusionOperator(uint component);

protected:
    virtual void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true);

public:
    virtual void advanceTime(DiffusionStepWidth stepSize);

    virtual void step() = 0;
    virtual void step(int numberOfSteps)
    {
        for (int i = 0; i < numberOfSteps; ++i)
            step();
    }

    friend std::ostream& operator<<(std::ostream& o,
        const Lattice<T_model>& lattice)
    {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            o << lattice.lattice[component];
        }
        return o;
    }

    void status(std::ostream& o = std::cout);

    int numberOfClusters();
    std::vector<Cluster> getClusters();

    double noiseIntensity();
    int noiseCorrelation();
    void setNoiseIntensity(double d);
    void setNoiseCorrelation(int i);

    blitz::Array<std::complex<double>, 2>
        latticeFftOutput[Components::number_of_Variables];

    void dump(std::ostream& stream, int every = 1, bool isBinary = false) const;
    void undump(std::istream& stream, int every = 1, bool isBinary = false);
    void save(bool append = false, std::string fileName = "tmpsave.bin") const;
    void recall(std::string fileName = "tmpsave.bin");


    /**
     * Gibt die Matrizen in einem von Matlab lesbaren Format aus.
     */
    //void saveToMatlab(int accuracy) const {}
    /**
     * Lädt aus dem Matlab-Format
     */
    //void loadFromMatlab(std::string fileName) {}

    BoundaryCondition boundaryCondition() const;
    void setBoundaryCondition(BoundaryCondition bc);

    void applyDefect(Defect<GeneralComponentSystem> defect)
    {
        applyDefect(Defect<Components>(defect));
    }

    void addDefect(Defect<GeneralComponentSystem> defect)
    {
        addDefect(Defect<Components>(defect));
    }

    void applyDefect(Defect<Components> defect);
    void applyDefects();
    void applyNoFluxMap();
    void addDefect(Defect<Components> defect);
    void removeDefects();

    double setSpotAt(double x, double y, double radius,
        BoundaryCondition bc);
    virtual void setSpotAtComponent(double x, double y, double size,
        double value, uint component, bool round);
    double setSpotAt(double x, double y, double size, const Components& c,
        bool round);
    double setSpotAt(double x, double y, double size,
        const GeneralComponentSystem& values, bool round)
    {
        return setSpotAt(x, y, size, Components(values), round);
    }


    void setNoiseGenerator(uint noiseComponent, NoiseGenerator* ng);
    const NoiseGenerator* getNoiseGenerator(uint noiseComponent) const;

    std::string modelName() const
    {
        return modelName_;
    }

    std::string modelInformation() const
    {
        return modelInformation_;
    }

    SurfacePoint centreOfMass(uint component) const;
    SurfacePoint centreOfExcitation(uint component) const;

    double suggestedTimeStep() const;

    bool isAlive() const;

    std::list<Parameter<double>*> parameters()
    {
        return modelParameters_;
    }

    std::list<Parameter<double>*> adaptationParameters()
    {
        return modelAdaptationParameters_;
    }


    Parameter<double>* parameter(std::string name)
    {
        typedef std::list<Parameter<double>*>::const_iterator T_iter;
        for (T_iter it = modelParameters_.begin(); it != modelParameters_.end(); ++it) {
            if ((*it)->name == name)
                return (*it);
        }
        return 0;
    }

    std::vector<SurfacePoint> getSpiralTips();
    //    bool newton( SurfacePoint& p );
protected:
    ClusterCounter clusterCounter_;
    NoiseGenerator* noiseGenerator_[number_of_Noise_Variables];

private:
    //blitz::TinyVector<NoiseGenerator*, number_of_Noise_Variables> originalNoiseGenerator_;//[ number_of_Noise_Variables ];
    NoiseGenerator* originalNoiseGenerator_[number_of_Noise_Variables];

protected:
    void registerParameter(Parameter<double>* p)
    {
        modelParameters_.push_back(p);
    }

    void registerAdaptationParameter(Parameter<double>* p)
    {
        modelAdaptationParameters_.push_back(p);
    }

    blitz::Array<double, 2> lattice[Components::number_of_Variables];
    blitz::Array<std::complex<double>, 2>
        latticeFft[Components::number_of_Variables];

    blitz::Array<std::complex<double>, 2>
        //blitz::Array< double, 2>
        precomputedDiffusionOperator[Components::number_of_Variables];

    // Stellen ohne Diffusion
    blitz::Array<bool, 2> latticePointHasReaction_;

    std::map<int, int> noFluxCorrespondenceMap_;

    void setExcitationThreshold(double threshold);
    double excitationThreshold() const;

    bool copyFromOtherReal(const LatticeInterface& other);
    bool copyFromOtherLattice(const LatticeInterface& other);

    bool insertOtherAt(const LatticeInterface& other, int x, int y);
    bool copyFromOther(const LatticeInterface& other, int x, int y);

    /**
     * holds the time step
     */
    double tau;

    /**
     * holds the name of the model
     */
    std::string modelName_;

    /**
         * holds some more Information about the model
         */
    std::string modelInformation_;

    //Components fixpoint_;

    /**
     * holds the current time
     */
    double time_;

protected:
    /**
     * Kann vom Modell überschrieben werden, um Code vor einem Schritt auszuführen.
     *
     * Nicht virtual, da über CRTP aufgerufen.
     */
    void doBeforeStep() {}

    /**
     * Kann vom Modell überschrieben werden, um Code nach einem Schritt auszuführen.
     *
     * Nicht virtual, da über CRTP aufgerufen.
     */
    void doAfterStep() {}


    /**
     * \warning Experimental
     * Gives the threshold which is used for cluster counting, and wave size calculation.
     * Could be set in the model implementation.
     */
    double excitationThresholdFirstComponent_;
    long long numSteps_;

    double lastClusterCountTime_;

    double noiseIntensity_;
    int noiseCorrelation_;

    static void precomputeNoise(Lattice<T_model>* lattice);

    double diffusion_[Components::number_of_Variables];

    fftw_plan forwardPlan[Components::number_of_Variables];
    fftw_plan backwardPlan[Components::number_of_Variables];

    void createFftwPlanForComponent(uint component);
    void destroyFftwPlanForComponent(uint component);

    Fftw3Wrapper* fftw3Wrapper;
    // Gibt an, ob die Komponente bereits einen Plan hat.
    blitz::TinyVector<bool, Components::number_of_Variables> componentHasPlan;

    void setBoundary();

    BoundaryCondition boundaryCondition_;

    std::vector<Defect<Components> > defects_;
    std::vector<LatticePoint> noReactionList_;

    /**
     * Even though the blitz++ library is very fast, sometimes even accessing the
     * array data introduces a tiny overhead. If we do not want to risk anything, it
     * can be reasonable to map each lattice component’s Array::data() onto this array of pointers.
     *
     * \attention Make sure that this array of pointers gets re-initialised as often as needed.
     * Having played around with blitz’s Array::resize() will lead to serious issues like breaking
     * or obscure and hard-to-find data corruption!
     */
    double* latticeDataPointer[Components::number_of_Variables];

    void adaptParameters() {}
    std::list<Parameter<double>*> modelParameters_;
    std::list<Parameter<double>*> modelAdaptationParameters_;

    /*
     * Gibt den 4er-Laplace an \a position in Komponente \a component zurück.
     */
    double laplacian4(uint component, int position, bool periodic) { return 0; }
    /*
     * Gibt den 8er-Laplace an \a position in Komponente \a component zurück.
     */
    double laplacian8(uint component, int position, bool periodic) { return 0; }

    /*
     * Gibt den X-Gradienten an \a position in Komponente \a component zurück.
     */
    double gradientX(uint component, int position, bool periodic) { return 0; }
    /*
     * Gibt den Y-Gradienten an \a position in Komponente \a component zurück.
     */
    double gradientY(uint component, int position, bool periodic) { return 0; }
};

template <typename T_model>
inline double Lattice<T_model>::time() const
{
    return time_;
}

template <typename T_model>
inline void Lattice<T_model>::setTime(double t)
{
    time_ = t;
}

#include "lattice.cpp"

#endif
