test_inc = include_directories('.')

glib_dep = dependency('glib-2.0', required : false)
if glib_dep.found()

    testlib_sources = ['testlib.c']
    testlib = library('testlib',
                      testlib_sources,
                      include_directories : inc,
                      dependencies : [m_dep, glib_dep],
                      link_with : pantheralib)

    subdir('xs')

    # xstable test
    test_xstable = executable('test_xstable', 'test_xstable.c',
                              include_directories : [inc, test_inc],
                              dependencies : [glib_dep, m_dep],
                              link_with : [testlib, ciilib, pantheralib])
    test('test_xstable', test_xstable,
         env: ['G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
         'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),]
    )

    # reach test
    test_reach = executable('test_reach', 'test_reach.c',
                            include_directories : [inc, test_inc],
                            dependencies : [glib_dep],
                            link_with : [testlib, ciilib, pantheralib])
    test('test_reach', test_reach,
         env: ['G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
         'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),]
    )

    #standard step test
    test_standardstep = executable('test_standardstep', 'test_standardstep.c',
                                   include_directories : [inc, test_inc],
                                   dependencies : [glib_dep],
                                   link_with : [testlib, ciilib, pantheralib])
    test('test_standardstep', test_standardstep,
         env: ['G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
         'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),]
)

endif

vlgnd = find_program('valgrind')
if vlgnd.found()
    subdir('mem')
endif
