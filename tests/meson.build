glib_dep = dependency('glib-2.0', required : false)

if glib_dep.found()

    test_inc = include_directories('.')
    src_inc = include_directories('../src/')

    testlib_sources = ['testlib.c']
    testlib = library('testlib',
                      testlib_sources,
                      include_directories : inc,
                      dependencies : [m_dep, glib_dep])

    subdir('xs')
endif
