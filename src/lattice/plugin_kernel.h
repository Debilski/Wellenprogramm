/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

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

#include <stdexcept>
#include <string>
#include <map>

/// The engine's system core
class PluginKernel
{
public:
    /// Access the lattice server
    LatticeServer& getLatticeServer()
    {
        return latticeServer_;
    }

    /// Loads a plugin
    void loadPlugin(const std::string& sFilename)
    {
        try {
            if (loadedPlugins_.find(sFilename) == loadedPlugins_.end())
                loadedPlugins_.insert(PluginMap::value_type(sFilename, Plugin(sFilename))).first->second.registerPlugin(*this);
        } catch (std::runtime_error e) {
            std::cout << e.what() << " "
                      << "Ignoring.\n";
        }
    }

private:
    /// Map of plugins by their associated file names
    typedef std::map<std::string, Plugin> PluginMap;

    PluginMap loadedPlugins_;  ///< All plugins currently loaded

    LatticeServer latticeServer_;  ///< The graphics server
};

#endif /* PLUGIN_KERNEL_H_ */
