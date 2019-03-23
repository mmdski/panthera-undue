project('panthera documentation', version: '0.0.0')

sources = ['../include/*.h', '../include/panthera/*.h']

hotdoc = import('hotdoc')
documentation = hotdoc.generate_doc(meson.project_name(),
    project_version: meson.project_version(),
    sitemap: 'sitemap.txt',
    build_by_default: true,
    index: 'markdown/index.md',
    c_smart_index : true,
    c_sources : sources,
    c_include_directories : '../include',
    install: false,
    languages : ['c'],
    include_paths: ['markdown'],
    syntax_highlighting_activate: true,
)

run_target('upload',
    command: [find_program('hotdoc'), 'run', '--conf-file',
              documentation.config_path()]
)
