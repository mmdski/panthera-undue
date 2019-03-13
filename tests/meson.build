glib_dep = dependency('glib-2.0')

test_inc = include_directories('.')
testlib_sources = ['panthera_test.c']
panthera_testlib = library('panthera_test',
                  testlib_sources,
                  dependencies : [m_dep, glib_dep])

subdir('xs')
