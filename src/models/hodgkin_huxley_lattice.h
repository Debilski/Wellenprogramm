#ifndef HODGKIN_HUXLEY_LATTICE_H
#define HODGKIN_HUXLEY_LATTICE_H

#include "../lattice/rds_lattice.h"

/**
 * \class HodgkinHuxleyLattice
 *
 * The smallest funtions are to be inlined, so we mention them in the header file.
 */
class HodgkinHuxleyLattice;
/**
 * Helper Class for additional but dependent Information
 */
template <>
struct Metainfo<HodgkinHuxleyLattice> : MetainfoBase
{
    typedef HodgkinHuxleyComponentSystem Components;
    static const int number_of_Noise_Variables = 1;
    template <int N>
    struct NoiseMapping
    {
        enum
        {
            value = -1
        };
    };
};
template <>
struct Metainfo<HodgkinHuxleyLattice>::NoiseMapping<0>
{
    enum
    {
        value = 0
    };
};
//META(HodgkinHuxleyLattice, HodgkinHuxleyComponentSystem)
class HodgkinHuxleyLattice : public SIIP_LatticeIntegrator<HodgkinHuxleyLattice>
{
    //  double gamma_, epsilon_;
public:
    inline HodgkinHuxleyLattice(int sizeX, int sizeY, int latticeSizeX, int latticeSizeY) : SIIP_LatticeIntegrator<HodgkinHuxleyLattice>(sizeX, sizeY, latticeSizeX, latticeSizeY),
                                                                                            E_Na(115, "E_Na", 80, 150, 1),
                                                                                            E_K(-12, "E_K", -20, -1, 1),
                                                                                            E_L(
                                                                                                10.6, "E_L", 8, 15, 1),
                                                                                            g_Na(120, "g_Na", 80, 150, 1),
                                                                                            g_K(36, "g_K", 20, 50, 1),
                                                                                            g_L(0.3, "g_L", 0, 1, 2),
                                                                                            C(1.0, "C", 0.01, 10, 2),
                                                                                            I_ext(-20.0, "I_ext", -35, 20.0, 1)
    {

        componentInfos[0] = ComponentInfo("Membranpotenzial", "V", -80, 60).setPhysicalUnitSymbol("mV").setPhysicalQuantity("Potenzial");
        componentInfos[1] = ComponentInfo("Gating n", "n", 0, 1);
        componentInfos[2] = ComponentInfo("Gating m", "m", 0, 1);
        componentInfos[3] = ComponentInfo("Gating h", "h", 0, 1);

        modelName_ = "Hodgkin-Huxley";

        registerParameter(&g_K);
        registerParameter(&g_Na);
        registerParameter(&g_L);
        registerParameter(&E_K);
        registerParameter(&E_Na);
        registerParameter(&E_L);
        registerParameter(&I_ext);
        registerParameter(&C);
    }

    Parameter<double> E_Na, E_K, E_L, g_Na, g_K, g_L, C, I_ext;

    inline double alpha_n(double V)
    {
        double a_n = 0.001 * (10.0 - V) / (exp((10.0 - V) / 10.0) - 1.0);
        return a_n;
    }
    inline double alpha_m(double V)
    {
        double a_m = 0.1 * (25.0 - V) / (exp((25.0 - V) / 10.0) - 1.0);
        return a_m;
    }
    inline double alpha_h(double V)
    {
        double a_h = 0.07 * exp(-V / 20.0);
        return a_h;
    }
    inline double beta_n(double V)
    {
        double b_n = 0.125 * exp(-V / 80.0);
        return b_n;
    }
    inline double beta_m(double V)
    {
        double b_m = 4.0 * exp(-V / 18.0);
        return b_m;
    }
    inline double beta_h(double V)
    {
        double b_h = 1.0 / (exp((30.0 - V) / 10.0) + 1.0);
        return b_h;
    }

    inline HodgkinHuxleyComponentSystem step_df(HodgkinHuxleyComponentSystem sys)
    {
        double dV, dn, dm, dh;

        double I_K = g_K() * pow(sys.n(), 4) * (sys.V() - E_K());
        double I_Na = g_Na() * pow(sys.m(), 3) * sys.h() * (sys.V() - E_Na());
        double I_L = g_L() * (sys.V() - E_L());

        dV = (1.0 / C()) * (I_ext() - I_K - I_Na - I_L);
        dn = alpha_n(sys.V()) * (1 - sys.n()) - beta_n(sys.V()) * sys.n();
        dm = alpha_m(sys.V()) * (1 - sys.m()) - beta_m(sys.V()) * sys.m();
        dh = alpha_h(sys.V()) * (1 - sys.h()) - beta_h(sys.V()) * sys.h();
        return HodgkinHuxleyComponentSystem(dV, dn, dm, dh);
    }
    inline HodgkinHuxleyComponentSystem fixpoint(long int) const
    {
        return HodgkinHuxleyComponentSystem(0.0, 0.4, 0.1, 0.5);
    }
};

#endif
