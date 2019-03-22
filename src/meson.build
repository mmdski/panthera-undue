subdir('cii')

panthera_sources = ['panthera.c', 'crosssection.c', 'coarray.c',
                    'subsection.c', 'hydraulicproperties.c']
pantheralib = library('panthera',
                      panthera_sources,
                      include_directories : inc,
                      dependencies : m_dep,
                      link_with : ciilib)
