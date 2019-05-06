cii_sources = [
               'cii/assert.c',
               'cii/except.c',
               'cii/list.c',
               'cii/mem.c'
              ]
panthera_sources = [
                    'exceptions.c',
                    'crosssection.c',
                    'coarray.c',
                    'xstable.c',
                    'reach.c',
                    'standardstep.c',
                    'secantsolve.c',
                    cii_sources
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
