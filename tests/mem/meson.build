xstable_test_sources = ['xstable.c']
xstable_mem_test = executable('xstable', xstable_test_sources,
                              include_directories : [inc],
                              link_with: [ciilib, pantheralib])

test('test_mem_xstable', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             '@0@/xstable'.format(meson.current_build_dir()),
            ]
     )

reach_test_sources = ['reach.c']
reach_mem_test = executable('reach', reach_test_sources,
                              include_directories : [inc],
                              link_with: [ciilib, pantheralib])

test('test_mem_reach', vlgnd,
     args : [
             '--leak-check=full',
             '--errors-for-leak-kinds=all',
             '--error-exitcode=33',
             '@0@/reach'.format(meson.current_build_dir()),
            ]
)
