/*
 * plugin_kernel.h
 *
 *  Created on: 15.03.2009
 *      Author: rikebs
 */

#ifndef PLUGIN_KERNEL_H_
#define PLUGIN_KERNEL_H_

#include "lattice_server.h"
#include "plugin.h"

#include <string>
#include <map>

/// The engine's system core
class Kernel {
  public:
    /// Access the lattice server
     LatticeServer &getLatticeServer() { return m_LatticeServer; }

    /// Loads a plugin
     void loadPlugin(const std::string &sFilename) {
      if(m_LoadedPlugins.find(sFilename) == m_LoadedPlugins.end())
        m_LoadedPlugins.insert(PluginMap::value_type(
          sFilename,
          Plugin(sFilename)
        )).first->second.registerPlugin(*this);
    }

  private:
    /// Map of plugins by their associated file names
    typedef std::map<std::string, Plugin> PluginMap;

    PluginMap      m_LoadedPlugins;  ///< All plugins currently loaded

    LatticeServer m_LatticeServer; ///< The graphics server
};


#endif /* PLUGIN_KERNEL_H_ */
