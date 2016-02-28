#ifndef BISTABLE_LATTICE_H
#define BISTABLE_LATTICE_H

#include "../lattice/rds_lattice.h"

class BistableLattice;
template <>
struct Metainfo<BistableLattice> : MetainfoBase
{
    typedef OneComponentSystem Components;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
};
class BistableLattice : public SIIP_LatticeIntegrator<BistableLattice>
{
public:
    inline BistableLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<BistableLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY), epsilon(0.1, "epsilon", 0.0, 100.0)
    {
        componentInfos[0] = ComponentInfo("Aktivator", "u", -2.2, 2.5);

        registerParameter(&epsilon);
    }
    Parameter<double> epsilon;
    inline OneComponentSystem fixpoint(long int) const
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
    inline OneComponentSystem step_df(OneComponentSystem sys)
    {
        return OneComponentSystem(step_du(sys.u()));
    }
};

#endif
