subdir('cii')

panthera_sources = [
                    'exceptions.c',
                    'crosssection.c',
                    'coarray.c',
                    'xstable.c',
                    'reach.c',
                    'standardstep.c',
                    'secantsolve.c'
                    ]

if target_machine.system() == 'windows'
    pantheralib = static_library('panthera',
                             panthera_sources,
                             include_directories : inc,
                             dependencies : m_dep,
                             link_with : ciilib,
                             name_prefix : '', name_suffix : 'lib')
else
    pantheralib = static_library('panthera',
                             panthera_sources,
                             include_directories : inc,
                             dependencies : m_dep,
                             link_with : ciilib)
endif
