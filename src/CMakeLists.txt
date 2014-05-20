
FIND_PACKAGE( Qt4 REQUIRED QtCore QtGui QtScript )

SET( CMAKE_AUTOMOC TRUE )

INCLUDE( ${QT_USE_FILE} )

SET( QwtBuildDir ${CMAKE_BINARY_DIR}/external/Qwt )

SET( QwtHeaders ${QwtBuildDir}/src/Qwt/src )
SET( QwtLibrary ${QwtBuildDir}/src/Qwt-build/lib/libqwt.dylib )

INCLUDE_DIRECTORIES( ${QwtHeaders} )

include(ExternalProject)
ExternalProject_Add( Qwt
  PREFIX ${QwtBuildDir}
  SVN_REPOSITORY "svn://svn.code.sf.net/p/qwt/code/branches/qwt-5.2"
  CONFIGURE_COMMAND qmake -set prefix ${QwtBuildDir} ../Qwt/qwt.pro
  BUILD_COMMAND make
  )

ADD_DEFINITIONS( ${QT_DEFINITIONS} )

add_subdirectory(models)

add_subdirectory( ui )


#if (FFTW_INCLUDES)
  # Already in cache, be silent
  #  set (FFTW_FIND_QUIETLY TRUE)
  #endif (FFTW_INCLUDES)

  #find_path (FFTW_INCLUDES fftw3.h)

  #find_library (FFTW_LIBRARIES NAMES fftw3)
  #find_library (FFTW_THREADS_LIBRARIES NAMES fftw3_threads)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} lattice models share )

ADD_EXECUTABLE( modeltest
  "src/modeltest.cpp"
  "lattice/fftw3_wrapper.cpp"
  "lattice/cluster_counter.cpp"
  "lattice/noise_lattice.cpp"
  "lattice/noise_generator.cpp"
  "lattice/fftw3_wrapper.cpp"
  ${FFTW_INCLUDES}
  "lattice/rds_helper.cpp"
  "share/progressbar/progressbar.cpp"
  "models/fhn_lattice.cpp"
  )
ADD_DEPENDENCIES( modeltest Qwt )
TARGET_LINK_LIBRARIES( modeltest ${QT_LIBRARIES} ${Boost_LIBRARIES} ${QwtLibrary} ${lattice} ${FFTW_LIBRARIES} ${FFTW_THREADS_LIBRARIES} )

