
FIND_PACKAGE( Qt4 REQUIRED QtCore QtGui QtScript )

SET( CMAKE_AUTOMOC TRUE )

INCLUDE( ${QT_USE_FILE} )

SET( BlitzBuildDir ${CMAKE_BINARY_DIR}/external/Blitz++ )
SET( BlitzHeaders ${BlitzBuildDir}/include )

SET( QwtBuildDir ${CMAKE_BINARY_DIR}/external/Qwt )
SET( QwtHeaders ${QwtBuildDir}/src/QWT/src )
IF( APPLE )
  SET( QwtLibrary ${QwtBuildDir}/src/QWT-build/lib/libqwt.dylib )
ELSE()
  SET( QwtLibrary ${QwtBuildDir}/src/QWT-build/lib/libqwt.so )
ENDIF()

INCLUDE_DIRECTORIES( ${QwtHeaders} ${BlitzHeaders} )

INCLUDE(ExternalProject)

ExternalProject_add(QWT
  PREFIX ${QwtBuildDir}
  URL "http://sourceforge.net/projects/qwt/files/qwt/5.2.3/qwt-5.2.3.tar.bz2"
  PATCH_COMMAND perl -pi -e "s|^.*INSTALLBASE.*=.*|INSTALLBASE=${QwtBuildDir}|" "../QWT/qwtconfig.pri"
  CONFIGURE_COMMAND qmake ../QWT/qwt.pro
  BUILD_COMMAND make
  )

ExternalProject_Add( Blitz
  PREFIX ${BlitzBuildDir}
  URL "http://downloads.sourceforge.net/project/blitz/blitz/Blitz%2B%2B%200.10/blitz-0.10.tar.gz"
  CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${BlitzBuildDir}
  BUILD_COMMAND make -j
  )


ADD_DEFINITIONS( ${QT_DEFINITIONS} )

ADD_SUBDIRECTORY(models)

ADD_SUBDIRECTORY( ui )

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
ADD_DEPENDENCIES( modeltest Qwt Blitz )

TARGET_LINK_LIBRARIES( modeltest ${QT_LIBRARIES} ${Boost_LIBRARIES} ${QwtLibrary} ${lattice} ${FFTW_LIBRARIES} ${FFTW_THREADS_LIBRARIES} )

