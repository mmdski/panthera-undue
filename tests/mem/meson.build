include_dirs = [inc, test_inc]
link_libs = [pantheralib, testlib]

mem_test_src = ['memtest.c', 'crosssection.c', 'reach.c', 'xstable.c',
                'standardstep.c']

mem_test = executable('mem_test', mem_test_src,
                         include_directories : include_dirs,
                         link_with : link_libs)
test('mem_test', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             mem_test.full_path()
            ]
     )
