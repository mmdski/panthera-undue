subdir('cii')

src_inc = include_directories('.')

hp_sources = ['hydraulicproperties.c']
hplib = library('hp',
                hp_sources,
                include_directories : inc,
                link_with : ciilib)

panthera_sources = ['panthera.c']
pantheralib = library('panthera',
                      panthera_sources,
                      include_directories : inc,
                      link_with : ciilib)

xs_sources = ['crosssection.c', 'coarray.c', 'subsection.c']
xslib = library('xs',
                 xs_sources,
                 include_directories : inc,
                 dependencies : m_dep,
                 link_with : [ciilib, hplib, pantheralib])
