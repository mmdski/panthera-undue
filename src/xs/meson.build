xs_inc = include_directories('.')
xs_sources = ['coarray.c', 'coordinate.c', 'subsection.c']
xslib = library('xs',
                 xs_sources,
                 include_directories : inc,
                 dependencies : m_dep,
                 link_with : [ciilib, hplib, pantheralib])
