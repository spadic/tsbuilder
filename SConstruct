# ex: syntax=python

env = Environment(CPPPATH='../ipc', CPPFLAGS=['-std=c++11'])
fles_ipc = env.Library(env.Glob('../ipc/*.cpp'))

mcsource = env.Library('MicrosliceSource.cpp')

env.Program('tscreator', ['main.cpp', mcsource],
            LIBS=[fles_ipc, 'boost_serialization'])

env.Program('tstester', ['tester.cpp'],
            LIBS=[fles_ipc, 'boost_serialization'])
