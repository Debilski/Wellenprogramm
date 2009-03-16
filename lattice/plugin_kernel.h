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
class PluginKernel {
public:
    /// Access the lattice server
    LatticeServer &getLatticeServer()
    {
        return latticeServer_;
    }

    /// Loads a plugin
    void loadPlugin(const std::string &sFilename)
    {
        if ( loadedPlugins_.find( sFilename ) == loadedPlugins_.end() )
            loadedPlugins_.insert( PluginMap::value_type( sFilename, Plugin( sFilename ) ) ).first->second.registerPlugin(
                *this );
    }

private:
    /// Map of plugins by their associated file names
    typedef std::map< std::string, Plugin > PluginMap;

    PluginMap loadedPlugins_; ///< All plugins currently loaded

    LatticeServer latticeServer_; ///< The graphics server
};

#endif /* PLUGIN_KERNEL_H_ */
