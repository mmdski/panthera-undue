app = executable('app', 'app.c', include_directories : inc,
                 link_with : [ciilib, pantheralib])
sstep = executable('sstep', 'standardstep.c', include_directories : inc,
                   dependencies : m_dep,
                   link_with : [ciilib, pantheralib])
