panthera_sources = ['xscoord.c']

if target_machine.system() == 'windows'
    pantheralib = static_library('panthera',
                             [panthera_sources],
                             include_directories : inc,
                             dependencies : [m_dep, glib_dep],
                             name_prefix : '', name_suffix : 'lib')
else
    pantheralib = static_library('panthera',
                             panthera_sources,
                             include_directories : inc,
                             dependencies : [m_dep, glib_dep])
endif
