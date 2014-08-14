# ex: syntax=python

env = Environment(CPPPATH='../ipc', CPPFLAGS=['-std=c++11'])
fles_ipc = env.Library(env.Glob('../ipc/*.cpp'))

env.Program('tscreator', ['main.cpp'],
            LIBS=[fles_ipc, 'boost_serialization'])

env.Program('tstester', ['tester.cpp'],
            LIBS=[fles_ipc, 'boost_serialization'])
