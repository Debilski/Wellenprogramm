/*
 * lattice_server.h
 *
 *  Created on: 15.03.2009
 *      Author: rikebs
 */

#ifndef LATTICE_SERVER_H_
#define LATTICE_SERVER_H_

/// Manages graphics related stuff
class LatticeServer {
public:
    /// Engine graphics driver
    class LatticeDriver {
    public:
        /// Destructor
        virtual ~LatticeDriver()
        {
        }
        /// Get name of the Lattice driver
        virtual const std::string &getName() const = 0;
        /// Create a renderer
        virtual std::auto_ptr< LatticeInterface > createRenderer(int x, int y, int lx, int ly) = 0;
    };

    /// Destructor
    ~LatticeServer()
    {
        for (LatticeDriverVector::reverse_iterator It = LatticeDrivers_.rbegin(); It
            != LatticeDrivers_.rend(); ++It)
            delete *It;
    }

    /// Allows plugins to add new Lattice drivers
    void addLatticeDriver(std::auto_ptr< LatticeDriver > GD)
    {
        LatticeDrivers_.push_back( GD.release() );
    }

    /// Get the total number of registered Lattice drivers
    size_t getDriverCount() const
    {
        return LatticeDrivers_.size();
    }
    /// Access a driver by its index
    LatticeDriver &getDriver(size_t Index)
    {
        return *LatticeDrivers_.at( Index );
    }

private:
    /// A vector of Lattice drivers
    typedef std::vector< LatticeDriver * > LatticeDriverVector;

    LatticeDriverVector LatticeDrivers_; ///< All available Lattice drivers
};

#define DEFINE_LATTICE_DRIVER2(lattice_name,lattice_class)                          \
class lattice_class##_LatticeDriver : public LatticeServer::LatticeDriver {         \
  public:                                                                           \
    /* Destructor */                                                                \
     virtual ~lattice_class##_LatticeDriver() {}                                    \
                                                                                    \
    /* Get name of the graphics driver */                                           \
     virtual const std::string &getName() const {                                   \
      static std::string sName(lattice_name);                                       \
      return sName;                                                                 \
    }                                                                               \
                                                                                    \
    /* Create a renderer */                                                         \
    std::auto_ptr<LatticeInterface> createRenderer(int x,int y,int lx, int ly) {    \
      return std::auto_ptr<LatticeInterface>(new lattice_class(x,y,lx,ly));         \
    }                                                                               \
};

#define REGISTER_PLUGINS_BEGIN                                                      \
/* Retrieve the engine version we're going to expect */                             \
extern "C" int getEngineVersion() {                                                 \
return LATTICE_INTERFACE_VERSION;                                                   \
}                                                                                   \
extern "C"  void registerPlugin(PluginKernel &K) {

#define REGISTER_PLUGINS_END      }

#define REGISTER_PLUGIN(lattice_class)                                              \
    K.getLatticeServer().addLatticeDriver(                                          \
        std::auto_ptr<LatticeServer::LatticeDriver>(new lattice_class##_LatticeDriver())    \
      );

#define LATTICE_REGISTER_MODEL2(lattice_name,lattice_class)                         \
DEFINE_LATTICE_DRIVER2(lattice_name,lattice_class)                                  \
/* Tells us to register our functionality to an engine kernel */                    \
REGISTER_PLUGINS_BEGIN                                                              \
REGISTER_PLUGIN(lattice_class)                                                      \
REGISTER_PLUGINS_END

#endif /* LATTICE_SERVER_H_ */
