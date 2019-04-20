include_dirs = [inc, test_inc]
link_libs = [ciilib, pantheralib, testlib]

xstable_mem_test = executable('xstable_mem_test', 'xstable.c',
                              include_directories : include_dirs,
                              link_with: link_libs)
test('mem_test_xstable', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
	     xstable_mem_test.full_path()
            ]
     )

reach_mem_test = executable('reach_mem_test', 'reach.c',
                              include_directories : include_dirs,
                              link_with: link_libs)
test('mem_test_reach', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
	     reach_mem_test.full_path()
            ]
)

sstep_mem_test = executable('sstep_mem_test', 'standardstep.c',
                            include_directories : include_dirs,
                            dependencies : m_dep,
                            link_with: link_libs)
test('mem_test_sstep', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
	     sstep_mem_test.full_path()
            ]
)
