#ifndef PIECEWISE_LINEAR_LATTICE_H
#define PIECEWISE_LINEAR_LATTICE_H

class PiecewiseLinearLattice;
template <>
struct Metainfo<PiecewiseLinearLattice>
{
    typedef TwoComponentSystem Components;
};
class PiecewiseLinearLattice : public SIIP_LatticeIntegrator<PiecewiseLinearLattice>
{
public:
    inline void step_d(double* in, double* out)
    {
        out[0] = step_du(in[0], in[1]);
        out[1] = step_dv(in[0], in[1]);
    }
    inline PiecewiseLinearLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<PiecewiseLinearLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY),
                                                                                              epsilon(0.1, "epsilon", 0.0, 100.0),
                                                                                              delta(-0.1, "delta", -2.0, 2.0)
    {
        componentInfos[0] = ComponentInfo("Aktivator", "u", -1.2, 1.2);
        componentInfos[1] = ComponentInfo("Inhibitor", "v", -1.2, 1.2);

        modelName_ = "Piecewise linear";

        setExcitationThreshold(0.5);

        registerParameter(&epsilon);
        registerParameter(&delta);
    }
    Parameter<double> epsilon;
    Parameter<double> delta;

    inline double fixpointU() const
    {
        return delta.get();
    }
    inline double fixpointV() const
    {
        return -1 - delta.get();
    }
    inline double step_du(double u, double v)
    {
        if (u >= 0) {
            return (1.0 / epsilon.get()) * (-u - v + 1);
        } else {
            return (1.0 / epsilon.get()) * (-u - v - 1);
        }
    }
    inline double step_dv(double u, double /*v*/)
    {
        return u - delta.get();
    }

    inline TwoComponentSystem step_d(TwoComponentSystem sys)
    {
        return TwoComponentSystem(step_du(sys.u(), sys.v()), step_dv(sys.u(), sys.v()));
    }

    inline blitz::TinyVector<double, number_of_Variables> step_d(blitz::TinyVector<double,
        number_of_Variables> sys)
    {
        return blitz::TinyVector<double, number_of_Variables>(
            step_du(sys[0], sys[1]), step_dv(sys[0], sys[1]));
    }

    inline TwoComponentSystem fixpoint() const
    {
        return TwoComponentSystem(fixpointU(), fixpointV());
    }
    inline int noiseVariable() const
    {
        return 1;
    }
};


#endif
