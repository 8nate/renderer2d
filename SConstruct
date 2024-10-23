#!/usr/bin/env python3

env = Environment()

env.Append(CPPPATH='deps/include')
env.Append(LIBPATH='bin')

env.Append(LIBS=[
  'glfw3',
  'opengl32',
  'user32',
  'gdi32',
  'shell32'
])

objs = []

objs += env.Object('deps/gl3w/gl3w.c')

for src in Glob("renderer/*.cpp"):
  objs += env.Object(src)

env.Program('bin/render', objs)
