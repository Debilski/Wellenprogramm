
import os

## where we find STATIC libraries
lib_search_path = ['/lib','/usr/lib','/usr/local/lib']

## where we should find things to include
include_search_path =  ['#share','#lattice','#tools','#src','#ui','#models']

## These are our source files
sources = []
test_sources = []

# update the environment with options from fltk-config
static_libs = ['']

#### You should not change these.  These are only here
#### If you want to start your own build setup with a
#### different layout than mine.
source_base_dir = 'src'
build_base_dir = './builds'
lib_sub_dir = 'lib'
bin_sub_dir = '.'
ui_sub_dir = 'ui'

models_sub_dir = 'models'