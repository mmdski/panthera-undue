xs_test_inc = include_directories('.')

test_xs = executable('test_xs',
                     [
                     'test_xs.c', 'test_coord.c', 'test_subsection.c',
                     'test_coarray.c'
                     ],
                     include_directories : [
                     inc, src_inc, test_inc, xs_test_inc],
                     dependencies : [glib_dep, m_dep],
                     link_with : [testlib, ciilib, xslib, pantheralib])

test('test_xs',
     test_xs, env: [
     'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
     'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
     ],
     is_parallel : false
)
