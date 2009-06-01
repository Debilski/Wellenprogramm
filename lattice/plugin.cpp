/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * plugin.cpp
 *
 *  Created on: 15.03.2009
 *      Author: rikebs
 */

#include "plugin.h"

#include <stdexcept>

#include <iostream>
#include <dlfcn.h>

#include "lattice_interface.h"

/** Loads the specified plugin as a dynamic library and locates
 the plugin's exported functions

 @param  sFilename  Filename of the plugin to load
 */
Plugin::Plugin(const std::string& sFilename)
    :  libHandle_( 0 ),
       pfnGetEngineVersion_( 0 ),
       pfnRegisterPlugin_( 0 ),
       libRefCount_( 0 )
{
    // Try to load the plugin as a dynamic library
    libHandle_ = dlopen( sFilename.c_str(), RTLD_LAZY);
    if ( !libHandle_ )
        throw std::runtime_error( std::string( "Could not load '" ) + sFilename + "'" );

    // Locate the plugin's exported functions
    try {
        pfnGetEngineVersion_ = reinterpret_cast< fnGetEngineVersion* > ( dlsym(
            libHandle_, "getEngineVersion" ) );
        pfnRegisterPlugin_ = reinterpret_cast< fnRegisterPlugin* > ( dlsym(
            libHandle_, "registerPlugin" ) );

        // If the functions aren't found, we're going to assume this is
        // a plain simple DLL and not one of our plugins
        if ( !pfnGetEngineVersion_ || !pfnRegisterPlugin_ )
            throw std::runtime_error( std::string( "'" ) + sFilename + "' is not a valid plugin" );

        if ( getEngineVersion() !=  LATTICE_INTERFACE_VERSION )
            throw std::runtime_error( std::string( "'" ) + sFilename + "' has not been compiled to the last version of the interface" );
        // Initialize a new DLL reference counter
        libRefCount_ = new size_t( 1 );
    } catch (...) {
        dlclose( libHandle_ );
        throw ;
    }
}

/** Creates a copy of a plugin that has already been loaded.
 Required to provide correct semantics for storing plugins in
 an STL map container.

 @param  Other  Other plugin instance to copy
 */
Plugin::Plugin(const Plugin& Other) :
    libHandle_( Other.libHandle_ ), pfnGetEngineVersion_( Other.pfnGetEngineVersion_ ),
        pfnRegisterPlugin_( Other.pfnRegisterPlugin_ ), libRefCount_( Other.libRefCount_ )
{
    /// Increase DLL reference counter
    ++*libRefCount_;
}

/** Destroys the plugin, unloading its library when no more references
 to it exist.
 */
Plugin::~Plugin()
{
    // Only unload the DLL if there are no more references to it
    if ( !--*libRefCount_ ) {
        delete libRefCount_;
        dlclose( libHandle_ );
    }
}
