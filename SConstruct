# ex: syntax=python

import os

env = Environment(CPPFLAGS=['-std=c++11'])

# fles_ipc
try:
    env.Replace(FLES_IPC_DIR=os.environ['FLES_IPC'])
except KeyError:
    raise SystemExit("Please set the FLES_IPC environment variable "
                     "to the root of the fles_ipc source tree.")
env.Append(CPPPATH=['$FLES_IPC_DIR'])
fles_ipc = env.Library(env.Glob('$FLES_IPC_DIR/*.cpp'))

# MicrosliceSource
mcsource = env.Library('MicrosliceSource.cpp')

# tscreator
env.Program('tscreator', ['main.cpp', 'TimesliceBuilder.cpp', mcsource],
            LIBS=[fles_ipc, 'boost_serialization'])

# tstester
env.Program('tstester', ['tester.cpp'],
            LIBS=[fles_ipc, 'boost_serialization'])
