
if (FFTW_INCLUDES)
  # Already in cache, be silent
  set (FFTW_FIND_QUIETLY TRUE)
endif (FFTW_INCLUDES)

find_path (FFTW_INCLUDES fftw3.h)

find_library (FFTW_LIBRARIES NAMES fftw3)
find_library (FFTW_THREADS_LIBRARIES NAMES fftw3_threads)

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
set_target_properties( fhn_lattice
  PROPERTIES
  COMPILE_FLAGS -DRDS_PLUGIN
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/target/models"
  )
TARGET_LINK_LIBRARIES( fhn_lattice ${Boost_LIBRARIES} ${lattice} ${FFTW_LIBRARIES} ${FFTW_THREADS_LIBRARIES} )

