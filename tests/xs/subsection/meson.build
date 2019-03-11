test_subsection_new = executable('test_subsection_new', 
                                 'test_subsection_new.c',
                                 include_directories : [inc, xs_inc],
                                 link_with : [ciilib, xslib])
test('subsection_new test', test_subsection_new)

test_subsection_area = executable('test_subsection_area', 
                                 'test_subsection_area.c',
                                 include_directories : [inc, xs_inc],
                                 link_with : [ciilib, xslib])
test('subsection_area test', test_subsection_area)

test_subsection_perimeter = executable('test_subsection_perimeter', 
                                       'test_subsection_perimeter.c',
                                       include_directories : [inc, xs_inc],
                                       link_with : [ciilib, xslib],
                                       dependencies : m_dep)
test('subsection_perimeter test', test_subsection_perimeter)
