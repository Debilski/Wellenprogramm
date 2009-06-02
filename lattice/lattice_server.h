/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

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
        virtual std::auto_ptr< LatticeInterface > createRenderer() = 0;
        virtual void destroy( LatticeInterface* l) = 0;
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

    std::list< std::string > getModelNames()
    {
        std::list< std::string > names;

        for (size_t DriverIndex = 0; DriverIndex < getDriverCount(); ++DriverIndex) {
            names.push_back( getDriver( DriverIndex ).getName() );
        }
        return names;
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
    std::auto_ptr<LatticeInterface> createRenderer() {    \
      return std::auto_ptr<LatticeInterface>(new lattice_class());         \
    }                                                                               \
    void destroy(LatticeInterface* l) {                                             \
      delete l;                                                                     \
    }                                                                               \
};

#ifndef RDS_PLUGIN
#define INTERNAL static
#else
#define INTERNAL extern "C"
#endif

#define REGISTER_PLUGINS_BEGIN                                                      \
/* Retrieve the engine version we're going to expect */                             \
INTERNAL int getEngineVersion() {                                                 \
return LATTICE_INTERFACE_VERSION;                                                   \
}                                                                                   \
INTERNAL void registerPlugin(PluginKernel &K) {

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
