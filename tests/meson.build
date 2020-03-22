glib_dep = dependency('glib-2.0', required : false)

src_inc = include_directories('../src/')

if glib_dep.found()

    testlib_sources = ['testlib.c']
    if target_machine.system() == 'windows'
        testlib = static_library(
            'testlib', [testlib_sources], 
            include_directories : inc,
            name_prefix : '', name_suffix : 'lib')
    else
        testlib = library(
            'testlib',
            testlib_sources,
            include_directories : inc,
            dependencies : [m_dep])
    endif

    # coarray tests
    test_coarray = executable('test_coarray', ['test_coarray.c'],
        include_directories : [inc],
        dependencies : [glib_dep],
        link_with : [pantheralib])
    test('test_coarray',
        test_coarray,
        env: [
            'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
            'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir())
            ]
        )

    # subsection tests
    test_subsection = executable('test_subsection', ['test_subsection.c'],
        include_directories : [inc, src_inc],
        dependencies : [glib_dep],
        link_with : [testlib, pantheralib])
    test('test_subsection',
        test_subsection,
        env: [
            'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
            'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir())
            ]
        )

    # cross section tests
    test_crosssection = executable('test_crosssection',
        ['test_crosssection.c'],
        include_directories : [inc],
        dependencies : [glib_dep],
        link_with : [testlib, pantheralib])
    test('test_crosssection',
        test_crosssection,
        env: [
            'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
            'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir())
            ]
        )

    # reach tests
    test_crosssection = executable('test_reach',
        ['test_reach.c'],
        include_directories : [inc],
        dependencies : [glib_dep],
        link_with : [testlib, pantheralib])
    test('test_reach',
        test_crosssection,
        env: [
            'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
            'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir())
            ]
        )

endif

vlgnd = find_program('valgrind', required : false)
if vlgnd.found()
    subdir('mem')
endif
