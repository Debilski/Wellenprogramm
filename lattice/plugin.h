/*
 * plugin.h
 *
 *  Created on: 15.03.2009
 *      Author: rikebs
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include <string>
#include <memory>

class Kernel;

/// Representation of a plugin
class Plugin {
public:
    /// Initialize and load plugin
    Plugin(const std::string &sFilename);
    /// Copy existing plugin instance
    Plugin(const Plugin &Other);
    /// Unload a plugin
    ~Plugin();

    //
    // Plugin implementation
    //
public:
    /// Query the plugin for its expected engine version
    int getEngineVersion() const
    {
        return m_pfnGetEngineVersion();
    }

    /// Register the plugin to a kernel
    void registerPlugin(Kernel &K)
    {
        m_pfnRegisterPlugin( K );
    }

private:
    /// Too lazy to this now...
    Plugin &operator =(const Plugin &Other);

    /// Signature for the version query function
    typedef int fnGetEngineVersion();
    /// Signature for the plugin's registration function
    typedef void fnRegisterPlugin(Kernel &);

    void* m_hDLL; ///< Win32 DLL handle
    size_t *m_pDLLRefCount; ///< Number of references to the DLL
    fnGetEngineVersion *m_pfnGetEngineVersion; ///< Version query function
    fnRegisterPlugin *m_pfnRegisterPlugin; ///< Plugin registration function
};





#endif /* PLUGIN_H_ */
