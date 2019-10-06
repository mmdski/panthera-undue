panthera_sources = [
                    'coarray.c',
                    'coordinate.c',
                    'crosssection.c',
                    'list.c',
                    'mem.c',
                    'reach.c',
                    'redblackbst.c',
                    'secantsolve.c',
                    'subsection.c',
                    'xsproperties.c'
                    ]

if target_machine.system() == 'windows'
    pantheralib = static_library('panthera',
                             [panthera_sources],
                             include_directories : inc,
                             dependencies : m_dep,
                             name_prefix : '', name_suffix : 'lib')
else
    pantheralib = static_library('panthera',
                             panthera_sources,
                             include_directories : inc,
                             dependencies : m_dep)
endif
