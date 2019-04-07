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

    test_reach = executable('test_reach', 'test_reach.c',
                         include_directories : [inc, test_inc],
                         dependencies : [glib_dep, m_dep],
                         link_with : [testlib, ciilib, pantheralib])

    test('test_reach', test_reach,
         env: ['G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
         'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),]
    )
endif
