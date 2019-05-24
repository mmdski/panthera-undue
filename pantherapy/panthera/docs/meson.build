project('panthera docs', version: '0.0.0')

sources = ['../include/panthera/*.h']

hotdoc = import('hotdoc')
documentation = hotdoc.generate_doc(meson.project_name(),
    git_repo : '..',
    project_version: meson.project_version(),
    sitemap: 'sitemap.txt',
    build_by_default: false,
    index: 'markdown/index.md',
    c_smart_index : true,
    c_sources : sources,
    c_include_directories : '@0@/../include'.format(meson.current_source_dir()),
    c_index : 'markdown/c-index.md',
    install: false,
    languages : ['c'],
    include_paths: ['markdown'],
    syntax_highlighting_activate: true,
    git_upload_repository: 'git@github.com:mmdski/panthera-undue-docs.git',
    edit_on_github_repository: 'https://github.com/mmdski/panthera-undue/',
)

run_target('upload',
    command: [find_program('hotdoc'), 'run', '--conf-file',
              documentation.config_path()]
)
