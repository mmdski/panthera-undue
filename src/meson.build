subdir('cii')

panthera_sources = ['exceptions.c', 'crosssection.c', 'coarray.c', 'xstable.c']
pantheralib = static_library('panthera',
                             panthera_sources,
                             include_directories : inc,
                             dependencies : m_dep,
                             link_with : ciilib)
