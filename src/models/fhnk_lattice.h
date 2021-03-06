/*
 * fhnk_lattice.h
 *
 *  Created on: 02.03.2009
 *      Author: rikebs
 */

#ifndef FHNK_LATTICE_H_
#define FHNK_LATTICE_H_

#include "../lattice/rds_lattice.h"
#include "../lattice/siip_lattice_integrator.h"

THREE_COMPONENT_SYSTEM(FhnKSystem, x, y, z)

class FhnKLattice4;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice4> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template <>
struct Metainfo<FhnKLattice4>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnKLattice4 : public SIIP_LatticeIntegrator<FhnKLattice4>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice4() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice 4er";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }
    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double z_sum = 0;
            int numNeighbours = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (!isFhn(indexToX(pos) + i, indexToY(pos) + j) && !(j == 0 && i == 0)
                        && (i * j == 0)) {
                        z_sum += lattice[2](indexToX(pos) + i, indexToY(pos) + j);
                        ++numNeighbours;
                    }
                }
            }

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * z_sum / numNeighbours);  //sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }
    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

    inline long int view(int position) const
    {
        long int r, g, b;
        if (isFhn(indexToX(position), indexToY(position))) {
            if (lattice[0].data()[position] < 0)
                r = g = b = 0;
            else
                r = g = b = 255;
        } else {
            g = b = 0;
            r = lattice[2].data()[position] * 2;
        }
        return r * 256 * 256 + g * 256 + b;
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                if (isFhn(x, y)) {
                    diffMatrix(x, y) = 0;
                    continue;
                }

                if (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1)) {
                    if (Base::boundaryCondition_ == PeriodicBoundary) {
                        diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                    }
                } else {
                    diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                }
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (isFhn(x + i, y + j) && !(j == 0 && i == 0) && !(x + i == 0) && !(x + i == 0)
                    && (i * j == 0)) {
                    ++numNeighbours;
                    res += Psi(lattice[0](
                        Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                               Base::indexPeriodic(x + i, y + j))));
                }
            }
        }
        return alpha() * res;  // / numNeighbours;;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (!isFhn(x + i, y + j) && !(j == 0 && i == 0) && (i * j == 0)) {
                    ++numNeighbours;
                    res += (lattice[2](
                               Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                                      Base::indexPeriodic(x + i, y + j))))
                        - lattice[2](x, y);
                }
            }
        }
        return gamma() * res;  // / numNeighbours;
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 2; i < latticeSizeX() - 3; ++i) {
            for (int j = 2; j < latticeSizeY() - 3; ++j) {
                if (number == 1) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
                if (number == 2) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 4) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }

    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y) const
    {
        return isFhnField(x, y);
    }
};

class FhnKLattice8;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice8> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template <>
struct Metainfo<FhnKLattice8>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnKLattice8 : public SIIP_LatticeIntegrator<FhnKLattice8>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice8() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice 8er";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }
    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double z_sum = 0;
            int numNeighbours = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    if (!isFhn(indexToX(pos) + i, indexToY(pos) + j) && !(j == 0 && i == 0) /*&& (i*j==0)*/) {
                        z_sum += lattice[2](indexToX(pos) + i, indexToY(pos) + j);
                        ++numNeighbours;
                    }
                }
            }

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * z_sum / numNeighbours);  //sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }
    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                if (isFhn(x, y)) {
                    diffMatrix(x, y) = 0;
                    continue;
                }

                if (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1)) {
                    if (Base::boundaryCondition_ == PeriodicBoundary) {
                        diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                    }
                } else {
                    diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                }
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (isFhn(x + i, y + j) && !(j == 0 && i == 0) && !(x + i == 0) && !(x + i == 0) /*&& (i*j==0)*/) {
                    ++numNeighbours;
                    res += Psi(lattice[0](
                        Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                               Base::indexPeriodic(x + i, y + j))));
                }
            }
        }
        return alpha() * res;  // / numNeighbours;;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (!isFhn(x + i, y + j) && !(j == 0 && i == 0) /*&& (i*j==0)*/) {
                    ++numNeighbours;
                    res += (lattice[2](
                               Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                                      Base::indexPeriodic(x + i, y + j))))
                        - lattice[2](x, y);
                }
            }
        }
        return gamma() * res;  // / numNeighbours;
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }

                if (number == 1) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 2) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }

    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};

class FhnKLattice_z4;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice_z4> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template <>
struct Metainfo<FhnKLattice_z4>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnKLattice_z4 : public SIIP_LatticeIntegrator<FhnKLattice_z4>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice_z4() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice global z 4er";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }
    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }
    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {

                if (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1)) {
                    if (Base::boundaryCondition_ == PeriodicBoundary) {
                        diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                    }
                } else {
                    diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                }
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (isFhn(x + i, y + j)
                    /*&& !(j == 0 && i == 0) && !(x + i == 0) && !(x + i == 0)*/
                    && (i * j == 0)) {
                    ++numNeighbours;
                    res += Psi(lattice[0](
                        Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                               Base::indexPeriodic(x + i, y + j))));
                }
            }
        }
        return alpha() * res;  // / numNeighbours;;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                {
                    if (!isFhn(x + i, y + j) && !(j == 0 && i == 0) && (i * j == 0))
                        ++numNeighbours;
                    res += (lattice[2](
                               Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                                      Base::indexPeriodic(x + i, y + j))))
                        - lattice[2](x, y);
                }
            }
        }
        return gamma() * res;  // / numNeighbours;
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }

                if (number == 1) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 2) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }

    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};

class FhnKLattice_z8;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice_z8> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template <>
struct Metainfo<FhnKLattice_z8>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};

//META(FhnLattice, TwoComponentSystem)
class FhnKLattice_z8 : public SIIP_LatticeIntegrator<FhnKLattice_z8>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice_z8() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice global z 8er";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }
    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }
    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {

                if (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1)) {
                    if (Base::boundaryCondition_ == PeriodicBoundary) {
                        diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                    }
                } else {
                    diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
                }
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (isFhn(x + i, y + j) /*&& (i*j==0)*/) {
                    ++numNeighbours;
                    res += Psi(lattice[0](
                        Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                               Base::indexPeriodic(x + i, y + j))));
                }
            }
        }
        return alpha() * res;  // / numNeighbours;;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        int numNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (!isFhn(x + i, y + j) && !(j == 0 && i == 0) /*&& (i*j==0)*/) {
                    ++numNeighbours;
                    res += (lattice[2](
                               Base::indexToX(Base::indexPeriodic(x + i, y + j)), Base::indexToY(
                                                                                      Base::indexPeriodic(x + i, y + j))))
                        - lattice[2](x, y);
                }
            }
        }
        return gamma() * res;  // / numNeighbours;
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }

                if (number == 1) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 2) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }

    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};


class FhnKLattice_laplace;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice_laplace> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
    static const int CLUSTER_COUNTER_LATTICE = 2;
};
template <>
struct Metainfo<FhnKLattice_laplace>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};


//META(FhnLattice, TwoComponentSystem)
class FhnKLattice_laplace : public SIIP_LatticeIntegrator<FhnKLattice_laplace>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice_laplace() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice laplace";
        modelInformation_ = "FHN-K-Modell mit 8-er Nachbarschaft und Abstandsgewichtung.";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }

    inline void doNormalize()
    {

        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if (!isFhnField(i, j)) {
                    lattice[0](i, j) = fixpoint(-1).x();
                    lattice[1](i, j) = fixpoint(-1).y();
                } else {
                    lattice[2](i, j) = fixpoint(-1).z();
                }
            }
        }
    }

    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double z_sum = 0;
            double sumNeighbours = 0;
            for (int i = -1; i <= 1; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    int x = indexToX(pos);
                    int y = indexToY(pos);

                    int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                    int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                    // Nehme nur Nicht-Fhn-Stellen mit und schließe sich selbst aus
                    if (!isFhn(neighbourX, neighbourY) && !(j == 0 && i == 0)) {
                        // direkte Nachbarn
                        if (i * j == 0) {
                            z_sum += lattice[2](neighbourX, neighbourY);
                            sumNeighbours += 1.0;
                        }  // Eck-Nachbarn
                        else {
                            z_sum += lattice[2](neighbourX, neighbourY) / std::sqrt(2);
                            sumNeighbours += 1.0 / std::sqrt(2);
                        }
                    }
                }
            }

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * z_sum / sumNeighbours);  //sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }


    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                if (isFhn(x, y)) {
                    diffMatrix(x, y) = 0;
                    continue;
                }

                if ((!Base::boundaryCondition_ == PeriodicBoundary) &&

                    (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1))) {
                    continue;
                }

                diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // Nehme nur Fhn-Stellen
                if (isFhn(neighbourX, neighbourY)) {
                    // Wenn nächster = direkter Nachbar
                    if (i * j == 0) {
                        sumNeighbours += 1.0;
                        res += Psi(lattice[0](neighbourX, neighbourY));
                    }
                    // Wenn Ecknachbar
                    else {
                        sumNeighbours += 1.0 / std::sqrt(2);
                        res += Psi(lattice[0](neighbourX, neighbourY)) / std::sqrt(2);
                    }
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        return alpha() * res;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {

                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // Nehme nur Nicht-Fhn-Stellen mit und schließe sich selbst aus
                if (!isFhn(neighbourX, neighbourY) && !(j == 0 && i == 0)) {
                    {
                        // Wenn nächster = direkter Nachbar
                        if (i * j == 0) {
                            sumNeighbours += 1.0;
                            res += lattice[2](neighbourX, neighbourY);
                        }
                        // Wenn Ecknachbar
                        else {
                            sumNeighbours += 1.0 / std::sqrt(2);
                            res += lattice[2](neighbourX, neighbourY) / std::sqrt(2);
                        }
                    }
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        // Mittelpunkt abziehen
        res -= lattice[2](x, y);
        return res * gamma();
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 2; i < latticeSizeX() - 3; ++i) {
            for (int j = 2; j < latticeSizeY() - 3; ++j) {
                if (number == 1) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if (number == 5) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
                if (number == 2) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 4) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }


    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};


class FhnKLattice_laplace_extended;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice_laplace_extended> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
};
template <>
struct Metainfo<FhnKLattice_laplace_extended>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};


//META(FhnLattice, TwoComponentSystem)
class FhnKLattice_laplace_extended : public SIIP_LatticeIntegrator<FhnKLattice_laplace_extended>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l, max_distance;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice_laplace_extended() : epsilon(0.04, "epsilon"), a(1.04, "a"), C_z(0.1, "C_z"), tau_r(1.0, "tau_r"), x_0(0, "x0"), x_s(0.05, "xs"), alpha(11.0, "alpha"), beta(0.6, "beta"), gamma(0.2, "gamma"), tau_l(2.0, "tau_l"), max_distance(2.0, "max_distance")
    {
        modelName_ = "FhnKLattice laplace ex.";
        modelInformation_ = "FHN-K-Modell mit noch größerer Nachbarschaft und Abstandsgewichtung.";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
        registerParameter(&max_distance);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }

    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {

            double z_sum = 0;
            double sumNeighbours = 0;

            int distance_sum = std::ceil(max_distance);

            for (int i = -distance_sum; i <= distance_sum; ++i) {
                for (int j = -distance_sum; j <= distance_sum; ++j) {
                    int x = indexToX(pos);
                    int y = indexToY(pos);

                    int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                    int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                    // Nehme nur Nicht-Fhn-Stellen mit und schließe sich selbst aus
                    if (!isFhn(neighbourX, neighbourY) && !(j == 0 && i == 0)) {
                        double dist = std::sqrt(i * i + j * j);
                        if (dist >= max_distance) {
                            continue;
                        }
                        z_sum += lattice[2](neighbourX, neighbourY) / dist;
                        sumNeighbours += 1.0 / dist;
                    }
                }
            }

            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * z_sum / sumNeighbours);  //sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }


    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                if (isFhn(x, y)) {
                    diffMatrix(x, y) = 0;
                    continue;
                }

                if ((!Base::boundaryCondition_ == PeriodicBoundary) &&

                    (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1))) {
                    continue;
                }

                diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        int distance_sum = std::ceil(max_distance);

        for (int i = -distance_sum; i <= distance_sum; ++i) {
            for (int j = -distance_sum; j <= distance_sum; ++j) {
                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // Nehme nur Fhn-Stellen
                if (isFhn(neighbourX, neighbourY)) {
                    double dist = std::sqrt(i * i + j * j);
                    // Wenn nächster = direkter Nachbar
                    if (dist >= max_distance) {
                        continue;
                    }
                    sumNeighbours += 1.0 / dist;
                    res += Psi(lattice[0](neighbourX, neighbourY)) / dist;
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        return alpha() * res;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        int distance_sum = std::ceil(max_distance);

        for (int i = -distance_sum; i <= distance_sum; ++i) {
            for (int j = -distance_sum; j <= distance_sum; ++j) {

                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // Nehme nur Nicht-Fhn-Stellen mit und schließe sich selbst aus
                if (!isFhn(neighbourX, neighbourY) && !(j == 0 && i == 0)) {

                    double dist = std::sqrt(i * i + j * j);
                    if (dist >= max_distance) {
                        continue;
                    }
                    sumNeighbours += 1.0 / dist;
                    res += lattice[2](neighbourX, neighbourY) / dist;
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        // Mittelpunkt abziehen
        res -= lattice[2](x, y);
        return res * gamma();
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 2; i < latticeSizeX() - 3; ++i) {
            for (int j = 2; j < latticeSizeY() - 3; ++j) {
                if (number == 1) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if (number == 5) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
                if (number == 2) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 4) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }


    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};


class FhnKLattice_layers;
/**
 * Helper Class for additional but dependend Information
 */
template <>
struct Metainfo<FhnKLattice_layers> : MetainfoBase
{
    typedef FhnKSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
    static const bool OPTIMISE_NO_MULTIPLICATIVE_NOISE = true;
    static const bool OPTIMISE_NO_CLUSTER_COUNT = true;
    static const bool OPTIMISE_NO_EXTERNAL_FORCE = true;
    static const int CLUSTER_COUNTER_LATTICE = 2;
};
template <>
struct Metainfo<FhnKLattice_layers>::NoiseMapping<secondComponent>
{
    enum
    {
        value = firstComponent
    };
};


//META(FhnLattice, TwoComponentSystem)
class FhnKLattice_layers : public SIIP_LatticeIntegrator<FhnKLattice_layers>
{
public:
    Parameter<double> epsilon, a, C_z, tau_r, x_0, x_s, alpha, beta, gamma, tau_l;
    blitz::Array<bool, 2> isFhnField;
    FhnKLattice_layers() :

                           epsilon(0.04, "epsilon"),
                           a(1.04, "a"),
                           C_z(0.1, "C_z"),
                           tau_r(1.0, "tau_r"),
                           x_0(0, "x0"),
                           x_s(0.05, "xs"),
                           alpha(11.0, "alpha"),
                           beta(0.6, "beta"),
                           gamma(
                               0.2, "gamma"),
                           tau_l(2.0, "tau_l")
    {
        modelName_ = "FhnKLattice Layers";
        modelInformation_ = "FHN-K-Modell mit 8er Nachbarschaft und Abstandsgewichtung. Neuronen sind aufgelegt. Das z-Medium ist durchgängig.";

        componentInfos[0] = ComponentInfo("Aktivator", "x", -2.2, 2.5);
        componentInfos[1] = ComponentInfo("Inhibitor", "y", -2.2, 2.5);
        componentInfos[2] = ComponentInfo("Diffusiv", "z", 0, 8);

        registerParameter(&epsilon);
        registerParameter(&a);
        registerParameter(&C_z);
        registerParameter(&tau_r);
        registerParameter(&x_0);
        registerParameter(&x_s);
        registerParameter(&alpha);
        registerParameter(&beta);
        registerParameter(&gamma);
        registerParameter(&tau_l);
    }

    inline double Psi(double x)
    {
        return 0.5 * (1.0 + tanh(x / x_s()));
    }
    inline double tau(double x)
    {
        return tau_l() + (tau_r() - tau_l()) * Psi(x);
    }

    inline void doNormalize()
    {

        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if (!isFhnField(i, j)) {
                    lattice[0](i, j) = fixpoint(-1).x();
                    lattice[1](i, j) = fixpoint(-1).y();
                } else {
                    //                lattice[ 2 ](i,j) = fixpoint(-1).z();
                }
            }
        }
    }

    inline FhnKSystem step_f(FhnKSystem sys, long int pos)
    {
        if (isFhn(indexToX(pos), indexToY(pos))) {
            double x = 1. / epsilon() * (sys.x() - sys.x() * sys.x() * sys.x() / 3.0 - sys.y());
            double y = 1. / tau(sys.x()) * (sys.x() + a() - C_z() * sys.z());
            double z = sys.z();
            return FhnKSystem(x, y, 0);
        } else
            return FhnKSystem(0, 0, 0);
    }

    inline FhnKSystem fixpoint(int) const
    {
        return FhnKSystem(-1, 0, 0);
    }

protected:
    void executeDiffusion(DiffusionStepWidth stepSize, bool advanceInTime = true)
    {
        advanceTime(stepSize);

        double scaleFactor = 0;

        if (stepSize == Base::HalfStep) {
            scaleFactor = Base::tau / 2.;
        } else if (stepSize == Base::WholeStep) {
            scaleFactor = Base::tau;
        }

        /* Hack für reflektierende Randbedingungen */
        if (Base::boundaryCondition_ == NoFluxBoundary) {
            for (int x = 1; x < Base::latticeSizeX() - 1; ++x) {
                lattice[2](x, 0) = lattice[2](x, 1);
                lattice[2](x, Base::latticeSizeX() - 1) = lattice[2](x, Base::latticeSizeX()
                        - 2);
            }

            for (int y = 1; y < Base::latticeSizeY() - 1; ++y) {
                lattice[2](0, y) = lattice[2](1, y);
                lattice[2](Base::latticeSizeY() - 1, y) = lattice[2](Base::latticeSizeY()
                        - 2,
                    y);
            }
        }
        /* Hack-Ende */
        blitz::Array<double, 2> diffMatrix(Base::lattice[2].shape());
        diffMatrix = 0;
        for (int x = 0; x < Base::latticeSizeX(); ++x) {
            for (int y = 0; y < Base::latticeSizeY(); ++y) {
                /*if ( isFhn( x, y ) ) {
                    diffMatrix( x, y ) = 0;
                    continue;
                }*/

                if ((!Base::boundaryCondition_ == PeriodicBoundary) &&

                    (x == 0 || x == (Base::latticeSizeX() - 1) || y == 0 || y == (Base::latticeSizeY() - 1))) {
                    continue;
                }

                diffMatrix(x, y) += (nb1_periodic(x, y) + nb2_periodic(x, y) - beta() * lattice[2](x, y)) * scaleFactor;
            }
        }
        Base::lattice[2] += diffMatrix;
    }

    inline double nb1_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // Nehme nur Fhn-Stellen
                if (isFhn(neighbourX, neighbourY)) {
                    // Wenn nächster = direkter Nachbar
                    if (i * j == 0) {
                        sumNeighbours += 1.0;
                        res += Psi(lattice[0](neighbourX, neighbourY));
                    }
                    // Wenn Ecknachbar
                    else {
                        sumNeighbours += 1.0 / std::sqrt(2);
                        res += Psi(lattice[0](neighbourX, neighbourY)) / std::sqrt(2);
                    }
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        return alpha() * res;
    }

    inline double nb2_periodic(int x, int y)
    {
        double res = 0;
        double sumNeighbours = 0;
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {

                int neighbourX = Base::indexToX(Base::indexPeriodic(x + i, y + j));
                int neighbourY = Base::indexToY(Base::indexPeriodic(x + i, y + j));
                // /*Nehme nur Nicht-Fhn-Stellen mit und*/ schließe sich selbst aus
                if (/* !isFhn( neighbourX, neighbourY ) &&*/ !(j == 0 && i == 0)) {
                    {
                        // Wenn nächster = direkter Nachbar
                        if (i * j == 0) {
                            sumNeighbours += 1.0;
                            res += lattice[2](neighbourX, neighbourY);
                        }
                        // Wenn Ecknachbar
                        else {
                            sumNeighbours += 1.0 / std::sqrt(2);
                            res += lattice[2](neighbourX, neighbourY) / std::sqrt(2);
                        }
                    }
                }
            }
        }
        // Normieren
        if (sumNeighbours != 0) {
            res = res / sumNeighbours;
        }
        // Mittelpunkt abziehen
        res -= lattice[2](x, y);
        return res * gamma();
    }

    void toInitial(int number)
    {
        isFhnField.resize(latticeSizeX(), latticeSizeY());
        isFhnField = false;
        for (int i = 2; i < latticeSizeX() - 3; ++i) {
            for (int j = 2; j < latticeSizeY() - 3; ++j) {
                if (number == 1) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 0; i < latticeSizeX(); ++i) {
            for (int j = 0; j < latticeSizeY(); ++j) {
                if (number == 5) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
            }
        }
        for (int i = 5; i < latticeSizeX() - 6; ++i) {
            for (int j = 5; j < latticeSizeY() - 6; ++j) {

                if (number == 0) {
                    isFhnField(i, j) = (i % 2 == 0 && j % 2 == 0);
                }
                if (number == 2) {
                    isFhnField(i, j) = (rand() % 4 == 0);
                }

                if (number == 3) {
                    isFhnField(i, j) = (i % 3 == 0 && j % 3 == 0);
                }

                if (number == 4) {
                    isFhnField(i, j) = (i % 3 != 0 && j % 3 != 0);
                }
            }
        }
    }


    /**
     * Gibt aus, ob das System an dieser Stelle ein FHN ist oder nicht
     */
    inline bool isFhn(int x, int y)
    {
        return isFhnField(x, y);
    }
};


#endif /* FHNK_LATTICE_H_ */
