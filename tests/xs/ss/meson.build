test_ss_new = executable('test_ss_new', 
                         'test_ss_new.c',
                         include_directories : [inc, xs_inc],
                         link_with : [ciilib, xslib])
test('ss_new test', test_ss_new)

test_ss_area = executable('test_ss_area', 
                          'test_ss_area.c',
                          include_directories : [inc, xs_inc],
                          link_with : [ciilib, xslib])
test('ss_area test', test_ss_area)

test_ss_perimeter = executable('test_ss_perimeter', 
                               'test_ss_perimeter.c',
                               include_directories : [inc, xs_inc],
                               link_with : [ciilib, xslib],
                               dependencies : m_dep)
test('ss_perimeter test', test_ss_perimeter)
