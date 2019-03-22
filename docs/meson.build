project('panthera documentation', version: '0.0.0')

hotdoc = import('hotdoc')
documentation = hotdoc.generate_doc(meson.project_name(),
    project_version: meson.project_version(),
    sitemap: 'sitemap.txt',
    build_by_default: true,
    index: 'markdown/index.md',
    install: false,
    include_paths: ['markdown', '../include'],
    syntax_highlighting_activate: true,
)

run_target('upload',
    command: [find_program('hotdoc'), 'run', '--conf-file', documentation.config_path()]
)
