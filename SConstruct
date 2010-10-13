#!/usr/bin/env python
import os
import sys
import platform

# Setup some of our requirements

env = Environment(ENV = {'PATH' : "/home/rikebs/bin:"+os.environ['PATH'],
                         'TERM' : os.environ['TERM'],
                         'HOME' : os.environ['HOME']                 
                         })#CXX = 'mpicxx.openmpi')
env.Tool('qt4', toolpath=['./ui/scons-qt4'])

#print env.Dump()
debug = ARGUMENTS.get('debug', 0)
noopt = ARGUMENTS.get('noopt', 0)
release = ARGUMENTS.get('release', 0)

environment = ARGUMENTS.get('env', 'dong')


VariantDir("./ui", "./builds/ui", duplicate=0)
VariantDir("./model", "./builds/model", duplicate=0)
VariantDir("./src", "./builds/src", duplicate=0)
VariantDir("./lattice", "./builds/lattice", duplicate=0)

env.Decider('MD5-timestamp')
env.Append(CCFLAGS='-Wall -Wextra ')

## Special Libraries
progressbar = env.Library('progressbar', 'share/progressbar/progressbar.cpp')
gnuplot = env.Library('gnuplot_i++', 'tools/gnuplot_i++/gnuplot_i.cc')
        

if int(debug):
    env.Append(CCFLAGS = ' -g -ggdb ')
if not int(noopt):
    env.Append(CCFLAGS= ' -O3 ')
if int(release):
    env.Append(CCFLAGS= ' -DNDEBUG -DBOOST_THREADS=4 ')


env['CXXFILESUFFIX']='.cpp'


env.Append(CCFLAGS= ' -fast -fopenmp -O4 -F/usr/local/Cellar/qt/4.7.0/Frameworks -I/usr/local/Cellar/qt/4.7.0/Frameworks/QtCore.framework/Headers  -I/usr/local/Cellar/qt/4.7.0/Frameworks/QtGui.framework/Headers -I/usr/local/Cellar/qt/4.7.0/Frameworks/QtScript.framework/Headers ')
env.Append(LIBS=['qwt', 'boost_thread-mt', "fftw3" , "fftw3_threads" ,"m", "gomp", "blitz"])
env.Replace(CXX = 'ccache g++ ')#/Developer/usr/bin/llvm-g++-4.2 ')

# variables the sub build directories need
Export('env', 'progressbar', 'gnuplot')

#SConscript(source_base_dir + os.sep + 'SConscript', variant_dir=target_bin_dir, duplicate=0)

pluginEnv = env.Clone()
pluginEnv.Append( CCFLAGS=" -DRDS_PLUGIN " )


library_files = [ '#lattice/rds_helper.cpp',
'#lattice/cluster_counter.cpp',
'#lattice/noise_generator.cpp',
'#lattice/noise_lattice.cpp',
'#lattice/fftw3_wrapper.cpp',
 '#lattice/plugin.cpp' ] 

lattice_plugin_registration = env.Library('lattice_plugin_registration', ['lattice/lattice_plugin_registration.cpp'])

plugin = env.Library('lattice/plugin', ['lattice/plugin.cpp'])
lattice = env.Library('lattice/lattice', library_files)

fhnlattice = env.Library('models/fhn_lattice_s', ['models/fhn_lattice.cpp'] + library_files)
fhnlatticeS = pluginEnv.SharedLibrary('models/fhn_lattice', ['models/fhn_lattice.cpp'] + library_files)


#SConscript('lattice/SConscript', variant_dir=target_lib_dir, duplicate=0)
#SConscript('models/SConscript', variant_dir=target_models_dir, duplicate=0)

env.Append(CPPPATH=["lattice", "ui/extensions", "ui/3rd-party"])

extensions = env.Library(Glob('ui/extensions/*.cpp'))

env.EnableQt4Modules(["QtGui","QtCore","QtScript"])
env.Program('ui/plot', Glob('ui/*.cpp'), LIBS=env["LIBS"]+[lattice, extensions])



