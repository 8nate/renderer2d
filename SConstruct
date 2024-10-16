env = Environment(CPPPATH = 'include')

env.Append(LIBPATH='lib')
env.Append(LIBS=[
    'glfw3',
    'opengl32',
    'user32',
    'gdi32',
    'shell32'
])

env.Program('bin/renderer2d', ['src/gl3w.c', 'src/renderer2d.cpp'])