include_dirs = [inc, test_inc]
link_libs = [ciilib, pantheralib, testlib]

xstable_test_sources = ['xstable.c']
xstable_mem_test = executable('xstable', xstable_test_sources,
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

reach_test_sources = ['reach.c']
reach_mem_test = executable('reach', reach_test_sources,
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

