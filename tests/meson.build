glib_dep = dependency('glib-2.0')

test_inc = include_directories('.')
testlib_sources = ['testlib.c']
testlib = library('testlib',
                  testlib_sources,
                  dependencies : [m_dep, glib_dep])

subdir('xs')

test_panthera_sources = ['test_panthera.c']
test_panthera = executable('test_panthera',
                           test_panthera_sources,
                           dependencies : glib_dep,
                           link_with : test_xs)

test('test_panthera', test_panthera)
