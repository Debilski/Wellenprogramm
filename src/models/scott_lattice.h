#ifndef SCOTT_LATTICE_H
#define SCOTT_LATTICE_H

class ScottLattice;
template <>
struct Metainfo<ScottLattice>
{
    typedef OneComponentSystem Components;
};
class ScottLattice : public SIIP_LatticeIntegrator<ScottLattice>
{
public:
    inline void step_d(double* in, double* out)
    {
        out[0] = step_du(in[0]);
    }
    inline ScottLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<ScottLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY), epsilon(0.1, "epsilon", 0.0, 100.0)
    {
        componentInfos[0] = ComponentInfo("Aktivator", "u", -2.2, 2.5);

        registerParameter(&epsilon);
    }
    Parameter<double> epsilon;

    inline OneComponentSystem fixpoint() const
    {
        return OneComponentSystem(fixpointU());
    }
    inline double fixpointU() const
    {
        return -1.1;
    }
    inline double fixpointV() const
    {
        return -1.969;
    }
    inline double step_du(double u /*v*/)
    {
        return (1.0 / epsilon()) * (3.0 * u - u * u * u - fixpointV());
    }
    inline double step_dv(double /*u*/, double /*v*/)
    {
        return 0;
    }

    inline OneComponentSystem step_d(OneComponentSystem sys)
    {
        return OneComponentSystem(step_du(sys.u()));
    }
    inline blitz::TinyVector<double, number_of_Variables> step_d(blitz::TinyVector<double,
        number_of_Variables> sys)
    {
        return blitz::TinyVector<double, number_of_Variables>(step_du(sys[0]));
    }

    inline int noiseVariable() const
    {
        return 0;
    }
};

#endif
