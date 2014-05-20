
FIND_PACKAGE( Qt4 REQUIRED QtCore QtGui QtScript )

SET( CMAKE_AUTOMOC TRUE )

INCLUDE( ${QT_USE_FILE} )

# Collect all required files for build
FILE( GLOB_RECURSE PLOT_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.h" )
FILE( GLOB_RECURSE PLOT_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.cpp" )
FILE( GLOB_RECURSE resources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.qrc" )
FILE( GLOB_RECURSE PLOT_UIS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ui" )

# Preprocess all Qt files
QT4_WRAP_CPP( PLOT_MOC_HEADERS ${PLOT_HEADERS} )
QT4_WRAP_UI( PLOT_UI_HEADERS ${PLOT_UIS} )
QT4_ADD_RESOURCES( resources_rcc ${resources} )

INCLUDE_DIRECTORIES( ../lattice ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/extensions ${CMAKE_CURRENT_BINARY_DIR} )

SET( QwtBuildDir ${CMAKE_BINARY_DIR}/external/Qwt )

SET( QwtHeaders ${QwtBuildDir}/src/Qwt/src )
SET( QwtLibrary ${QwtBuildDir}/src/Qwt-build/lib/libqwt.dylib )

INCLUDE_DIRECTORIES( ${QwtHeaders} )

include(ExternalProject)
ExternalProject_Add( Qwt
  PREFIX ${QwtBuildDir}
  SVN_REPOSITORY "svn://svn.code.sf.net/p/qwt/code/branches/qwt-5.2"
  CONFIGURE_COMMAND qmake ../Qwt/qwt.pro
  BUILD_COMMAND make
  )

ADD_DEFINITIONS( ${QT_DEFINITIONS} )
ADD_EXECUTABLE( plot ${PLOT_UI_HEADERS} ${PLOT_HEADER_MOCS} ${PLOT_SOURCES} ${resources_rcc} ../lattice/plugin.cpp )
ADD_DEPENDENCIES( plot Qwt )

TARGET_LINK_LIBRARIES( plot ${QT_LIBRARIES} ${Boost_LIBRARIES} ${QwtLibrary} ${lattice} )

