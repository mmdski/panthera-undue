include_dirs = [inc, test_inc]
link_libs = [ciilib, pantheralib, testlib]

xstable_mem_test = executable('xstable', 'xstable.c',
                              include_directories : include_dirs,
                              link_with: link_libs)
test('mem_test_xstable', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             '@0@/xstable'.format(meson.current_build_dir()),
            ]
     )

reach_mem_test = executable('reach', 'reach.c',
                              include_directories : include_dirs,
                              link_with: link_libs)
test('mem_test_reach', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             '@0@/reach'.format(meson.current_build_dir()),
            ]
)

sstep_mem_test = executable('sstep', 'standardstep.c',
                            include_directories : include_dirs,
                            dependencies : m_dep,
                            link_with: link_libs)
test('mem_test_sstep', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             '@0@/sstep'.format(meson.current_build_dir()),
            ]
)
