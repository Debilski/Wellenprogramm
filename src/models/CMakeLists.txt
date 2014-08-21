
IF (FFTW_INCLUDES)
  # Already in cache, be silent
  SET (FFTW_FIND_QUIETLY TRUE)
ENDIF (FFTW_INCLUDES)

FIND_PATH (FFTW_INCLUDES fftw3.h)

FIND_LIBRARY (FFTW_LIBRARIES NAMES fftw3)
FIND_LIBRARY (FFTW_THREADS_LIBRARIES NAMES fftw3_threads)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ../lattice )

ADD_LIBRARY( fhn_lattice SHARED
  fhn_lattice.cpp
  "../lattice/fftw3_wrapper.cpp"
  "../lattice/cluster_counter.cpp"
  "../lattice/noise_lattice.cpp"
  "../lattice/noise_generator.cpp"
  "../lattice/fftw3_wrapper.cpp"
  ${FFTW_INCLUDES}
  "../lattice/rds_helper.cpp"
  )
ADD_DEPENDENCIES( fhn_lattice Blitz )
SET_TARGET_PROPERTIES( fhn_lattice
  PROPERTIES
  COMPILE_FLAGS -DRDS_PLUGIN
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target/models"
  )
TARGET_LINK_LIBRARIES( fhn_lattice ${Boost_LIBRARIES} ${lattice} ${FFTW_LIBRARIES} ${FFTW_THREADS_LIBRARIES} )

