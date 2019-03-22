glib_dep = dependency('glib-2.0')

test_inc = include_directories('.')
src_inc = include_directories('../src/')

testlib_sources = ['testlib.c']
testlib = library('testlib',
                  testlib_sources,
                  dependencies : [m_dep, glib_dep])

subdir('xs')
