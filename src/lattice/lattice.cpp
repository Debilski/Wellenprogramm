/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

#ifndef LATTICE_CPP
#define LATTICE_CPP

#ifndef RDS_LATTICE_H_
#error must be included via rds_lattice.h
#endif

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

/**
 * Central Lattice class
 *
 * The class greatly relies on blitz++ as an underlying structure. By use of
 * expression templates blitz++ is able to generate fast code through templates.
 *
 * http://ubiety.uwaterloo.ca/~tveldhui/papers/Expression-Templates/exprtmpl.html
 *
 */
template <typename T_model>
Lattice<T_model>::Lattice() : tau(0.001), time_(0), numSteps_(0), lastClusterCountTime_(-1), noiseIntensity_(0), noiseCorrelation_(-1), componentHasPlan(false)
{
    Components comp = Components();
    comp.components = 0;
    setDiffusion(comp);

    componentInfos.resize(Components::number_of_Variables);
}


/**
 * Räumt die Pläne auf und entfernt die Referenzen zum Rauschgenerator.
 */
template <typename T_model>
Lattice<T_model>::~Lattice()
{
    std::cout << "Destroy Lattice... " << std::flush;
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        destroyFftwPlanForComponent(i);
    }
    std::cout << "done\n" << std::flush;

    for (uint noiseComponent = 0; noiseComponent < number_of_Noise_Variables; ++noiseComponent) {
        delete originalNoiseGenerator_[noiseComponent];
    }
    fftw3Wrapper->release();
}

template <typename T_model>
void Lattice<T_model>::init(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY)
{
    setGeometry(sizeX, sizeY, latticeSizeX, latticeSizeY);
    clusterCounter_.init(sizeX, sizeY, latticeSizeX, latticeSizeY);

    std::cout << sizeX << "x" << sizeY << " / " << latticeSizeX << "x" << latticeSizeY << std::endl;

    for (uint noiseComponent = 0; noiseComponent < number_of_Noise_Variables; ++noiseComponent) {
        noiseGenerator_[noiseComponent] = new NoiseLattice(
            sizeX, sizeY, latticeSizeX, latticeSizeY);
        originalNoiseGenerator_[noiseComponent] = noiseGenerator_[noiseComponent];
    }

    // Sets threshold here and in cluster counter
    setExcitationThreshold(0.8);

    setBoundaryCondition(FixedBoundary);

    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice[i].resize(latticeSizeX, latticeSizeY);
        latticeFft[i].resize(latticeSizeX, latticeSizeY / 2 + 1);

        latticeFftOutput[i].resize(latticeSizeX, latticeSizeY / 2 + 1);
        precomputedDiffusionOperator[i].resize(latticeSizeX, latticeSizeY / 2 + 1);
    }

    latticePointHasReaction_.resize(latticeSizeX, latticeSizeY);
    latticePointHasReaction_ = true;

    fftw3Wrapper = Fftw3Wrapper::instance();
    fftw3Wrapper->importWisdom();

    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        createFftwPlanForComponent(i);
    }

    fftw3Wrapper->exportWisdom();

    // Works as long as fixpoint is not virtual!
    clear();
}


/**
 * Erzeugt einen Plan für die FFTW in einer bestimmten Komponente.
 *
 * Ist schon ein Plan vorhanden, so passiert nichts. Um einen Plan auf jeden Fall neu zu erstellen,
 * muss vorher destroyFftwPlanForComponent aufgerufen werden.
 */
template <typename T_model>
void Lattice<T_model>::createFftwPlanForComponent(uint component)
{
    if (componentHasPlan[component])
        return;

    forwardPlan[component] = Fftw3Wrapper::blitzFftwPlan_dft_r2c_2d(
        lattice[component], latticeFft[component]);
    backwardPlan[component] = Fftw3Wrapper::blitzFftwPlan_dft_c2r_2d(
        latticeFft[component], lattice[component]);
    componentHasPlan[component] = true;
}

/**
 * Zerstört einen Plan für die FFTW in einer bestimmten Komponente (falls vorhanden).
 */
template <typename T_model>
void Lattice<T_model>::destroyFftwPlanForComponent(uint component)
{
    if (!componentHasPlan[component])
        return;
    fftw_destroy_plan(forwardPlan[component]);
    fftw_destroy_plan(backwardPlan[component]);
    componentHasPlan[component] = false;
}

/**
 * Sets the system to the current fixpoint and might do some other initialisations
 * and resets in future.
 */
template <typename T_model>
void Lattice<T_model>::clear()
{
    toFixpoint();
}

/**
 * Sets the system to zero
 */
template <typename T_model>
void Lattice<T_model>::toZero()
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        lattice[component] = 0;
    }
}

/**
 * Sets the system to the current fixpoint.
 */
template <typename T_model>
void Lattice<T_model>::toFixpoint()
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        // Thanks to blitz++ this sets all to fixpoint[i]
        for (int i = 0; i < latticeSize(); ++i)
            lattice[component].data()[i] = fixpoint(i)[component];
    }
}

/**
 * Sets the system to the current fixpoint.
 */
template <typename T_model>
void Lattice<T_model>::toInitial(int)
{
    toFixpoint();
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::fixpoint(long int position) const
{
    return static_cast<const T_model*>(this)->fixpoint(position);
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::getAt(int x, int y) const
{
    Components c;
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        c[component] = lattice[component](x, y);
    }
    return c;
}

template <typename T_model>
inline double Lattice<T_model>::getComponentAt(uint component, int x, int y) const
{
    return lattice[component](x, y);
}

template <typename T_model>
inline std::complex<double> Lattice<T_model>::getFftComponentAt(uint component, int x, int y) const
{
    return latticeFftOutput[component](x, y);
}


template <typename T_model>
void Lattice<T_model>::setAt(const Components& m, int x, int y)
{
    for (uint i = 0; i < Components::number_of_Variables; ++i) {
        lattice[i](x, y) = m.components[i];
    }
}

template <typename T_model>
void Lattice<T_model>::setAtReal(const Components& m, int x, int y)
{
    //cout << scaleX <<flush;
    for (int i = 0; i < 1. / scaleX(); ++i) {
        for (int j = 0; j < 1. / scaleY(); ++j) {

            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                lattice[component]((int)(x / scaleX() + i), (int)(y / scaleY() + j))
                    = m[component];
            }
        }
    }
}

template <typename T_model>
void Lattice<T_model>::setComponentAt(uint component, double value, int x, int y)
{
    lattice[component](x, y) = value;
}

template <typename T_model>
blitz::Array<double, 2> Lattice<T_model>::getRawComponent(uint component) const
{
    return lattice[component].copy();
}

template <typename T_model>
bool Lattice<T_model>::setRawComponent(uint component, const blitz::Array<double, 2>& raw)
{
    if (!(raw.extent(blitz::firstDim) == latticeSizeX() && raw.extent(blitz::secondDim) == latticeSizeY()))
        return false;

    // Der Blitz-Assignment-Operator kopiert hoffentlich nur und erzeugt keine Referenz…
    lattice[component] = raw;

    // Sicherheitshalber fftw neu initialisieren, falls vorher Plan vorhanden.
    if (componentHasPlan[component]) {
        destroyFftwPlanForComponent(component);
        createFftwPlanForComponent(component);
    }
    return true;
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::getMax() const
{
    Components c;
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        c[component] =  //blitz::max( lattice[ component ] );
            Lattice<T_model>::getMax(component);
    }
    return c;
}

template <typename T_model>
double Lattice<T_model>::getMax(uint component) const
{
    return blitz::max(lattice[component]);
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::getMin() const
{
    Components c;
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        c[component] = Lattice<T_model>::getMax(component);
    }
    return c;
}

template <typename T_model>
double Lattice<T_model>::getMin(uint component) const
{
    return blitz::min(lattice[component]);
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::sumAll() const
{
    Components c;
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        c[component] = blitz::sum(lattice[component]);
    }
    return c;
}

template <typename T_model>
double Lattice<T_model>::numberLargerThanThreshold(double threshold) const
{
    return blitz::sum(blitz::where(lattice[0] > threshold, 1.0, 0.0)) * scaleX() * scaleY();
}

template <typename T_model>
double Lattice<T_model>::currentWavesize() const
{
    return numberLargerThanThreshold(excitationThresholdFirstComponent_);
}

/**
 * Prints some status information.
 *
 * \param o The @b std::ostream to output to.
 */
template <typename T_model>
void Lattice<T_model>::status(std::ostream& o)
{
    const char BREAKLN = '\n';
    o << "Information for Lattice of Type “" << modelName_ << "”:" << BREAKLN
      << "Integrator used:      “" << INTEGRATOR_NAME << "”" << BREAKLN
      << "Size:                 " << sizeX() << " x " << sizeY() << BREAKLN
      << "Lattice Size:         " << latticeSizeX() << " x " << latticeSizeY() << " = " << latticeSize() << " Lattice Points" << BREAKLN
      << "Elapsed time:         " << time() << BREAKLN << "Simulated steps:      " << numSteps_
      << BREAKLN << "Time Step:            " << tau << BREAKLN << "Suggested Time Step:  "
      << suggestedTimeStep() << BREAKLN << "Number of Components: "
      << Components::number_of_Variables << std::endl;

    for (uint i = 0; i < componentInfos.size(); ++i)
        o << "    " << componentInfos[i].name() << " (" << componentInfos[i].shortName() << ")"
          << BREAKLN;

    std::list<Parameter<double>*> p = parameters();
    o << "Parameters:           " << p.size() << std::endl;
    typedef std::list<Parameter<double>*>::const_iterator T_iter;
    for (T_iter it = p.begin(); it != p.end(); ++it)
        o << "    " << (*it)->name << ": " << (*it)->get() << BREAKLN;

    o << "Diffusion:            " << BREAKLN;
    for (uint i = 0; i < componentInfos.size(); ++i)
        o << "    " << componentInfos[i].shortName() << ": " << diffusion_[i] << BREAKLN;
    o << "Threshold:            " << excitationThreshold() << BREAKLN << "Current Wavesize:     "
      << currentWavesize() << BREAKLN;
    o << "Number of Clusters:   " << numberOfClusters() << BREAKLN;
    for (uint noiseComponent = 0; noiseComponent < number_of_Noise_Variables; ++noiseComponent) {
        o << "Noise Model:          "
          << (noiseGenerator_[noiseComponent] ? noiseGenerator_[noiseComponent]->getModelName()
                                              : "none")
          << BREAKLN;
        o << "Noide Intensity:      " << noiseIntensity() << BREAKLN;
    }
    o << std::flush;
}

/**
 * Erhöht die Zeit um einen \a stepSize entsprechende Wert.
 */
template <typename T_model>
void Lattice<T_model>::advanceTime(DiffusionStepWidth stepSize)
{
    switch (stepSize) {
        case IgnoreStep:
            return;
        //     cout << "zero";
        case HalfStep:
            time_ = time_ + tau / 2.0;
            break;
        case WholeStep:
            time_ = time_ + tau;
            break;
    }
}

/**
 * Führt die Diffusoin um eine Schrittweite von \a stepSize durch.
 *
 * \param stepSize Die Größe des Zeitschrittes, um die die Diffusion ausgeführt werden soll
 * \param advanceInTime Soll die Zeit ebenfalls erhöht werden?
 */
template <typename T_model>
void Lattice<T_model>::executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime /*= true*/)
{
    // Diffusion
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (diffusion_[component] == 0.0)
            continue;
        fftw_execute(forwardPlan[component]);

        latticeFftOutput[component] = latticeFft[component];

        if (stepSize == HalfStep) {
            latticeFft[component] = latticeFft[component]
                * precomputedDiffusionOperator[component];
        } else if (stepSize == WholeStep) {
            latticeFft[component] = latticeFft[component]
                * precomputedDiffusionOperator[component]
                * precomputedDiffusionOperator[component] * (double)latticeSize();
        }

        fftw_execute(backwardPlan[component]);
    }

    // Zeit erhöhen
    if (advanceInTime)
        advanceTime(stepSize);
}

template <typename T_model>
double Lattice<T_model>::getDiffusion(uint component) const
{
    return diffusion_[component];
}

template <typename T_model>
typename Lattice<T_model>::Components Lattice<T_model>::getDiffusion() const
{
    return Components(diffusion_);
}

/**
 * Setzt den Wert für die Diffusion in Komponente \a component auf \a value und
 * berechnet den Diffusionsoperator anschließend neu.
 */
template <typename T_model>
void Lattice<T_model>::setDiffusion(uint component, double value)
{
    std::cout << "Diffusion" << component << " set to " << value << std::endl;
    diffusion_[component] = value;

    computeDiffusionOperator(component);
}

template <typename T_model>
void Lattice<T_model>::setDiffusion(const Components& diff_components)
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        setDiffusion(component, diff_components[component]);
    }
}

/**
 * Berechnet den Diffusionsoperator für die @b halbe Schrittweite in @b allen Komponenten
 */
template <typename T_model>
void Lattice<T_model>::computeDiffusionOperator()
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        computeDiffusionOperator(component);
    }
}

/**
 * Berechnet den Diffusionsoperator für die @b halbe Schrittweite
 *
 * \param component Die Komponente, für die der Operator berechnet werden soll
 */
template <typename T_model>
void Lattice<T_model>::computeDiffusionOperator(uint component)
{
    int max_x_out = (latticeSizeX());
    int max_y_out = (latticeSizeY()) / 2 + 1;
    static const long double SQRT_M_PIl_POW_3l = pow(sqrt(M_PIl * 2.), 3);
    for (int i = 0; i < max_x_out; ++i) {
        for (int j = 0; j < max_y_out; ++j) {
            long double f_x = (i < latticeSizeX() / 2) ? i : latticeSizeX() - i;
            long double f_y = j;
            f_x = f_x * SQRT_M_PIl_POW_3l / sizeX();
            f_y = f_y * SQRT_M_PIl_POW_3l / sizeY();

            std::complex<double> multiplier(-f_x * f_x - f_y * f_y);
            precomputedDiffusionOperator[component](i, j) = (exp(multiplier
                                                                 * diffusion_[component] * tau / 2.)
                / static_cast<std::complex<double>>(latticeSize()));
        }
    }
}


/*
 * Defekte und Randbedingungen.
 *
 * Dieser Code sollte nochmal entrümpelt und verbessert werden.
 */


template <typename T_model>
BoundaryCondition Lattice<T_model>::boundaryCondition() const
{
    return boundaryCondition_;
}

template <typename T_model>
void Lattice<T_model>::setBoundaryCondition(BoundaryCondition bc)
{
    boundaryCondition_ = bc;
}

/**
 * Kümmert sich um das Setzen der Randbedingungen.
 * Da die Implementierung der Diffusion im Fourier-Raum stattfindet, ist es schwierig, die Randbedingungen „hart“ einzuhalten.
 * Um dennoch genau zu sein, muss die Diffusionslänge also kleiner als der Rand sein. Für feste Randbedingungen wird ein Rand
 * von 1 LE eingebaut; No-Flux-Randbedingungen haben feste Randbedingungen als Sicherheit und davor 1 LE reflektierende Randbedingungen.
 */
template <typename T_model>
void Lattice<T_model>::setBoundary()
{
    if (boundaryCondition_ == PeriodicBoundary)
        return;
    double horizontal[Components::number_of_Variables];
    double vertical[Components::number_of_Variables];

    /* Absorbing & NoFlux */
    if (boundaryCondition_ == FixedBoundary || boundaryCondition_ == NoFluxBoundary) {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            horizontal[component] = fixpoint()[component];
            vertical[component] = fixpoint()[component];
        }
    } else
        return;

    double widthX = 1 / scaleX();
    double widthY = 1 / scaleY();

    for (int y = 0; y < widthX; ++y) {
        for (int x = 0; x < latticeSizeX(); ++x) {

            /* NoFlux */
            //u_1 = u_lattice[index(s, scaleY + 1)];
            //u_2 = u_lattice[index(s, lattice_sizeY - scaleY - 1)];
            //v_1 = v_lattice[index(s, scaleY + 1)];
            //v_2 = v_lattice[index(s, lattice_sizeY - scaleY - 1)];
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                lattice[component](x, y) = fixpoint(index(x, y))[component];  //horizontal[ component ];
                lattice[component](x, (latticeSizeY() - 1) - y) = fixpoint(index(
                    x, latticeSizeY() - 1 - y))[component];  //horizontal[ component ];
            }
        }
    }

    for (int x = 0; x < widthX; ++x) {
        for (int y = 0; y < latticeSizeY(); y++) {
            /* NoFlux */
            //u_1 = u_lattice[index(scaleX + 1, s)];
            //u_2 = u_lattice[index(lattice_sizeX - scaleX - 1, s)];
            //v_1 = v_lattice[index(scaleX + 1, s)];
            //v_2 = v_lattice[index(lattice_sizeX - scaleX - 1, s)];
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                lattice[component](x, y) = fixpoint(index(x, y))[component];  // vertical[ component ];
                lattice[component]((latticeSizeX() - 1) - x, y) = fixpoint(index(
                    latticeSizeX() - 1 - x, y))[component];  // vertical[ component ];
            }
        }
    }

    int innerWidthX = (int)ceil(widthX);
    int innerWidthY = (int)ceil(widthY);

    if (boundaryCondition_ == NoFluxBoundary) {
        for (int x = innerWidthX; x < innerWidthX + widthX; ++x) {
            for (int y = innerWidthY; y < latticeSizeY() - innerWidthY; ++y) {

                for (uint component = 0; component < Components::number_of_Variables; ++component) {
                    lattice[component](x, y) = lattice[component](innerWidthX + (int)ceil(
                                                                                    widthX),
                        y);
                    lattice[component](latticeSizeX() - 1 - x, y) = lattice[component](
                        latticeSizeX() - 1 - (innerWidthX + (int)ceil(widthX)), y);
                }
            }
        }

        for (int y = innerWidthY; y < innerWidthY + widthY; ++y) {
            for (int x = innerWidthX; x < latticeSizeX() - innerWidthX; ++x) {

                for (uint component = 0; component < Components::number_of_Variables; ++component) {
                    lattice[component](x, y) = lattice[component](x, innerWidthY
                            + (int)ceil(widthY));
                    lattice[component](x, latticeSizeY() - 1 - y) = lattice[component](
                        x, latticeSizeY() - 1 - (innerWidthY + (int)ceil(widthY)));
                }
            }
        }

        /*          if ( boundaryCondition_ == NoFluxBoundary ) {
         lattice[ component ](x, y) = lattice[ component ](x, boxesPerLengthY_ + 1);
         lattice[ component ](x, latticeSizeY() - 1 - y) = lattice[ component ](x, latticeSizeY() - 1 - boxesPerLengthY_);
         continue;
         }*/
    }
}

template <typename T_model>
void Lattice<T_model>::applyDefects()
{
    for (uint i = 0; i < defects_.size(); ++i) {
        applyDefect(defects_[i]);
    }
    applyNoFluxMap();
}

template <typename T_model>
void Lattice<T_model>::applyDefect(Defect<Components> defect)
{
    if (defect.boundaryCondition == FixedValueBoundary) {
        setSpotAt(defect.centre.x, defect.centre.y, defect.radius, defect.value, true /*round*/);
    } else {
        setSpotAt(defect.centre.x, defect.centre.y, defect.radius, defect.boundaryCondition);
    }
}

template <typename T_model>
void Lattice<T_model>::addDefect(Defect<Components> defect)
{
    if (defect.boundaryCondition == PeriodicBoundary) {
        for (int i = -(int)floor(defect.radius / scaleX() + 0.5); i <= (int)floor(
                                                                           defect.radius / scaleX() + 0.5);
             ++i) {
            for (int j = -(int)floor(defect.radius / scaleY() + 0.5); j <= (int)floor(
                                                                               defect.radius / scaleY() + 0.5);
                 ++j) {
                //      if (j>=0 || atan(i/j) > -0.5)
                if (i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY() <= defect.radius
                        * defect.radius) {
                    int xval = i + (int)(defect.centre.x / scaleX());
                    int yval = j + (int)(defect.centre.y / scaleY());
                    latticePointHasReaction_.data()[indexPeriodic(xval, yval)] = true;
                }
            }
        }
        return;
    }
    if (defect.boundaryCondition == NoReactionBoundary) {
        if (defect.polygon.size() == 0) {

            for (int i = -(int)floor(defect.radius / scaleX() + 0.5); i <= (int)floor(
                                                                               defect.radius / scaleX() + 0.5);
                 ++i) {
                for (int j = -(int)floor(defect.radius / scaleY() + 0.5); j <= (int)floor(
                                                                                   defect.radius / scaleY() + 0.5);
                     ++j) {
                    //      if (j>=0 || atan(i/j) > -0.5)
                    if (i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY() <= defect.radius
                            * defect.radius) {
                        int xval = i + (int)(defect.centre.x / scaleX());
                        int yval = j + (int)(defect.centre.y / scaleY());
                        latticePointHasReaction_.data()[indexPeriodic(xval, yval)] = false;
                    }
                }
            }
            return;
        } else {
            LatticeIterator it(this, defect.polygon);
            while (!it.isDone()) {
                ++it;
                latticePointHasReaction_.data()[it.index()] = false;
            }

            return;
        }
    }
    if (defect.boundaryCondition == NoFluxBoundary) {
        std::vector<LatticePoint> mirroredPoints;
        std::vector<LatticePoint> outerPoints;

        double mirroredRadius = defect.radius + 3;
        double innerRadius = defect.radius - 3;  //radius * 0.5;
        if (innerRadius < 0)
            innerRadius = 0;

        for (int i = -(int)floor(mirroredRadius / scaleX() + 0.5); i <= (int)floor(
                                                                            mirroredRadius / scaleX() + 0.5);
             ++i) {
            for (int j = -(int)floor(mirroredRadius / scaleY() + 0.5); j <= (int)floor(
                                                                                mirroredRadius / scaleY() + 0.5);
                 ++j) {
                //      if (j>=0 || atan(i/j) > -0.5)
                double distance = sqrt(i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY());
                if (distance <= mirroredRadius) {
                    int xval = i + (int)(defect.centre.x / scaleX());
                    int yval = j + (int)(defect.centre.y / scaleY());
                    if (distance <= innerRadius) {
                    } else if (distance <= defect.radius) {
                        outerPoints.push_back(LatticePoint(xval, yval));
                    } else {
                        mirroredPoints.push_back(LatticePoint(xval, yval));
                    }
                }
            }
        }
        for (uint i = 0; i < outerPoints.size(); ++i) {
            LatticePoint mirroredPoint;
            double distance = -1;
            for (uint j = 0; j < mirroredPoints.size(); ++j) {
                double d = latticeDistance(outerPoints[i], mirroredPoints[j]);
                if (distance == -1)
                    distance = d;
                if (distance >= d) {
                    mirroredPoint = mirroredPoints[j];
                    distance = d;
                }
            }
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                noFluxCorrespondenceMap_[indexPeriodic(outerPoints[i].x, outerPoints[i].y)]
                    = indexPeriodic(mirroredPoint.x, mirroredPoint.y);
            }
        }
        return;
    }
    defects_.push_back(defect);
}

template <typename T_model>
void Lattice<T_model>::removeDefects()
{
    latticePointHasReaction_ = true;
    defects_.clear();
    noFluxCorrespondenceMap_.clear();
    std::cout << defects_.size();
}

template <typename T_model>
void Lattice<T_model>::applyNoFluxMap()
{
    if (noFluxCorrespondenceMap_.empty())
        return;
    std::map<int, int>::iterator it;
    for (it = noFluxCorrespondenceMap_.begin(); it != noFluxCorrespondenceMap_.end(); ++it) {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            lattice[component].data()[(*it).first] = lattice[component].data()[(*it).second];
        }
    }
}

template <typename T_model>
double Lattice<T_model>::setSpotAt(double x, double y, double radius, BoundaryCondition bc)
{
    if (bc == FixedBoundary) {
        return setSpotAt(x, y, radius, fixpoint(), true);
    }

    int numPointsSet = 0;
    if (radius == 0)
        return 0;

    std::vector<LatticePoint> mirroredPoints;
    std::vector<LatticePoint> outerPoints;

    /* Absorbing & NoFlux */
    double mirroredRadius = radius + 3;
    double innerRadius = radius - 3;  //radius * 0.5;
    if (innerRadius < 0)
        innerRadius = 0;
    if (bc == NoFluxBoundary) {
        for (int i = -(int)floor(mirroredRadius / scaleX() + 0.5); i <= (int)floor(
                                                                            mirroredRadius / scaleX() + 0.5);
             ++i) {
            for (int j = -(int)floor(mirroredRadius / scaleY() + 0.5); j <= (int)floor(
                                                                                mirroredRadius / scaleY() + 0.5);
                 ++j) {
                //      if (j>=0 || atan(i/j) > -0.5)
                double distance = sqrt(i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY());
                if (distance <= mirroredRadius) {
                    int xval = i + (int)(x / scaleX());
                    int yval = j + (int)(y / scaleY());

                    if (bc == NoFluxBoundary) {
                        if (distance <= innerRadius) {
                            setAt(fixpoint(), xval, yval);
                            ++numPointsSet;
                        } else if (distance <= radius) {
                            outerPoints.push_back(LatticePoint(xval, yval));
                            ++numPointsSet;
                        } else {
                            mirroredPoints.push_back(LatticePoint(xval, yval));
                        }
                    } else {
                        for (uint component = 0; component < Components::number_of_Variables; ++component) {
                            lattice[component].data()[indexPeriodic(xval, yval)]
                                = fixpoint()[component];
                        }
                    }
                }
            }
        }
    }

    if (bc == NoFluxBoundary) {
        for (uint i = 0; i < outerPoints.size(); ++i) {
            LatticePoint mirroredPoint;
            double distance = -1;
            for (uint j = 0; j < mirroredPoints.size(); ++j) {
                double d = latticeDistance(outerPoints[i], mirroredPoints[j]);
                if (distance == -1)
                    distance = d;
                if (distance >= d) {
                    mirroredPoint = mirroredPoints[j];
                    distance = d;
                }
            }
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                lattice[component](outerPoints[i].x, outerPoints[i].y)
                    = lattice[component](mirroredPoint.x, mirroredPoint.y);
            }
        }
    }
    return numPointsSet;
}

template <typename T_model>
void Lattice<T_model>::setSpotAtComponent(double x, double y, double size, double value,
    uint component, bool round)
{
    int numPointsSet = 0;
    if (size == 0)
        return;

    for (int i = -(int)floor(size / scaleX() + 0.5); i <= (int)floor(size / scaleX() + 0.5); ++i) {
        for (int j = -(int)floor(size / scaleY() + 0.5); j
             <= (int)floor(size / scaleY() + 0.5);
             ++j) {
            //      if (j>=0 || atan(i/j) > -0.5)
            if (!round || i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY() <= size * size) {
                int xval = i + (int)(x / scaleX());
                int yval = j + (int)(y / scaleY());
                lattice[component].data()[indexPeriodic(xval, yval)] = value;
                ++numPointsSet;
            }
        }
    }
}

template <typename T_model>
double Lattice<T_model>::setSpotAt(double x, double y, double size, const Components& c,
    bool round)
{
    int numPointsSet = 0;
    if (size == 0)
        return 0.0;

    for (int i = -(int)floor(size / scaleX() + 0.5); i <= (int)floor(size / scaleX() + 0.5); ++i) {
        for (int j = -(int)floor(size / scaleY() + 0.5); j
             <= (int)floor(size / scaleY() + 0.5);
             ++j) {
            //      if (j>=0 || atan(i/j) > -0.5)
            if (!round || i * i * scaleX() * scaleX() + j * j * scaleY() * scaleY() <= size * size) {
                int xval = i + (int)(x / scaleX());
                int yval = j + (int)(y / scaleY());
                for (uint component = 0; component < Components::number_of_Variables; ++component) {
                    lattice[component].data()[indexPeriodic(xval, yval)] = c[component];
                }
                ++numPointsSet;
            }
        }
    }
    return numPointsSet;
}

#define EVALFUNC(i, j, c) (lattice[component](i, j) - (component == 0 ? 0.5 : (0.5 * 0.75 - 0.06)))

template <typename T_model>
std::vector<SurfacePoint> Lattice<T_model>::getSpiralTips()
{
    std::vector<SurfacePoint> tips;

    // Following an algorithm by Barkley,
    // a tip is where both a function f1(x,y) and a function f2(x,y) are zero
    uint ind;
    // erstmal ohne Rand
    for (int i = 0; i < latticeSizeX() - 1; ++i) {
        for (int j = 0; j < latticeSizeY() - 1; ++j) {
            bool try_ = true;
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                if (!try_)
                    break;
                ind = ((EVALFUNC(i, j, component) >= 0.)) | ((EVALFUNC(i + 1, j, component) >= 0.) << 1) | ((EVALFUNC(i, j + 1, component) >= 0.) << 2) | ((EVALFUNC(i + 1, j + 1, component) >= 0.) << 3);
                if (ind == 0 || ind == 15)
                    try_ = false;
            }
            if (!try_)
                continue;
            SurfacePoint p = indexToSurfacePoint(index(i, j));
            tips.push_back(p);
        }
    }
    return tips;
}

template <typename T_model>
std::vector<Cluster> Lattice<T_model>::getClusters()
{
    if (time() != lastClusterCountTime_) {

        /*for (int i = 0; i < latticeSize(); ++i) {
         //! Defaults to Component 0
         clusterCounter.clusterField.data()[ i ] = (lattice[ 0 ].data()[ i ]
         > clusterCounter.clusterThreshold) ? -1 : 0;
         }*/
        clusterCounter_.clusterField = blitz::where(lattice[CLUSTER_COUNTER_LATTICE]
                > clusterCounter_.clusterThreshold(),
            -1, 0);

        clusterCounter_.countClusters();
        lastClusterCountTime_ = time();
        // cout << lastClusterCountTime << " ";
    }
    //   std::cout << std::endl;
    //   clusterCounter.printClusters();
    //   std::cout << std::endl;
    return clusterCounter_.getClusters();
}

template <typename T_model>
int Lattice<T_model>::numberOfClusters()
{
    if (time() != lastClusterCountTime_) {
        /*for (int i = 0; i < latticeSize(); ++i) {
         clusterCounter.clusterField.data()[ i ] = (lattice[ 0 ].data()[ i ]
         > clusterCounter.clusterThreshold) ? -1 : 0;
         }*/
        clusterCounter_.clusterField = blitz::where(lattice[CLUSTER_COUNTER_LATTICE]
                > clusterCounter_.clusterThreshold(),
            -1, 0);
        clusterCounter_.countClusters();
        lastClusterCountTime_ = time();
        //cout << lastClusterCountTime_ << "(" << clusterCounter.numClusters() <<  ")" << " ";
    }
    return clusterCounter_.numberOfClusters();
}

template <typename T_model>
int Lattice<T_model>::noiseCorrelation()
{
    return noiseCorrelation_;
}

template <typename T_model>
void Lattice<T_model>::setNoiseCorrelation(int i)
{
    noiseCorrelation_ = i;
}

template <typename T_model>
void Lattice<T_model>::setNoiseGenerator(uint noiseComponent, NoiseGenerator* ng)
{
    noiseGenerator_[noiseComponent] = ng;
}

template <typename T_model>
const NoiseGenerator* Lattice<T_model>::getNoiseGenerator(uint noiseComponent) const
{
    return noiseGenerator_[noiseComponent];
}

template <typename T_model>
double Lattice<T_model>::noiseIntensity()
{
    return noiseIntensity_;
}

template <typename T_model>
void Lattice<T_model>::setNoiseIntensity(double d)
{
    noiseIntensity_ = d;
}

template <typename T_model>
void Lattice<T_model>::doNormalize()
{
}

template <typename T_model>
void Lattice<T_model>::precomputeNoise(Lattice<T_model>* lattice)
{
    for (uint noiseComponent = 0; noiseComponent < number_of_Noise_Variables; ++noiseComponent) {
        lattice->noiseGenerator_[noiseComponent]->precomputeNoise(
            lattice->noiseCorrelation_, lattice->noiseIntensity_);
    }
}

template <typename T_model>
void Lattice<T_model>::dump(std::ostream& stream, int every /*= 1*/, bool isBinary /*= false*/) const
{
    if (isBinary == true) {
        for (int i = 0; i < latticeSizeX(); i = i + every) {
            for (int j = 0; j < latticeSizeY(); j = j + every) {
                for (uint component = 0; component < Components::number_of_Variables; ++component) {
                    // change double to float…
                    double var;
                    var = static_cast<double>(lattice[component](i, j));
                    stream.write(reinterpret_cast<char*>(&var), sizeof var);
                }
            }
        }
    } else {
        for (int i = 0; i < latticeSizeX(); i = i + every) {
            for (int j = 0; j < latticeSizeY(); j = j + every) {
                for (uint component = 0; component < Components::number_of_Variables; ++component) {
                    stream << lattice[component](i, j) << "\t";
                }
            }
        }
        stream << std::endl;
    }
}

template <typename T_model>
void Lattice<T_model>::undump(std::istream& stream, int every /*= 1 */, bool isBinary /*= false*/)
{
    (void)every;
    int i = 0;
    double c;
    if (isBinary == true) {
        while (!stream.eof()) {
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                stream.read(reinterpret_cast<char*>(&c), sizeof c);
                lattice[component].data()[i] = c;
            }
            ++i;
        }
    } else {
        while (!stream.eof()) {
            for (uint component = 0; component < Components::number_of_Variables; ++component) {
                stream >> c;
                lattice[component].data()[i] = c;
            }
            ++i;
        }
    }
}
template <typename T_model>

void Lattice<T_model>::save(bool append /*=false */, std::string fileName /*= "tmpsave.bin" */) const
{
    (void)append;
    std::ofstream outfile;
    outfile.open(fileName.c_str(), std::ios::binary);
    dump(outfile, 1, true);
    outfile.close();
}

template <typename T_model>
void Lattice<T_model>::recall(std::string fileName /*= "tmpsave.bin" */)
{
    std::ifstream infile;
    infile.open(fileName.c_str(), std::ios::binary);
    undump(infile, 1, true);
    infile.close();
}

/**** Additional Functions *****/

template <typename T_model>
SurfacePoint Lattice<T_model>::centreOfMass(uint component) const
{
    double mass = 0;
    double mx = 0;
    double my = 0;
    //#pragma omp parallel for reduction(+: mass, mx, my)

    blitz::firstIndex i;
    blitz::secondIndex j;

    /* It is strange, but using rather complicated and seemingly redundant blitz++ expressions
     * generates faster code than doing this manually.
     *
     * virtual getFixpoint() is automatically cached by blitz++.
     */
    mx = sum((lattice[component] - getFixpoint(component)) * i) * scaleX();
    my = sum((lattice[component] - getFixpoint(component)) * j) * scaleY();
    mass = sum((lattice[component] - getFixpoint(component)));

    SurfacePoint s = {mx / mass, my / mass};
    //cout << "coM (" << mx/mass << "; " << my/mass << ") ";
    return s;
}

template <typename T_model>
SurfacePoint Lattice<T_model>::centreOfExcitation(uint component) const
{
    double mass = 0;
    double mx = 0;
    double my = 0;
    //#pragma omp parallel for reduction(+: mass, mx, my)

    blitz::firstIndex i;
    blitz::secondIndex j;

    mx = sum(where(lattice[component] > 1, 1, 0) * i) * scaleX();
    my = sum(where(lattice[component] > 1, 1, 0) * j) * scaleY();
    mass = sum(where(lattice[component] > 1, 1, 0));

    SurfacePoint s = {mx / mass, my / mass};
    //cout << "coE (" << mx/mass << "; " << my/mass << ") " << endl <<flush;
    return s;
}

/**
 * Gibt den empfohlenen Zeitschritt zurück. Wenn negativ, hat das Modell diese Methode nicht
 * implementiert.
 *
 * In lattice.cpp wird dies daher auf -1 festgelegt.
 */
template <typename T_model>
double Lattice<T_model>::suggestedTimeStep() const
{
    return -1.;
}

/**
 * Gibt zurück, ob das System noch intakt ist, oder ob bereits NaN-Werte vorhanden sind.
 */
template <typename T_model>
bool Lattice<T_model>::isAlive() const
{
    double d = 0;
    if (blitz::has_quiet_NaN<double>(d)) {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            if (blitz::max(lattice[component]) < blitz::min(lattice[component])
                || blitz::any(lattice[component] == blitz::quiet_NaN<double>(d))
                || blitz::any(lattice[component] >= blitz::huge<double>(d)) || blitz::any(
                                                                                   lattice[component] <= blitz::neghuge<double>(d)))
                return false;
        }
    } else {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            if (blitz::max(lattice[component]) < blitz::min(lattice[component])
                || blitz::any(lattice[component] >= blitz::huge<double>(d)) || blitz::any(
                                                                                   lattice[component] <= blitz::neghuge<double>(d)))
                return false;
        }
    }
    if (blitz::has_signalling_NaN<double>(d)) {
        for (uint component = 0; component < Components::number_of_Variables; ++component) {
            if (blitz::any(lattice[component] == blitz::signalling_NaN<double>(d)))
                return false;
        }
    }
#ifndef NO_IEEE_BLITZ_FUNCTIONS
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        if (blitz::any(blitz::blitz_isnan(lattice[component]) != 0))
            return false;
    }
#endif
    return true;
}

template <typename T_model>
inline void Lattice<T_model>::setExcitationThreshold(double threshold)
{
    clusterCounter_.setClusterThreshold(threshold);
    excitationThresholdFirstComponent_ = threshold;
}

template <typename T_model>
inline double Lattice<T_model>::excitationThreshold() const
{
    return excitationThresholdFirstComponent_;
}

template <typename T_model>
bool Lattice<T_model>::copyFromOtherReal(const LatticeInterface& other)
{
    if (sizeX() != other.sizeX() || sizeY() != other.sizeY())
        return false;
    // sizeX == o.sizeX; sizeY == o.sizeY
    unsigned long commonMultipleX = leastCommonMultiple(other.latticeSizeX(), latticeSizeX());
    unsigned long commonMultipleY = leastCommonMultiple(other.latticeSizeY(), latticeSizeY());
    std::cout << " M: " << commonMultipleX << std::flush;
    // Zahl der n�tigen Abtastungen von other
    double xCount = (double)commonMultipleX / latticeSizeX();
    double yCount = (double)commonMultipleY / latticeSizeY();
    double xCount_other = (double)commonMultipleX / other.latticeSizeX();
    double yCount_other = (double)commonMultipleY / other.latticeSizeY();
    std::cout << " " << xCount << "," << yCount << ";" << xCount_other << "," << yCount_other
              << std::endl;
    toZero();
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        for (unsigned long i = 0; i < commonMultipleX; ++i) {
            int i_l = (int)(i / xCount);
            int i_o = (int)(i / xCount_other);
            int skipI = std::min((int)xCount - (i % (int)xCount), (int)xCount_other - (i
                                                                                          % (int)xCount_other));
            for (unsigned long j = 0; j < commonMultipleY; ++j) {
                int j_l = (int)(j / yCount);
                int j_o = (int)(j / yCount_other);
                int skipJ = std::min((int)yCount - (j % (int)yCount), (int)yCount_other - (j
                                                                                              % (int)yCount_other));
                lattice[component](i_l, j_l) += other.getComponentAt(component, i_o, j_o)
                    * skipI * skipJ;
                j += (skipJ - 1);
            }
            i += (skipI - 1);
        }
        lattice[component] /= (xCount * yCount);
    }
    return true;
}

template <typename T_model>
bool Lattice<T_model>::copyFromOtherLattice(const LatticeInterface& other)
{
    if (latticeSizeX() != other.latticeSizeX() || latticeSizeY() != other.latticeSizeY())
        return false;
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        for (int i = 0; i < latticeSizeX(); ++i)
            for (int j = 0; j < latticeSizeY(); ++j)
                lattice[component](i, j) = (double)other.getComponentAt(component, i, j);
    }
    return false;
}

template <typename T_model>
bool Lattice<T_model>::insertOtherAt(const LatticeInterface& other, int x, int y)
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        for (int i = 0; i < other.latticeSizeX(); ++i)
            for (int j = 0; j < other.latticeSizeY(); ++j)
                lattice[component].data()[indexPeriodic(i + x, j + y)]
                    = (double)other.getComponentAt(component, i, j);
    }
    return true;
}

template <typename T_model>
bool Lattice<T_model>::copyFromOther(const LatticeInterface& other, int x, int y)
{
    for (uint component = 0; component < Components::number_of_Variables; ++component) {
        for (int i = 0; i < latticeSizeX(); ++i)
            for (int j = 0; j < latticeSizeY(); ++j) {
                // Zwischenspeichern für Periodik.
                int idx = other.indexPeriodic(i + x, j + y);
                lattice[component](i, j) = (double)other.getComponentAt(
                    component, other.indexToX(idx), other.indexToY(idx));
            }
    }
    return true;
}

#endif
