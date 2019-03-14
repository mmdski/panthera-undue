xs_test_inc = include_directories('.')

test_coord_sources = ['test_coord.c']
test_coord = library('test_coord',
                     test_coord_sources,
                     include_directories : [inc, xs_inc, test_inc, xs_test_inc],
                     dependencies : glib_dep,
                     link_with : [testlib, ciilib, xslib])
test_subsection_sources = ['test_subsection.c']
test_subsection = library('test_subsection',
                     test_subsection_sources,
                     include_directories : [inc, xs_inc, test_inc, xs_test_inc],
                     dependencies : [glib_dep, m_dep],
                     link_with : [testlib, ciilib, xslib])

test_xs_sources = ['test_xs.c']
test_xs = executable('test_xs',
                     test_xs_sources,
                     include_directories : [test_inc, xs_test_inc],
                     dependencies : glib_dep,
                     link_with : [test_coord, test_subsection])

test('test_xs', test_xs)
