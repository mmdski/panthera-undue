coarray_test_sources = ['coarray_test.c']
coarray_testlib = library('coarray_test',
                          coarray_test_sources,
                          include_directories : [inc, xs_inc],
                          link_with : [ciilib, xslib])

test_coarray_new = executable('test_coarray_new', 'test_coarray_new.c',
                              include_directories : [inc, xs_inc],
                              link_with : [ciilib, xslib, coarray_testlib])
test('coarray_new test', test_coarray_new)

test_coarray_subarray = executable('test_coarray_subarray', 
                                   'test_coarray_subarray.c',
                                   include_directories : [inc, xs_inc],
                                   link_with : [ciilib, xslib, coarray_testlib])
test('coarray_subarray test', test_coarray_subarray)

test_coarray_from_array = executable('test_coarray_from_array', 
                                     'test_coarray_from_array.c',
                                     include_directories : [inc, xs_inc],
                                     link_with : [ciilib, xslib, coarray_testlib])
test('coarray_from_array test', test_coarray_from_array)

test_coarray_from_list = executable('test_coarray_from_list', 
                                    'test_coarray_from_list.c',
                                    include_directories : [inc, xs_inc],
                                    link_with : [ciilib, xslib, coarray_testlib])
test('coarray_from_list test', test_coarray_from_list)
