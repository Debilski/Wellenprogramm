#!/usr/bin/env python
import os
import sys
import platform
from build_support import *
from build_config import *

# Setup some of our requirements

env = Environment(ENV = {'PATH' : "/home/rikebs/bin:"+os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME']                 
                         })#CXX = 'mpicxx.openmpi')

#env.Replace(CXX = "icc")

# ccache als Option machen
env.Replace(CXX = "ccache g++ ")


# setup the include paths where FLTK and silc should live
env.Append(CPPPATH=include_search_path +['/opt/local/include']) #, '/usr/lib/openmpi/include'])


#['m', 'jpeg', 'png', 'tiff', 'z', 'Xext', 'X11'])

#env.Append(LIBPATH=['/usr/X11R6/lib'])

#print env.Dump()
debug = ARGUMENTS.get('debug', 0)
noopt = ARGUMENTS.get('noopt', 0)
release = ARGUMENTS.get('release', 0)

target_dir_argument = ARGUMENTS.get('build_dir', None)
environment = ARGUMENTS.get('env', 'dong')

# start the build
build_base_dir = SelectBuildBaseDir(build_base_dir)

if target_dir_argument is not None:
  target_dir = '#' + target_dir_argument
else:
  target_dir = '#' + build_base_dir
target_bin_dir = target_dir + os.sep + bin_sub_dir
target_ui_dir = target_dir + os.sep + ui_sub_dir
target_lib_dir = target_dir + os.sep + lib_sub_dir
target_models_dir = target_dir + os.sep + models_sub_dir
print build_base_dir

print target_dir

SConscript(target_dir + os.sep + 'SConscript')

BuildDir(target_bin_dir, source_base_dir, duplicate=0)

env.Decider('MD5-timestamp')
env.Append(CCFLAGS='-Wall -Wextra ')
env.Append(LIBPATH=[target_models_dir, target_lib_dir, target_bin_dir, '#ui', '/usr/lib/openmpi/lib', lib_search_path])   

## Special Libraries
progressbar = BuildLib(env, target_lib_dir + os.sep + 'progressbar', 'share/progressbar/progressbar.cpp')
gnuplot = BuildLib(env, target_lib_dir + os.sep + 'gnuplot_i++', 'tools/gnuplot_i++/gnuplot_i.cc')
        

if int(debug):
    env.Append(CCFLAGS = ' -g -ggdb ')
if not int(noopt):
    env.Append(CCFLAGS= ' -O3 ')
if int(release):
    env.Append(CCFLAGS= ' -DNDEBUG -DBOOST_THREADS=4 ')


env.Tool('qt4', toolpath=['./ui'])
#env.Tool('qt')

env['CXXFILESUFFIX']='.cpp'
if not "linux-default" in environment :
	env.EnableQt4Modules(["QtGui","QtCore","QtScript"])

if "linux-default" in environment :
    env.Append(CCFLAGS= ' -msse3 -msse2 -msse ') # -ffast-math # -fbranch-probabilities	    
    env.Append(CPPPATH=['/users/stud/rikebs/local/include','/users/stud/rikebs/local/include/qt4','/users/stud/rikebs/local/include/qt4/QtGui','/users/stud/rikebs/local/include/qt4/QtCore','/users/stud/rikebs/local/include/qwt-qt4'])
    env.Append(LIBPATH = ['/users/stud/rikebs/local/lib'])
    env.Append(LIBS=static_libs + ['qwt-qt4', 'QtGui', 'QtCore','boost_thread-gcc41-mt', "fftw3" , "fftw3_threads" ,"m" ])

if "lomo" in environment :
    env.Append(CCFLAGS= ' -mtune=native -march=native -msse3 -msse2 -msse -fopenmp ') # -ffast-math # -fbranch-probabilities
    env.Append(CPPPATH=['/usr/include/qwt-qt4/'])
    env.Append(LIBS=static_libs + ['qwt-qt4', 'boost_thread', "fftw3" , "fftw3_threads" ,"m", "mpi", "gomp"])

if "dong" in environment :
    env.Append(CCFLAGS= ' -mtune=native -march=native -msse3 -msse2 -msse -fPIC ')
    env.Append(LIBS=static_libs + ['qwt', 'boost_thread', "fftw3" , "fftw3_threads" ,"m"])

if "darwin" in environment :
    env.Append(CCFLAGS= ' -fast -fopenmp -O4 ')
    env.Append(LIBS=static_libs + ['qwt', 'boost_thread-mt', "fftw3" , "fftw3_threads" ,"m", "gomp", "blitz"])
    env.Append(FRAMEWORKS=['QtScript', 'QtXml'])
    env.Replace(CXX = 'ccache /Developer/usr/bin/llvm-g++-4.2 ')

# variables the sub build directories need
Export('env', 'sources', 'static_libs', 'test_sources', 'target_lib_dir', 'target_bin_dir', 'progressbar', 'gnuplot')

# this sets up an alias for test that will compile the unit tests
# into the resulting testrunner program.
env.Alias('test', target_dir + os.sep + 'testrunner')



SConscript(source_base_dir + os.sep + 'SConscript', variant_dir=target_bin_dir, duplicate=0)

SConscript('lattice/SConscript', variant_dir=target_lib_dir, duplicate=0)
SConscript('ui/SConscript', variant_dir=target_ui_dir, duplicate=0)

SConscript('models/SConscript', variant_dir=target_models_dir, duplicate=0)
