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

# flib_dpb
try:
    env.Replace(FLIB_DPB_DIR=os.environ['FLIB_DPB'])
except KeyError:
    raise SystemExit("Please set the FLIB_DPB environment variable "
                     "to the root of the flib_dpb source tree.")
env.Append(CPPPATH=['$FLIB_DPB_DIR'])
flib_dpb = env.Library(env.Glob('$FLIB_DPB_DIR/*.cpp'))

# tsbuilder
env.Program('tsbuilder',
            ['main.cpp', 'MicrosliceSource.cpp', 'TimesliceBuilder.cpp'],
            LIBS=[fles_ipc, flib_dpb, 'boost_serialization', 'zmq'])

# tstester
env.Program('tstester', ['tester.cpp'],
            LIBS=[fles_ipc, 'boost_serialization'])
