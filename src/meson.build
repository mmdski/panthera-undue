subdir('cii')

panthera_sources = ['exceptions.c', 'crosssection.c', 'coarray.c']
pantheralib = library('panthera',
                      panthera_sources,
                      include_directories : inc,
                      dependencies : m_dep,
                      link_with : ciilib)
