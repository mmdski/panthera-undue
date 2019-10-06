mem_test_src = [
    'mem_test.c',
    'coarray.c',
    'crosssection.c',
    'list.c',
    'reach.c',
    'subsection.c'
    ]

mem_test = executable('mem_test', mem_test_src,
    include_directories : [inc, src_inc],
    link_with : pantheralib)

test('mem_test', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             mem_test.full_path()
            ]
     )
