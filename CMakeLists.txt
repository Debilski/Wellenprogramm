cmake_minimum_required (VERSION 2.8)

project(Wellenprogramm)

FIND_PACKAGE( Boost REQUIRED
    COMPONENTS thread system
  )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

SET( CMAKE_CXX_FLAGS_RELEASE "-DBOOST_THREADS=4 -O3" )

SET( CMAKE_CXX_FLAGS_DEBUG "-g -ggdb" )

add_subdirectory(src)

