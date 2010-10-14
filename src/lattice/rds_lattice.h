/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/

/*
 * rds_lattice.h
 *
 *  Created on: 24.11.2008
 *      Author: rikebs
 */

#ifndef RDS_LATTICE_H_
#define RDS_LATTICE_H_

/**
 * Basic includes
 */

#include <cmath>
#include <complex>

#include <list>
#include <vector>
#include <map>
#include <queue>

#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#include <omp.h>
#endif

#include "rds_definitions.h"


#include <fftw3.h>
#include <blitz/array.h>
#include <blitz/array/stencil-et.h>
#include <blitz/tinyvec-et.h>
#include <blitz/numinquire.h>

#include "fftw3_wrapper.h"

#include "rds_helper.h"

#include "noise_generator.h"
#include "noise_lattice.h"
#include "cluster_counter.h"

#include "lattice_geometry.h"

#include "lattice_interface.h"
#include "lattice.h"
#include "lattice_integrator.h"

//#include "lattice_plugin_registration.h"
#include "plugin_kernel.h"

#endif /* RDS_LATTICE_H_ */
