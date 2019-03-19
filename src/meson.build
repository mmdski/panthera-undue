subdir('cii')

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

subdir('xs')
